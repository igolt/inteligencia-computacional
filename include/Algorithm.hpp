#ifndef ALGORITHM_HPP
#define ALGORITHM_HPP

#include "Solution.hpp"
#include <iostream>
#include <vector>

class Algorithm {
private:
    long long _executionTime;
    Solution bestNeighbor(Solution& s, const Instance& instance);
public:
    Solution run(Solution& initialSolution, unsigned timeLimit, const Instance& instance, bool enableLogging);
    long long executionTime() const;
};

#endif /* ALGORITHM_HPP */