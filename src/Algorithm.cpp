#include "Algorithm.hpp"

#include "Solution.hpp"

#include <chrono>
#include <iostream>
#include <vector>

long long Algorithm::executionTime() const { return this->_executionTime; };

Solution Algorithm::bestNeighbor(Solution& s, const Instance& instance)
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

Solution Algorithm::run(Solution& initialSolution, unsigned timeLimit,
                        const Instance& instance, bool enableLogging)
{
    bool improve   = true;
    bool timeout   = false;
    Solution bestS = initialSolution;
    Solution bestN;
    auto start = std::chrono::steady_clock::now();
    while (improve && !timeout) {
        if (enableLogging)
            std::cout << "Max Lateness: " << bestS.maxLateness() << "\n";
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
            std::cout << "Timeout!" << "\n";
    }
    this->_executionTime =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start)
            .count();
    if (enableLogging) {
        std::cout << "Execution Time: " << _executionTime << " milliseconds\n";
    }
    return bestS;
};
