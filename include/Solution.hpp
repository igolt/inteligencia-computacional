#ifndef SOLUTION_HPP
#define SOLUTION_HPP

#include "Instance.hpp"

#include <vector>

// REFACTOR(igolt): isso aqui não tá muito bem representado
class Solution {
public:
    // Initial Solution Algorithms Enum
    enum class InitialSolution {
        EDD, // Earliest Due Date
        ML,
        RAND,  // Maximum Lateness
    };

private:
    int _completionTime;
    std::vector<int> _completionTimes;
    int _maxLateness;
    int _maxLatenessJobLabel;
    std::vector<const Job *> _jobSequence;

    void addJob(const Job& j, const Instance& instance);
    void sortEarliestDueDate(const Instance& instance);
    void sortMaxLateness(const Instance& instance);
    void sortRandomOrder(const Instance& instance);

public:
    static Solution generateInitialSolution(const Instance& instance,
                                            InitialSolution algo);
    Solution swap(unsigned a, unsigned b, const Instance& instance);
    int completionTime() const;
    const std::vector<int>& completionTimes() const;
    int maxLateness() const;
    int maxLatenessJobLabel() const;
    const std::vector<const Job *>& jobSequence() const;
    void printJobSequence() const;
    Solution pmxCrossover(const Solution& parent2, unsigned cut1, unsigned cut2, const Instance& instance);
};

#endif /* SOLUTION_HPP */
