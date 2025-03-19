#include "LocalSearch.hpp"

#include "Solution.hpp"

#include <chrono>
#include <iostream>
#include <vector>

long long LocalSearch::executionTimeMS() const
{
    return this->_executionTimeMS;
};

static inline Solution cloneAndSwap(const Solution& s, unsigned idxA,
                                    unsigned idxB, const Instance& instance)
{
    Solution newS = Solution(s);
    newS.swap(idxA, idxB, instance);
    return newS;
}

Solution LocalSearch::bestNeighbor(Solution& s, const Instance& instance)
{
    unsigned sz    = s.jobSequence().size();
    Solution bestN = cloneAndSwap(s, 0, 1, instance);

    for (size_t i = 0; i < sz - 1; i++) {
        for (size_t j = i + 1; j < sz; j++) {
            Solution n = cloneAndSwap(s, i, j, instance);
            if (bestN.maxLateness() > n.maxLateness()) {
                bestN = std::move(n);
            }
        }
    }
    return bestN;
}

void LocalSearch::firstBetter(Solution& s, const Instance& instance)
{
    unsigned sz = s.jobSequence().size();
    bool done   = false;

    for (size_t i = 0; i < sz - 1 && !done; i++) {
        for (size_t j = i + 1; j < sz && !done; j++) {
            Solution n = cloneAndSwap(s, i, j, instance);
            if (s.maxLateness() > n.maxLateness()) {
                s    = std::move(n);
                done = true;
            }
        }
    }
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
