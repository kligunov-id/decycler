#include <algorithm>
#include <iostream>
#include <numeric>
#include <random>
#include <ranges>

#include "CLI11.hpp"

#include "log.hpp"
#include "graph.hpp"

struct Config {
    int verbose = 0;
    double jitter = 2;
    bool sort = false; // <- on by default
    bool dry = false;
};

class Evaluator {
public:

    Evaluator(Graph& graph): graph(graph), states(graph.n, NONE) {}

    std::vector<Node> evaluate(const std::vector<double> &priorities) {
        std::vector<Node> picking_order(graph.n);
        std::iota(picking_order.begin(), picking_order.end(), 0);
        sort_nodes_by_priority(picking_order, priorities);
        for (auto &out_links: graph.out_links) {
            sort_nodes_by_priority(out_links, priorities);
        }
        Log::info("Seed picking order: {}", picking_order);
        Log::info("Outlinks: {}", graph.out_links[0]);
        Log::info("Outlinks: {}", graph.out_links[1]);
        Log::info("Outlinks: {}", graph.out_links[2]);
        Log::info("Outlinks: {}", graph.out_links[3]);

        for (Node u = 0; u < graph.n; u++) {
            if (states[u] == NONE) {
                add_node(u);
            }
        }
        std::vector<Node> skipped;
        for (Node u = 0; u < graph.n; u++) {
            if (states[u] == SKIPPED) {
                skipped.push_back(u);
            }
        }
        return skipped;
    }

private:
    Graph& graph;

    enum State {
        SKIPPED,
        NONE,
        VISITING,
        VISITED
    };

    std::vector<State> states;

    void sort_nodes_by_priority(std::vector<Node> &nodes, const std::vector<double> &priorities) {
        std::sort(nodes.begin(), nodes.end(), [&](Node u, Node v) { return priorities[u] < priorities[v]; });
    }

    bool is_acyclic_with(Node u) {
        for (Node v: graph.out_links[u]) {
            if (states[v] == VISITING) {
                return false;
            }
        }
        return true;
    }

    void add_node(Node u) {
        Log::debug("Adding node {}", u);
        states[u] = VISITING;
        for (Node v: graph.out_links[u]) {
            if (states[v] == VISITING) {
                Log::error("Fatal eror, edge {}->{} is inside a cycle", v, u);
            }
            if (states[v] == NONE) {
                if (is_acyclic_with(v)) {
                    add_node(v);
                } else {
                    Log::debug("Skipping node {}", v);
                    states[v] = SKIPPED;
                }
            }
        }
        states[u] = VISITED;
    }
};

std::vector<double> generate_initial_priorities(const Graph &graph, const Config &config) {
    std::vector<double> priorities(graph.n, 0);
    if (config.sort) {
        for (auto [u, deg]: std::views::enumerate(graph.total_deg)) {
            priorities[u] = deg;
        }
    }
    if (config.jitter > 0) {
        std::mt19937 engine(std::random_device{}());
        std::uniform_real_distribution<> generator(-config.jitter, config.jitter);
        for (auto &priority: priorities) {
            priority += generator(engine);
        }
    }
    return priorities;
}

void solve(Graph &graph, const Config &config) { // <- reseed + local opt + sigterm
    Log::info("Graph: n={}, m={}", graph.n, graph.m);

    std::vector<double> priorities = generate_initial_priorities(graph, config);
    Log::info("Priorities: {}", priorities);
    const std::vector<Node> skipped = Evaluator(graph).evaluate(priorities);

    if (!config.dry) {
        for (Node u: skipped) {
            std::cout << u + 1 << "\n";
        }
    }

    Log::info("Total nodes skipped: {}, skipped nodes: {}", skipped.size(), skipped);
}

int main(int argc, char **argv) {
    Config config;

    CLI::App app;
    app.add_flag("-v", config.verbose, "Print more info, -vv for full debug output");
    app.add_flag("-d", config.dry, "Do not print the answer");

    app.add_flag("-s", config.sort, "Set node order priorites equal to node degrees");
    app.add_option("-j", config.jitter, "Specify node priority variation");

    CLI11_PARSE(app, argc, argv);

    if (config.verbose) {
        Log::init(config.verbose > 1 ? Log::Level::DEBUG : Log::Level::INFO);
    }

    Graph graph;
    std::cin >> graph;
    solve(graph, config);
    return 0;
}
