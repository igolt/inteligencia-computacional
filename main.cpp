#include "Instance.hpp"
#include "Solution.hpp"
#include "Algorithm.hpp"

#include <cctype>
#include <cstdio>
#include <cstring>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

Solution::InitialSolution getInitalSolutionAlgo(const char *algoName);

static void avaliateSolutionForInstanceFile(const char *fileName,
                                            const char *algoName);

int main(int argc, const char *argv[])
{
    const char *progName = argv[0];

    if (argc < 2 || argc > 3) {
        std::cerr << "usage: " << progName << " FILE [INITIAL_SOLUTION_ALGO]\n";
        return 1;
    }

    try {
        avaliateSolutionForInstanceFile(argv[1], argc == 3 ? argv[2] : "ML");
    } catch (std::exception& e) {
        std::cerr << progName << ": " << e.what() << '\n';
        return 2;
    }
    return 0;
}

Solution::InitialSolution getInitalSolutionAlgo(const char *algoName)
{
    if (!strcmp(algoName, "EDD")) {
        return Solution::InitialSolution::EDD;
    } else if (!strcmp(algoName, "ML")) {
        return Solution::InitialSolution::ML;
    }
    throw std::invalid_argument(
        std::string("invalid initial solution algorithm name '") + algoName +
        "'");
}

static void avaliateSolutionForInstanceFile(const char *fileName,
                                            const char *algoName)
{
    Instance instance              = Instance::fromFile(fileName);
    Solution::InitialSolution algo = getInitalSolutionAlgo(algoName);
    Solution solution = Solution::generateInitialSolution(instance, algo);

    /*for (const Job *j : solution.jobSequence()) {
        std::cout << j->label() << '\n';
    }
    std::cout << "max lateness: " << solution.maxLateness() << "\n";
    std::cout << "max lateness job: " << solution.maxLatenessJobLabel() << "\n";
    std::cout << "completion time: "
              << solution.completionTimes()[solution.maxLatenessJobLabel() - 1]
              << "\n";*/

    Algorithm algorithm;
    Solution solution2 = algorithm.run(solution, 1000, instance, true);
}
