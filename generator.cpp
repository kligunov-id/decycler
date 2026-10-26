#include <iostream>
#include <random>

#include "CLI11.hpp"

#include "log.hpp"
#include "graph.hpp"

struct Config {
    int32_t verbose = 0;
    size_t n;
    size_t m;
    int32_t seed = -1;
};


Graph generate(const Config &config) { // <- reseed + local opt + sigterm
    Log::info("Graph: n={}, m={}", config.n, config.m);

    std::mt19937 random_engine(config.seed > 0 ? config.seed : std::random_device{}());

    std::uniform_int_distribution<> node_generator(0, config.n - 1);

    Graph graph{.n = config.n, .m=config.m};
    graph.out_links.resize(graph.n);
    return graph;
}

int main(int argc, char **argv) {
    Config config;

    CLI::App app;
    app.add_flag("-v,--verbose", config.verbose, "Print more info, -vv for full debug output");

    app.add_option("-n,n", config.n, "Node number");
    app.add_option("-m,m", config.m, "Edge number");
    app.add_option("-s,--seed", config.seed, "Use specified random seed");

    CLI11_PARSE(app, argc, argv);

    if (config.verbose) {
        Log::init(config.verbose > 1 ? Log::Level::DEBUG : Log::Level::INFO);
    }

    const Graph graph = generate(config);
    std::cout << graph;
    return 0;
}
