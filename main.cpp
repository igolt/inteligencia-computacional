#include "Instance.hpp"
#include "LocalSearch.hpp"
#include "Solution.hpp"

#include <cctype>
#include <cstdio>
#include <cstring>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

#define DEFAULT_TIMEOUT 1000u

inline unsigned timeoutMSFromArgs(int argc, const char *argv[]);

Solution::InitialSolution getInitalSolutionAlgo(const char *algoName);

static void avaliateSolutionForInstanceFile(const char *fileName,
                                            const char *algoName,
                                            unsigned timoeutMS);

int main(int argc, const char *argv[])
{
    const char *progName = argv[0];

    if (argc < 2 || argc > 4) {
        std::cerr << "usage: " << progName
                  << " FILE [INITIAL_SOLUTION_ALGO] [TIMEOUT_MS]\n";
        return 1;
    }

    const char *initalSolutionAlgorithm = argc >= 3 ? argv[2] : "ML";
    unsigned int timeoutMS              = timeoutMSFromArgs(argc, argv);

    try {
        avaliateSolutionForInstanceFile(argv[1], initalSolutionAlgorithm,
                                        timeoutMS);
    } catch (std::exception& e) {
        std::cerr << progName << ": " << e.what() << '\n';
        return 2;
    }
    return 0;
}

inline unsigned timeoutMSFromArgs(int argc, const char *argv[])
{
    if (argc >= 4) {
        try {
            return std::stoul(argv[3]);
        } catch (std::exception& _) {
            std::cerr << "invalid value for timeout: " << argv[3] << '\n';
            std::cerr << "defaulting to " << DEFAULT_TIMEOUT << '\n';
        }
    }
    return DEFAULT_TIMEOUT;
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
                                            const char *algoName,
                                            unsigned timeoutMS)
{
    Instance instance              = Instance::fromFile(fileName);
    Solution::InitialSolution algo = getInitalSolutionAlgo(algoName);
    Solution solution = Solution::generateInitialSolution(instance, algo);

    LocalSearch localSearch;
    localSearch.run(solution, timeoutMS, instance, true);

    std::cout << "Execution Time: " << localSearch.executionTimeMS()
              << " milliseconds\n";
}
