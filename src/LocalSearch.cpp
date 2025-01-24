#include "LocalSearch.hpp"

#include "Solution.hpp"

#include <chrono>
#include <iostream>
#include <vector>

long long LocalSearch::executionTimeMS() const
{
    return this->_executionTimeMS;
};

Solution LocalSearch::bestNeighbor(Solution& s, const Instance& instance)
{
    unsigned sz    = s.jobSequence().size();
    Solution bestN = s.swap(0, 1, instance);

    for (size_t i = 0; i < sz - 1; i++) {
        for (size_t j = i + 1; j < sz; j++) {
            Solution n = s.swap(i, j, instance);
            if (bestN.maxLateness() > n.maxLateness()) {
                bestN = std::move(n);
            }
        }
    }
    return bestN;
}

Solution LocalSearch::run(Solution& initialSolution, unsigned timeLimit,
                          const Instance& instance, bool enableLogging)
{
    bool improve   = true;
    bool timeout   = false;
    Solution bestS = initialSolution;
    Solution bestN;
    auto start = std::chrono::steady_clock::now();
    while (improve && !timeout) {
        if (enableLogging)
            std::cerr << "Max Lateness: " << bestS.maxLateness() << "\n";
        improve = false;
        bestN   = bestNeighbor(bestS, instance);
        if (bestS.maxLateness() > bestN.maxLateness()) {
            bestS   = bestN;
            improve = true;
        }
        timeout = std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::steady_clock::now() - start)
                      .count() >= timeLimit;
        if (timeout && enableLogging)
            std::cerr << "Timeout!" << "\n";
    }
    this->_executionTimeMS =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start)
            .count();
    return bestS;
};
