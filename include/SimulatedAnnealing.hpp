#ifndef SIMULATED_ANNEALING_HPP
#define SIMULATED_ANNEALING_HPP

#include "Solution.hpp"
// TODO (paulo-rozatto): polimorfismo de algoritmos
class SimulatedAnnealing {
private:
    long long _executionTimeMS;

public:
    Solution run(Solution& initialSolution, unsigned timeLimit,
                 const Instance& instance, bool enableLogging);

    long long executionTimeMS() const { return this->_executionTimeMS; }
};

#endif