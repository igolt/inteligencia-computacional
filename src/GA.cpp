#include "GA.hpp"

#include "LocalSearch.hpp"
#include "Solution.hpp"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>

Solution::InitialSolution initialSolutionAlgo[] = {
    Solution::InitialSolution::EDD,  Solution::InitialSolution::EDD,
    Solution::InitialSolution::ML,   Solution::InitialSolution::ML,
    Solution::InitialSolution::RAND,
};

void GA::initialPopulation(int popSize, std::vector<Solution>& population,
                           const Instance& instance)
{
    population.reserve(popSize);

    population.push_back(Solution::generateInitialSolution(
        instance, Solution::InitialSolution::EDD));
    population.push_back(Solution::generateInitialSolution(
        instance, Solution::InitialSolution::ML));

    for (int i = 2; i < popSize; ++i) {
        population.push_back(Solution::generateInitialSolution(
            instance, initialSolutionAlgo[i % 5]));
    }
}

void GA::survival(int popSize, std::vector<Solution>& population,
                  std::vector<Solution>& nextPopulation)
{
    std::sort(population.begin(), population.end(),
              [](const Solution& a, const Solution& b) {
                  return a.maxLateness() < b.maxLateness();
              });

    int top25  = popSize * 0.25;
    int worst5 = popSize * 0.05;

    nextPopulation.insert(nextPopulation.end(), population.begin(),
                          population.begin() + top25);

    nextPopulation.insert(nextPopulation.end(), population.end() - worst5,
                          population.end());
}

int GA::rouletteSelection(const std::vector<Solution>& population, int start,
                          int end)
{
    std::vector<double> fitness;
    double totalFitness = 0.0;

    for (int i = start; i < end; i++) {
        double fit = 1.0 / (1.0 + population[i].maxLateness());
        fitness.push_back(fit);
        totalFitness += fit;
    }

    double r   = ((double) rand() / RAND_MAX) * totalFitness;
    double sum = 0.0;
    for (int i = start; i < end; i++) {
        sum += fitness[i - start];
        if (sum >= r) {
            return i;
        }
    }
    return start;
}

void GA::selection(std::vector<Solution>& population, unsigned& parentA,
                   unsigned& parentBC, unsigned top25, unsigned popSize)
{
    parentA  = rouletteSelection(population, 0, top25);
    parentBC = rouletteSelection(population, top25, popSize);
}

void GA::defineCuts(unsigned& cut1, unsigned& cut2, unsigned numJobs)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, numJobs - 1);
    cut1 = dist(gen);
    cut2 = dist(gen);
    if (cut1 > cut2)
        std::swap(cut1, cut2);
}

Solution GA::crossover(std::vector<Solution>& population, unsigned parentA,
                       unsigned parentBC, unsigned numJobs,
                       const Instance& instance)
{
    unsigned cut1 = 0;
    unsigned cut2 = 0;
    defineCuts(cut1, cut2, numJobs);
    return population[parentA].pmxCrossover(population[parentBC], cut1, cut2,
                                            instance);
}

void GA::mutation(Solution& child, double mutationRate, unsigned numJobs,
                  const Instance& instance)
{
    static std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<double> prob(0.0, 1.0);
    std::uniform_int_distribution<unsigned> dist(0, numJobs - 1);

    if (prob(rng) < mutationRate) {
        unsigned i = dist(rng);
        unsigned j = dist(rng);
        while (i == j) {
            j = dist(rng);
        }
        child.swap(i, j, instance);
    }
}

Solution GA::bestSolution(const std::vector<Solution>& population)
{
    return *std::min_element(population.begin(), population.end(),
                             [](const Solution& a, const Solution& b) {
                                 return a.maxLateness() < b.maxLateness();
                             });
}

Solution GA::run(unsigned popSize, int epochs, double mutationRate,
                 bool localSearch, const Instance& instance)
{
    std::vector<Solution> population;
    std::vector<Solution> nextPopulation;
    unsigned numJobs  = instance.jobs().size();
    unsigned top25    = popSize / 4;
    unsigned parentA  = 0;
    unsigned parentBC = 0;

    _executionTimes.clear(); // Limpa os dados antes de executar
    _maxLatenesses.clear();

    auto algoStart = std::chrono::high_resolution_clock::now();
    initialPopulation(popSize, population, instance);

    for (int epoch = 0; epoch < epochs; epoch++) {
        auto start = std::chrono::high_resolution_clock::now();

        nextPopulation.clear();
        survival(popSize, population, nextPopulation);
        while (nextPopulation.size() < popSize) {
            selection(population, parentA, parentBC, top25, popSize);
            Solution child =
                crossover(population, parentA, parentBC, numJobs, instance);
            mutation(child, mutationRate, numJobs, instance);
            if (localSearch) {
                LocalSearch::firstBetter(child, instance);
            }
            nextPopulation.push_back(child);
        }
        population = nextPopulation;

        auto end   = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;

        int maxLateness = bestSolution(population).maxLateness();
        std::cout << maxLateness << "\n";

        _executionTimes.push_back(
            elapsed.count());                  // Armazena o tempo de execução
        _maxLatenesses.push_back(maxLateness); // Armazena o maxLateness
    }
    auto algoEnd   = std::chrono::high_resolution_clock::now();
    _executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                         algoEnd - algoStart)
                         .count();

    return bestSolution(population);
}

std::vector<double> GA::executionTimes() { return _executionTimes; }

std::vector<int> GA::maxLatenesses() { return _maxLatenesses; }
