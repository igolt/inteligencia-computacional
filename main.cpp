#include "Algorithm.hpp"
#include "Instance.hpp"
#include "Solution.hpp"

#include <cctype>
#include <cstdio>
#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

Solution::InitialSolution getInitalSolutionAlgo(const char *algoName);

static Solution avaliateSolutionForInstanceFile(const char *algoName,
                                                const char *fileName);

static void benchmark(const char *algoName, const char **fileList,
                      int listSize);

int main(int argc, const char *argv[])
{
    const char *progName = argv[0];

    if (argc < 2) {
        std::cerr << "usage: " << progName
                  << " INITIAL_SOLUTION_ALGO FILE[S]\n";
        return 1;
    }

    try {
        if (argc == 2) {
            avaliateSolutionForInstanceFile(argv[1], argv[2]);
        } else {
            benchmark(argv[1], &argv[2], argc - 2);
        }
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

static Solution avaliateSolutionForInstanceFile(const char *algoName,
                                                const char *fileName)
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
    return algorithm.run(solution, 1000, instance, true);
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

    Algorithm algorithm;
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
        csv << "];" << algorithm.executionTime() << "\n";
    }

    csv.close();
}
