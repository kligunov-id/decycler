#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using Node = size_t;

struct Graph {
    Node n, m;

    std::vector<std::vector<Node>> out_links;
    std::vector<std::vector<Node>> in_links;
    std::vector<size_t> total_deg;
};

std::istream& operator>>(std::istream& in, Graph& graph) {
    int blank;
    in >> graph.n >> graph.m >> blank;
    in.ignore();
    graph.out_links.resize(graph.n);
    graph.in_links.resize(graph.n);
    graph.total_deg.resize(graph.n);
    for (Node u = 0; u < graph.n; u++) {
        std::string input_line;
        getline(in, input_line);
        std::stringstream ss(input_line);
        Node v;
        while (ss >> v) {
            graph.out_links[u].push_back(--v);
            graph.in_links[v].push_back(u);
            graph.total_deg[u]++;
            graph.total_deg[v]++;
        }
    }
    return in;
}

std::ostream& operator<<(std::ostream& out, const Graph& graph) { // <- check that extra spaces do not break python reader
    out << graph.n << " " << graph.m << " 0\n";
    for (Node u = 0; u < graph.n; u++) {
        for (Node v: graph.out_links[u]) {
            out << v + 1 << " ";
        }
        out << "\n";
    }
    return out;
}
