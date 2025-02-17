#include "Instance.hpp"
#include "LocalSearch.hpp"
#include "Solution.hpp"

#include <cctype>
#include <cstdio>
#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#define DEFAULT_TIMEOUT 1000u

inline unsigned timeoutMSFromArgs(int argc, const char *argv[]);

Solution::InitialSolution getInitalSolutionAlgo(const char *algoName);

static Solution avaliateSolutionForInstanceFile(const char *fileName,
                                                const char *algoName,
                                                unsigned timoeutMS);

static void benchmark(const char *algoName, const char **fileList,
                      int listSize);

int main(int argc, const char *argv[])
{
    const char *progName = argv[0];

    if (argc < 4) {
        std::cerr << "usage: " << progName
                  << " INITIAL_SOLUTION_ALGO TIMEOUT_MS FILE[S]\n";
        return 1;
    }

    const char *initalSolutionAlgorithm = argv[1];
    unsigned int timeoutMS              = timeoutMSFromArgs(argc, argv);

    try {
        if (argc == 4) {
            avaliateSolutionForInstanceFile(argv[3], initalSolutionAlgorithm,
                                            timeoutMS);
        } else {
            benchmark(argv[1], &argv[3], argc - 3);
        }
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
            return std::stoul(argv[2]);
        } catch (std::exception& _) {
            std::cerr << "invalid value for timeout: " << argv[2] << '\n';
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

static Solution avaliateSolutionForInstanceFile(const char *fileName,
                                                const char *algoName,
                                                unsigned timeoutMS)
{
    Instance instance              = Instance::fromFile(fileName);
    Solution::InitialSolution algo = getInitalSolutionAlgo(algoName);
    Solution solution = Solution::generateInitialSolution(instance, algo);

    LocalSearch localSearch;
    Solution s = localSearch.run(solution, timeoutMS, instance, true);
    std::cout << "Execution Time: " << localSearch.executionTimeMS()
              << " milliseconds\n";
    return s;
}

static void benchmark(const char *algoName, const char **fileList, int listSize)
{
    std::ofstream csv;
    csv.open("output.csv");

    if (!csv.is_open()) {
        std::cerr << "Failed to open file 'output.csv'." << std::endl;
        return;
    }

    csv << "File;Number of jobs;Numer of families;Setup Class;Setup "
           "Times;Distance "
           "Index;Initial Lateness;Final Lateness;Jobs order;Jobs "
           "families;Jobs Processing Time;Execution Time\n";

    LocalSearch algorithm;
    for (int i = 0; i < listSize; i++) {
        Instance instance              = Instance::fromFile(fileList[i]);
        Solution::InitialSolution algo = getInitalSolutionAlgo(algoName);
        Solution initialSolution =
            Solution::generateInitialSolution(instance, algo);

        std::cout << i + 1 << "/" << listSize << ": " << fileList[i]
                  << std::endl;

        csv << fileList[i] << ";";
        instance.toCsv(csv);

        Solution finalSolution =
            algorithm.run(initialSolution, 1000, instance, false);

        csv << initialSolution.maxLateness() << ";";
        csv << finalSolution.maxLateness() << ";";

        csv << "[";
        for (const Job *j : finalSolution.jobSequence()) {
            csv << j->label() << ',';
        }
        csv << "];";

        csv << "[";
        for (const Job *j : finalSolution.jobSequence()) {
            csv << j->family() << ',';
        }
        csv << "];";

        csv << "[";
        for (const Job *j : finalSolution.jobSequence()) {
            csv << j->processingTime() << ',';
        }
        csv << "];" << algorithm.executionTimeMS() << "\n";
    }

    csv.close();
}
