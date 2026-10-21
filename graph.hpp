#include <iostream>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>

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
