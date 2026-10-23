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

class Evaluator {
public:

    Evaluator(const Graph& graph): graph(graph), states(graph.n, NONE) {}

    std::vector<Node> evaluate() {
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
    const Graph& graph;

    enum State {
        SKIPPED,
        NONE,
        VISITING,
        VISITED
    };

    std::vector<State> states;

    bool is_acyclic_with(Node u) {
        for (Node v: graph.out_links[u]) {
            if (states[v] == VISITING) {
                return false;
            }
        }
        return true;
    }

    void add_node(Node u) {
        Log::info("Adding node {}", u);
        states[u] = VISITING;
        for (Node v: graph.out_links[u]) {
            if (states[v] == VISITING) {
                Log::info("Fatal eror, edge {}->{} is inside a cycle", v, u);
            }
            if (states[v] == NONE) {
                if (is_acyclic_with(v)) { // <- do not check twice
                    add_node(v);
                } else {
                    Log::info("Skipping node {}", v);
                    states[v] = SKIPPED;
                }
            }
        }
        states[u] = VISITED;
    }
};

void solve(Graph graph, const Config &config) { // <- reseed + local opt? + sigterm
    Log::info("Graph: n={}, m={}", graph.n, graph.m);

    const auto skipped = Evaluator(graph).evaluate();
    size_t skipped_count = 0;

    for (Node u: skipped) {
        skipped_count++;
        std::cout << u + 1 << "\n";
    }

    Log::info("Total nodes skipped: {}, skipped nodes: {}", skipped_count, skipped);
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
