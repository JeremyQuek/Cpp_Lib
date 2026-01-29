#include <unordered_map>
#include <vector>
#include <queue>
#include <deque>

using namespace std;
int bfs(const std::vector<std::vector<int>>& edges, int source) {
    std::unordered_map<int, std::vector<int>> adj_list;
    for (const auto& edge: edges) {
        int u=edge[0], v = edge[1];
        adj_list[u].push_back(v);
        adj_list[v].push_back(u);
    }

    std::unordered_map<int, bool> visited;
    visited[source]=true;
    std::deque<int> q;

    int max_depth = 0;
    q.push_back(source);
    while (!q.empty()) {
        int l = q.size();
        for (int i=0; i<l;i++) {
            int u = q.front();
            q.pop_front();
            for (const auto& v: adj_list[u]) {
                if (!visited[v]) {
                    visited[v] = true;
                    q.push_back(v);
                }
            }
        }
        max_depth++;
    }
    return max_depth;
}


using pairs = pair<int, int>;
int dijkstra(const vector<vector<int>> & edges, int source, int target) {
    unordered_map<int, vector<pairs>> adj;

    for (const auto& edge: edges) {
        int u=edge[0], v=edge[1], w=edge[2];
        adj[u].push_back({v, w});
        adj[v].push_back({u, w});
    }

    unordered_map<int, int> dist;
    priority_queue<pairs, vector<pairs>, greater<pairs>> pq;

    const int INF = numeric_limits<int>::max();
    for (auto const& [u, neighbors] : adj) {
        dist[u] = INF;
        for (auto const& [v, weight] : neighbors) {
            dist[v] = INF;
        }
    }

    dist[source] = 0;
    pq.push({0, source});

    while (!pq.empty()) {
        auto [w, u] = pq.top();
        pq.pop();

        if (w > dist[u]) continue;

        for (auto& [v, w2]: adj[u]) {
            if (w + w2<dist[v]) {
                pq.push({w+w2, v});
                dist[v] = w + w2;
            }
        }
    }
    return dist[target];
}
