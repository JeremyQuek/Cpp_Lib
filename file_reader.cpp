#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <deque>
#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <numeric>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <condition_variable>
#include <functional>

class ThreadPool {
    std::vector<std::thread> workers;
    std::deque<std::function<void()>> tasks;
    std::mutex mtx;
    std::condition_variable cv;
    bool stop;
public:
    ThreadPool(size_t n) : stop(false) {
        for(size_t i=0;i<n;++i) {
            workers.emplace_back([this](){
                while(true){
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(mtx);
                        cv.wait(lock,[this](){return stop || !tasks.empty();});
                        if(stop && tasks.empty()) return;
                        task=std::move(tasks.front());
                        tasks.pop_front();
                    }
                    task();
                }
            });
        }
    }
    void enqueue(std::function<void()> f){
        {
            std::lock_guard<std::mutex> lock(mtx);
            tasks.push_back(std::move(f));
        }
        cv.notify_one();
    }
    ~ThreadPool(){
        {
            std::lock_guard<std::mutex> lock(mtx);
            stop=true;
        }
        cv.notify_all();
        for(auto& w: workers) w.join();
    }
};

struct DataSet {
    std::vector<std::string> headers;
    std::vector<std::vector<double>> data;
    void load_csv(const std::string& path){
        std::ifstream f(path);
        if(!f) return;
        std::string line;
        if(std::getline(f,line)){
            std::stringstream ss(line);
            std::string h;
            while(std::getline(ss,h,',')) headers.push_back(h);
        }
        while(std::getline(f,line)){
            std::stringstream ss(line);
            std::string val;
            std::vector<double> row;
            while(std::getline(ss,val,',')){
                if(val.empty()) row.push_back(NAN);
                else row.push_back(std::stod(val));
            }
            data.push_back(row);
        }
    }
    std::vector<double> column(size_t idx){
        std::vector<double> col;
        for(auto& r:data) if(idx<r.size() && !std::isnan(r[idx])) col.push_back(r[idx]);
        return col;
    }
    void filter(std::function<bool(const std::vector<double>&)> cond){
        std::vector<std::vector<double>> new_data;
        for(auto& r:data) if(cond(r)) new_data.push_back(r);
        data = std::move(new_data);
    }
    void sort_column(size_t idx){
        std::sort(data.begin(),data.end(),[idx](const std::vector<double>& a,const std::vector<double>& b){
            double av = idx<a.size()?a[idx]:NAN;
            double bv = idx<b.size()?b[idx]:NAN;
            if(std::isnan(av)) return false;
            if(std::isnan(bv)) return true;
            return av<bv;
        });
    }
};

struct Stats {
    double mean, median, stddev, min, max;
};

Stats compute_stats(std::vector<double> v){
    if(v.empty()) return {NAN,NAN,NAN,NAN,NAN};
    std::sort(v.begin(),v.end());
    double m = std::accumulate(v.begin(),v.end(),0.0)/v.size();
    double med = v.size()%2?v[v.size()/2]:(v[v.size()/2-1]+v[v.size()/2])/2.0;
    double s = 0;
    for(auto x:v) s+=(x-m)*(x-m);
    s = std::sqrt(s/v.size());
    double mn = *std::min_element(v.begin(),v.end());
    double mx = *std::max_element(v.begin(),v.end());
    return {m,med,s,mn,mx};
}

int main(){
    DataSet ds;
    ds.load_csv("data.csv");
    ds.filter([](const std::vector<double>& row){
        for(auto v:row) if(!std::isnan(v) && v<0) return false;
        return true;
    });
    ThreadPool pool(std::thread::hardware_concurrency());
    std::vector<Stats> results(ds.headers.size());
    std::mutex res_mtx;
    for(size_t i=0;i<ds.headers.size();++i){
        pool.enqueue([i,&ds,&results,&res_mtx](){
            auto col = ds.column(i);
            auto s = compute_stats(col);
            std::lock_guard<std::mutex> lock(res_mtx);
            results[i]=s;
        });
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::ofstream out("summary_report.txt");
    out<<"Column,Mean,Median,StdDev,Min,Max\n";
    for(size_t i=0;i<ds.headers.size();++i){
        std::cout<<"Column: "<<ds.headers[i]<<" Mean:"<<results[i].mean
                 <<" Median:"<<results[i].median<<" StdDev:"<<results[i].stddev
                 <<" Min:"<<results[i].min<<" Max:"<<results[i].max<<"\n";
        out<<ds.headers[i]<<","<<results[i].mean<<","<<results[i].median<<","<<results[i].stddev
           <<","<<results[i].min<<","<<results[i].max<<"\n";
    }
    ds.sort_column(0);
    std::ofstream sorted("sorted_data.csv");
    for(size_t i=0;i<ds.headers.size();++i){
        sorted<<ds.headers[i];
        if(i+1<ds.headers.size()) sorted<<",";
    }
    sorted<<"\n";
    for(auto& r:ds.data){
        for(size_t i=0;i<r.size();++i){
            sorted<<r[i];
            if(i+1<r.size()) sorted<<",";
        }
        sorted<<"\n";
    }
    return 0;
}
