#ifndef ALGORITHM_HPP
#define ALGORITHM_HPP

#include "Solution.hpp"

class LocalSearch {
private:
    long long _executionTimeMS;
    Solution bestNeighbor(Solution& s, const Instance& instance);

public:
    Solution run(Solution& initialSolution, unsigned timeLimit,
                 const Instance& instance, bool enableLogging);
    long long executionTimeMS() const;
};

#endif /* ALGORITHM_HPP */
