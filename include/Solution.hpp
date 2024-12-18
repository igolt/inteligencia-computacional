#ifndef SOLUTION_HPP
#define SOLUTION_HPP

#include "Instance.hpp"

#include <vector>

// REFACTOR(igolt): isso aqui não tá muito bem representado
class Solution {
private:
    int _completionTime;
    std::vector<int> _completionTimes;
    int _maxLateness;
    int _maxLatenessJobLabel;
    std::vector<const Job *> _jobSequence;

    void addJob(const Job& j, const Instance& instance);

public:
    static Solution generateInitialSolution(const Instance& instance);

    int completionTime() const;
    const std::vector<int>& completionTimes() const;
    int maxLateness() const;
    int maxLatenessJobLabel() const;
    const std::vector<const Job *>& jobSequence() const;
};

#endif /* SOLUTION_HPP */
