#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <unistd.h>

#include <atomic>
#include <condition_variable>
#include <cstring>
#include <deque>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <mutex>
#include <queue>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

class ThreadPool {
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex mtx;
    std::condition_variable cv;
    bool stop;

public:
    ThreadPool(size_t n) : stop(false) {
        for (size_t i = 0; i < n; ++i) {
            workers.emplace_back([this]() {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(mtx);
                        cv.wait(lock, [this]() { return stop || !tasks.empty(); });
                        if (stop && tasks.empty()) return;
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
        }
    }

    void enqueue(std::function<void()> f) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            tasks.push(std::move(f));
        }
        cv.notify_one();
    }

    ~ThreadPool() {
        {
            std::lock_guard<std::mutex> lock(mtx);
            stop = true;
        }
        cv.notify_all();
        for (auto& w : workers) w.join();
    }
};

int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

std::string http_response(const std::string& body, const std::string& type="text/plain") {
    std::ostringstream oss;
    oss << "HTTP/1.1 200 OK\r\n";
    oss << "Content-Type: " << type << "\r\n";
    oss << "Content-Length: " << body.size() << "\r\n";
    oss << "Connection: close\r\n\r\n";
    oss << body;
    return oss.str();
}

std::string http_not_found() {
    std::string b = "404 Not Found";
    return http_response(b);
}

std::string http_bad_request() {
    std::string b = "400 Bad Request";
    return http_response(b);
}

std::string json_kv(const std::string& k, const std::string& v) {
    return "\"" + k + "\":\"" + v + "\"";
}

std::string json_obj(const std::vector<std::string>& items) {
    std::ostringstream oss;
    oss << "{";
    for (size_t i=0;i<items.size();++i) {
        oss << items[i];
        if (i+1<items.size()) oss << ",";
    }
    oss << "}";
    return oss.str();
}

std::unordered_map<std::string, std::function<std::string(const std::string&)>> routes;

std::string read_file(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) return "";
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

std::string handle_request(const std::string& req) {
    std::istringstream iss(req);
    std::string method, path, version;
    iss >> method >> path >> version;
    if (method != "GET") return http_bad_request();
    if (routes.count(path)) {
        return http_response(routes[path](path), "application/json");
    }
    if (path.find("/static/") == 0) {
        std::string file = "." + path;
        std::string data = read_file(file);
        if (data.empty()) return http_not_found();
        return http_response(data, "text/plain");
    }
    return http_not_found();
}

class Server {
    int listen_fd;
    int epoll_fd;
    ThreadPool pool;
    std::atomic<bool> running;

public:
    Server(int port, int threads) : pool(threads), running(true) {
        listen_fd = socket(AF_INET, SOCK_STREAM, 0);
        int opt = 1;
        setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);
        bind(listen_fd, (sockaddr*)&addr, sizeof(addr));
        listen(listen_fd, 128);
        set_nonblocking(listen_fd);
        epoll_fd = epoll_create1(0);
        epoll_event ev{};
        ev.events = EPOLLIN;
        ev.data.fd = listen_fd;
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev);
    }

    void run() {
        epoll_event events[64];
        while (running) {
            int n = epoll_wait(epoll_fd, events, 64, -1);
            for (int i=0;i<n;i++) {
                if (events[i].data.fd == listen_fd) {
                    accept_loop();
                } else {
                    int client = events[i].data.fd;
                    pool.enqueue([this, client]() { handle_client(client); });
                }
            }
        }
    }

    void accept_loop() {
        while (true) {
            sockaddr_in caddr{};
            socklen_t len = sizeof(caddr);
            int client = accept(listen_fd, (sockaddr*)&caddr, &len);
            if (client < 0) break;
            set_nonblocking(client);
            epoll_event ev{};
            ev.events = EPOLLIN | EPOLLET;
            ev.data.fd = client;
            epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client, &ev);
        }
    }

    void handle_client(int fd) {
        char buf[4096];
        std::string req;
        while (true) {
            ssize_t r = recv(fd, buf, sizeof(buf), 0);
            if (r <= 0) break;
            req.append(buf, r);
            if (req.find("\r\n\r\n") != std::string::npos) break;
        }
        std::string resp = handle_request(req);
        send(fd, resp.c_str(), resp.size(), 0);
        close(fd);
    }
};

int main() {
    routes["/ping"] = [](const std::string&) {
        return json_obj({json_kv("status","ok"), json_kv("msg","pong")});
    };

    routes["/time"] = [](const std::string&) {
        time_t t = time(nullptr);
        return json_obj({json_kv("epoch", std::to_string(t))});
    };

    routes["/about"] = [](const std::string&) {
        return json_obj({
            json_kv("server","C++ Epoll Server"),
            json_kv("version","1.0"),
            json_kv("author","Jeremy")
        });
    };

    Server s(8080, std::thread::hardware_concurrency());
    s.run();
    return 0;
}
