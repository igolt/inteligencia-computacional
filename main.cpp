#include "Instance.hpp"
#include "Solution.hpp"

#include <cctype>
#include <cstdio>
#include <iostream>
#include <string>

int main(int argc, const char *argv[])
{
    const char *progName = argv[0];

    if (argc != 2) {
        std::cerr << "usage: " << progName << " FILE\n";
        return 1;
    }
    Instance instance = Instance::fromFile(argv[1]);
    Solution solution = Solution::generateInitialSolution(instance);

    std::cout << instance << '\n';
    for (const Job *j : solution.jobSequence()) {
        std::cout << j->label() << '\n';
    }
    std::cout << "max lateness: " << solution.maxLateness() << "\n";
    std::cout << "max lateness job: " << solution.maxLatenessJobLabel() << "\n";
    std::cout << "completion time: "
              << solution.completionTimes()[solution.maxLatenessJobLabel() - 1]
              << "\n";

    return 0;
}
