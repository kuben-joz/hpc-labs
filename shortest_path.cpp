#include <bits/stdc++.h>
#include "tbb/tbb.h"
#include <iostream>

using namespace std;

typedef pair<long long, int> pii;

typedef std::vector<vector<pii>> graph_t;

typedef tbb::concurrent_priority_queue<pii, greater<pii>> heap_t;

typedef tbb::concurrent_hash_map<int, long long> map_t;

void rand_init_graph(graph_t &graph, int node_count,
                     double edge_probability)
{
    graph.resize(node_count);
    std::random_device rd;
    std::mt19937 gen{rd()};
    std::uniform_real_distribution<> dis{0, 1};
    uniform_int_distribution<> dis_w{1, 1000};
    for (int i = 0; i < node_count; ++i)
    {
        auto neighbors = std::vector<pii>();
        for (int j = 0; j < i; ++j)
        {
            if (dis(gen) < edge_probability)
            {
                int w = dis_w(gen);
                neighbors.emplace_back(j, w);
                graph[j].emplace_back(i, w);
            }
        }
        graph[i] = neighbors;
    }
}

map_t dijkstra_par(graph_t &G, int N)
{
    heap_t heap;
    heap.emplace(0, 0);
    map_t map;
    for (int i = 0; i < N; i++)
    {
        map_t::accessor a;
        map.insert(a, i);
        a->second = i ? 0 : INT_MAX;
    }
    while (!heap.empty())
    {
        pii current = make_pair(0, 0);
        if (!heap.try_pop(current))
        {
            cout << "race condition" << endl;
            exit(1);
        }
        int w = current.first;
        int v = current.second;
        {
            map_t::accessor a;
            if (w != map.find(a, v))
            {
                continue;
            }
        }
        tbb::parallel_for(tbb::blocked_range<long>(0, G[v].size()), [&G, &map, &heap, v, w](const tbb::blocked_range<long> &r)
                          {
            vector<pii>::const_iterator it = G[v].cbegin()+r.begin();
            for(long i = r.begin(); i  < r.end(); i++) {
                int v_neigh = (*it).first;
                int w_neigh = (*it).second;
                {
                    map_t::accessor a;
                    map.find(a, v_neigh);
                    if(w+w_neigh < a->second) {
                        a->second = w+w_neigh;
                        heap.emplace(a->second, v_neigh);
                    }
                }
                it++;
            } });
    }
    return map;
}

map<int, long long> dijkstra_seq(graph_t &G, int N)
{
    priority_queue<pii, vector<pii>, greater<pii>> heap;
    map<int, long long> map;
    for (int i = 0; i < N; i++)
    {
        map[i] = i ? 0 : LONG_LONG_MAX;
    }
    heap.emplace(0, 0);
    while (!heap.empty())
    {
        int w = heap.top().first;
        int v = heap.top().second;
        heap.pop();
        if (w != map[v])
        {
            continue;
        }
        for (auto edge : G[v])
        {
            int v_neigh = edge.first;
            int w_neigh = edge.second;
            if (w + w_neigh < map[v_neigh])
            {
                map[v_neigh] = w + w_neigh;
                heap.emplace(v_neigh, w + w_neigh);
            }
        }
    }
    return map;
}

int main()
{
    graph_t G;
    int num_nodes = 1'000'000;

    //------------------- par dijkstra -----------------------------------------

    tbb::tick_count par_start_time = tbb::tick_count::now();
    map_t map_par = dijkstra_par(G, num_nodes);
    tbb::tick_count par_end_time = tbb::tick_count::now();
    double par_time = (par_end_time - par_start_time).seconds();
    std::cout << "par time: " << par_time << "[s]" << std::endl;

    // ----------------------- seq dijkstra ------------------------------------
    tbb::tick_count seq_start_time = tbb::tick_count::now();
    map<int, long long> map_seq = dijkstra_seq(G, num_nodes);
    tbb::tick_count seq_end_time = tbb::tick_count::now();
    double seq_time = (seq_end_time - seq_start_time).seconds();
    std::cout << "seq time: " << seq_time << "[s]" << std::endl;
    //--------------------- chekc solutions equal ------------------------------
    for(int i = 0; i < num_nodes; i++) {
        map_t::accessor a;
        map_par.find(a, i);
        long long d_par = a->second;
        long long d_seq = map_seq[i];
        if(d_par != d_seq) {
            cout << "Mismatch for node " << i << " par " << d_par << " seq " << d_seq << endl;
            return 1;
        }
    }
    cout << "all good" << endl;
    return 0;
}
