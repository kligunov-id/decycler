#include <iostream>
#include <bitset>

#include "CLI11.hpp"

#include "log.hpp"
#include "graph.hpp"

struct Config {
    bool verbose = false;
};

void solve(Graph graph, const Config &config) {
    Log::info("Graph: n={}, m={}", graph.n, graph.m);
    std::vector<std::vector<bool>> achievable_from(graph.n, std::vector<bool>(graph.n, false));
}

int main(int argc, char **argv) {
    Config config;

    CLI::App app;
    app.add_flag("-v", config.verbose, "Print debug info");
    CLI11_PARSE(app, argc, argv);

    Log::init(config.verbose);

    Graph graph;
    std::cin >> graph;
    solve(graph, config);
    return 0;
}
