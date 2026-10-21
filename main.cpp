#include "CLI11.hpp"

#include <iostream>

int main(int argc, char **argv) {
    CLI::App app;
    CLI11_PARSE(app, argc, argv);
    int n;
    std::cin >> n;
    std::cout << n << "\n";
    return 0;
}
