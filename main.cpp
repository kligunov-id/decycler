#include "CLI11.hpp"

#include <iostream>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>

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
    std::cout << graph.n << " " << graph.m << "\n";
    for (auto &u_incident: graph.incident) {
        for (auto u: u_incident) {
            std::cout << u << " ";
        }
        std::cout << "\n";
    }
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
