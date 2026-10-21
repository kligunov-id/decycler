#include "CLI11.hpp"

#include <iostream>

struct Config {
    bool verbose = false;
};

struct Graph {
    int n, m;
};

std::istream& operator>>(std::istream& in, Graph& graph) {
    int blank;
    in >> graph.n >> graph.m >> blank;
    return in;
}

void solve(Graph G, const Config &config) {
    std::cout << G.n << " " << G.m << "\n";
    if (config.verbose) { std::cout << "hi!\n"; }
}

int main(int argc, char **argv) {
    Config config;

    CLI::App app;
    app.add_flag("-v", config.verbose, "Print debug info");
    CLI11_PARSE(app, argc, argv);

    Graph graph;
    std::cin >> graph;
    solve(graph, config);
    return 0;
}
