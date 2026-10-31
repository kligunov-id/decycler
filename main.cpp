#include <algorithm>
#include <csignal>
#include <cmath>
#include <iostream>
#include <limits>
#include <numeric>
#include <random>
#include <ranges>
#include <utility>

#include "CLI11.hpp"

#include "log.hpp"
#include "graph.hpp"

bool caught_sigterm = false;

void handle_sigterm(int signal) {
    caught_sigterm = true;
}

struct Config {
    int32_t verbose = 0;
    double jitter = 2;
    bool no_priorities = false;
    bool dry = false;
    size_t iterations = std::numeric_limits<std::size_t>::max();
    int32_t seed = -1;
    int32_t swap_parts = 1000;
    double T_max = 0;
    double q = 0.99;
};

class Evaluator {
public:

    Evaluator(Graph& graph): graph(graph), states(graph.n, NONE), index(graph.n, 0) {
        queue.reserve(graph.n);
    }

    std::vector<Node> evaluate(const std::vector<double> &priorities) {
        std::vector<Node> picking_order(graph.n);
        std::iota(picking_order.begin(), picking_order.end(), 0);
        sort_nodes_by_priority(picking_order, priorities);
        for (auto &out_links: graph.out_links) {
            sort_nodes_by_priority(out_links, priorities);
        }
        Log::debug("Seed picking order: {}", picking_order);

        std::fill(states.begin(), states.end(), NONE);
        std::fill(index.begin(), index.end(), 0);

        for (Node u = 0; u < graph.n; u++) {
            if (states[u] == NONE) {
                add_nodes(u);
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
    std::vector<size_t> index; // dirtiest hack
    std::vector<Node> queue;

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

void process_node(Node u) {
        while (index[u] != graph.out_links[u].size()) {
            Node v = graph.out_links[u][index[u]++];
            if (states[v] == VISITED || states[v] == SKIPPED) {
                continue;
            }
            if (is_acyclic_with(v)) {
                queue.push_back(v);
                states[v] = VISITING;
                return;
            }
            states[v] = SKIPPED;
        }
        queue.pop_back();
        states[u] = VISITED;
    }

    void add_nodes(Node u) {
        queue.push_back(u);
        states[u] = VISITING;
        while (!queue.empty()) {
            process_node(queue.back());
        }
    }
};

std::vector<double> generate_initial_priorities(const Graph &graph, const Config &config, auto &random_engine) {
    std::vector<double> priorities(graph.n, 0);
    if (!config.no_priorities) {
        for (auto [u, deg]: std::views::enumerate(graph.total_deg)) {
            priorities[u] = deg;
        }
    }
    if (config.jitter > 0) {
        std::uniform_real_distribution<> generator(-config.jitter, config.jitter);
        for (auto &priority: priorities) {
            priority += generator(random_engine);
        }
    }
    return priorities;
}

void solve(Graph &graph, const Config &config) { // <- reseed + local opt + sigterm
    Log::info("Graph: n={}, m={}", graph.n, graph.m);

    std::mt19937 random_engine(config.seed > 0 ? config.seed : std::random_device{}());

    std::vector<double> priorities = generate_initial_priorities(graph, config, random_engine);

    Evaluator evaluator(graph);
    std::vector<Node> skipped = evaluator.evaluate(priorities);

    std::uniform_int_distribution<> node_generator(0, graph.n - 1);
    double T = config.T_max;
    for (size_t iteration = 0; iteration < config.iterations && !caught_sigterm; iteration++) {
        std::vector<double> new_priorities = priorities;

        for (size_t swap_count = 0; swap_count * config.swap_parts < graph.n; swap_count++) {
            Node u = node_generator(random_engine), v = node_generator(random_engine);
            std::swap(new_priorities[u], new_priorities[v]);
        }

        std::vector<Node> new_skipped = evaluator.evaluate(new_priorities);
        double delta_E = static_cast<double>(new_skipped.size()) - skipped.size(); // without cast negative would overflow to positive
        double transition_prob = std::min(1.0, std::exp(-delta_E / T));
        if (std::bernoulli_distribution{transition_prob}(random_engine)) {
            std::swap(new_skipped, skipped);
            std::swap(new_priorities, priorities);
        }
        T *= config.q;
        Log::info("Temp: {:.2f}, score: {}, candidate score: {}, p: {:.3f}", T, skipped.size(), new_skipped.size(), transition_prob); // candidate score is not correct, but I don't care
    }

    if (!config.dry) {
        for (Node u: skipped) {
            std::cout << u + 1 << "\n";
        }
    }

    Log::debug("Total nodes skipped: {}, skipped nodes: {}", skipped.size(), skipped);
}

int main(int argc, char **argv) {
    std::signal(SIGTERM, handle_sigterm);
    std::signal(SIGINT, handle_sigterm);

    Config config;

    CLI::App app;
    app.add_flag("-v,--verbose", config.verbose, "Print more info, -vv for full debug output");
    app.add_flag("-d,--dry", config.dry, "Do not print the answer");

    app.add_flag("-p,--no-priorities", config.no_priorities, "Disable degree-based ordering heuristic. Warning: do not use without jitter");
    app.add_option("-j,--jitter", config.jitter, "Specify node priority variation")->capture_default_str();
    app.add_option("-i,--iterations", config.iterations, "Number of simulation runs");
    app.add_option("-s,--seed", config.seed, "Use specified random seed");
    app.add_option("-m,--mutation", config.swap_parts, "Mutate 1 in every X node")->capture_default_str();
    app.add_option("-t,--temp", config.T_max, "Inital temperature")->capture_default_str();
    app.add_option("-q", config.q, "Cooling factor")->capture_default_str();

    CLI11_PARSE(app, argc, argv);

    if (config.verbose) {
        Log::init(config.verbose > 1 ? Log::Level::DEBUG : Log::Level::INFO);
    }

    Graph graph;
    std::cin >> graph;
    solve(graph, config);
    return 0;
}
