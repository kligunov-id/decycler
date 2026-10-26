#include <algorithm>
#include <iostream>
#include <fstream>
#include <iterator>
#include <random>

#include "CLI11.hpp"

#include "log.hpp"
#include "graph.hpp"

struct Config {
    int32_t verbose = 0;
    size_t n;
    size_t k;
    int32_t seed = -1;
    std::string file;
};


Graph generate(const Config &config) { // <- reseed + local opt + sigterm
    Graph graph{.n = config.n, .m=config.n * config.k};
    Log::info("Graph: n={}, m={}", graph.n, graph.m);

    std::mt19937 random_engine(config.seed > 0 ? config.seed : std::random_device{}());

    //std::uniform_int_distribution<> node_generator(0, graph.n - 1);

    graph.out_links.resize(graph.n);
    graph.in_links.resize(graph.n);
    graph.total_deg.resize(graph.n);
    std::vector<Node> other_nodes(graph.n - 1);
    std::iota(other_nodes.begin(), other_nodes.end(), 1);
    for (Node u = 0; u < graph.n; u++) {
        std::sample(
            other_nodes.begin(),
            other_nodes.end(),
            std::back_inserter(graph.out_links[u]),
            config.k,
            random_engine
        );
        for (Node v: graph.out_links[u]) {
            graph.total_deg[v]++;
            graph.total_deg[u]++;
            graph.in_links[v].push_back(u);
        }
        if (u != graph.n - 1) {
            other_nodes[u] = u;
        }
    }
    return graph;
}

int main(int argc, char **argv) {
    Config config;

    CLI::App app;
    app.add_flag("-v,--verbose", config.verbose, "Print more info, -vv for full debug output");

    app.add_option("-n,n", config.n, "Node number")
        ->check(CLI::Range(3, 200'000))
        ->required();
    app.add_option("-k,k", config.k, "Node out degree")
        ->check(CLI::Range(1, 1000))
        ->required();
    app.add_option("-s,--seed", config.seed, "Use specified random seed");
    app.add_option("-f,--file", config.file, "Save graph to file");

    CLI11_PARSE(app, argc, argv);

    if (config.verbose) {
        Log::init(config.verbose > 1 ? Log::Level::DEBUG : Log::Level::INFO);
    }

    const Graph graph = generate(config);
    if (config.file == "") {
        std::cout << graph;
    } else {
        std::ofstream out(config.file);
        out << graph;
    }
    return 0;
}
