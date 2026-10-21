#include <iostream>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>
#include <bitset>

#include "CLI11.hpp"

#include "log.hpp"

struct Config {
    bool verbose = false;
};

struct Graph {
    size_t n, m;
    std::vector<std::vector<size_t>> incident;
};

std::istream& operator>>(std::istream& in, Graph& graph) {
    int blank;
    in >> graph.n >> graph.m >> blank;
    in.ignore();
    graph.incident.resize(graph.n);
    for (auto &&[u, u_incedent]: std::views::enumerate(graph.incident)) {
        std::string input_line;
        getline(in, input_line);
        std::stringstream ss(input_line);
        size_t v;
        while (ss >> v) {
            u_incedent.push_back(--v);
        }
    }
    return in;
}

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
