#include <algorithm>
#include <iostream>
#include <bitset>

#include "CLI11.hpp"

#include "log.hpp"
#include "graph.hpp"

struct Config {
    bool verbose = false;
    bool shuffle = false; // implement
    bool sort = false; // <- on by default
};

void solve(Graph graph, const Config &config) { // <- reseed + local opt? + sigterm
    Log::info("Graph: n={}, m={}", graph.n, graph.m);

    std::vector<std::vector<bool>> achievable_from(graph.n, std::vector<bool>(graph.n, false)); // <- bitset
    std::vector<bool> included(graph.n, false);
    std::vector<Node> picking_order;
    for (Node u = 0; u < graph.n; u++) {
        picking_order.push_back(u);
    }
    if (config.sort) {
        std::sort(picking_order.begin(), picking_order.end(), [&](Node u, Node v) {
                return graph.total_deg[u] < graph.total_deg[v];
        });
    }
    for (Node u: picking_order) {
        // w_i -> u -> v_i
        std::vector<bool> achievable_from_u(graph.n, false);
        for (Node v: graph.out_links[u]) {
            if (!included[v]) continue;
            achievable_from[u][v] = true;
            for (size_t t = 0; t < graph.n; t++) {
                achievable_from[u][t] = achievable_from[u][t] || achievable_from[v][t];
            }
        }
        bool adds_cycle = false;
        for (Node w: graph.in_links[u]) {
            if (!included[w]) continue;
            for (Node v: graph.out_links[u]) {
                if (!included[v]) continue;
                adds_cycle |= achievable_from[v][w]; // <- need early exit
            }
        }
        if (adds_cycle) {
            Log::info("Skipping node {}", u);
            continue;
        }
        Log::info("Including node {}", u);
        included[u] = true;
        for (Node w: graph.in_links[u]) {
            if (!included[w]) continue;
            achievable_from[w][u] = true;
            for (size_t t = 0; t < graph.n; t++) {
                achievable_from[w][t] = achievable_from[w][t] || achievable_from[u][t];
            }
        }
    }
    int skipped_count = 0;
    for (Node u = 0; u < graph.n; u++) {
        if (!included[u]) {
            skipped_count++;
            std::cout << u + 1 << "\n";
        }
    }
    Log::info("Total nodes skipped {}", skipped_count);
}

int main(int argc, char **argv) {
    Config config;

    CLI::App app;
    app.add_flag("-v", config.verbose, "Print debug info");
    app.add_flag("-s", config.shuffle, "Shuffle nodes before sorting");
    app.add_flag("-o", config.sort, "Pick nodes in the order of their degrees");
    CLI11_PARSE(app, argc, argv);

    if (config.verbose) {
        Log::init(Log::Level::INFO);
    }

    Graph graph;
    std::cin >> graph;
    solve(graph, config);
    return 0;
}
