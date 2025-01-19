#include "Algorithm.hpp"
#include "Solution.hpp"
#include <iostream>
#include <vector>
#include <chrono>

long long Algorithm::executionTime() const {return this->_executionTime;};

Solution Algorithm::bestNeighbor(Solution& s, const Instance& instance) {
    unsigned sz = s.jobSequence().size();
    Solution best_n = s.swap(0, 1, instance);

    for (size_t i = 0; i < sz - 1; i++) {
        for (size_t j = i + 1; j < sz; j++) {
            Solution n = s.swap(i, j, instance);
            if (best_n.maxLateness() > n.maxLateness()) {
                best_n = std::move(n);
            }
        }
    }
    return best_n;
}

Solution Algorithm::run(Solution& initialSolution, unsigned timeLimit, const Instance& instance, bool enableLogging)
{
    bool improve = true;
    bool timeout = false;
    Solution best_s = initialSolution;
    Solution best_n;
    auto start = std::chrono::steady_clock::now();
    while(improve && !timeout) {
        if(enableLogging)
            std::cout << "Max Lateness: " << best_s.maxLateness() << "\n";
        improve = false;
        best_n = bestNeighbor(best_s, instance);
        if (best_s.maxLateness() > best_n.maxLateness()) {
            best_s = best_n;
            improve = true;
        }
        timeout = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() >= timeLimit;
        if(timeout && enableLogging)
            std::cout << "Timeout!" << "\n";
        
    }
    this->_executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
    if (enableLogging) {
        std::cout << "Execution Time: " << _executionTime << " milliseconds\n";
    }
    return best_s;
};