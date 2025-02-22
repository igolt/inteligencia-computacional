#include "SimulatedAnnealing.hpp"

#include "Instance.hpp"
#include "Job.hpp"
#include "JobComposed.hpp"
#include "Solution.hpp"

#include <chrono>
#include <climits>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <math.h>
#include <memory>
#include <random>
#include <vector>

int calculateMaxLateness(const std::vector<const Job *>& jobs,
                         const Instance& instance)
{
    int currentTime = 0;
    int maxLateness = INT_MIN;
    int lastFamily  = -1;

    for (const auto& job : jobs) {
        int setup =
            (lastFamily > 0) ? instance.s(lastFamily, job->family()) : 0;
        currentTime += job->processingTime() + setup;
        int lateness = currentTime - job->dueDate();
        lastFamily   = job->family();

        if (lateness > maxLateness) {
            maxLateness = lateness;
        }
    }

    return maxLateness;
}

// TODO (paulo-rozatto): Testa vizinhacas promissoras ao inves de aleatorio
void randomSwap(std::vector<std::unique_ptr<Job>>& jobs)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, jobs.size() - 1);

    size_t i = dist(gen);
    size_t j = dist(gen);
    while (i == j) {
        j = dist(gen);
    }

    std::swap(jobs[i], jobs[j]);
}

Solution SimulatedAnnealing::run(Solution& initialSolution, unsigned timeLimit,
                                 const Instance& instance, bool enableLogging)
{
    std::vector<const Job *> init = initialSolution.jobSequence();
    std::vector<const Job *> reduced;
    reduced.reserve(init.size());

    // Agrupando todos jobs sequencias segundo o Teorema 4 repetidamente
    // Teorema 4: Se jobs i e j sao sequenciais e o jobs estao ordenados em EDD
    // Pode se agrupar repetitivamente i e j se d_j < d_i + p_j;
    // https://doi.org/10.1016/S0167-6377(97)00028-X
    auto start = std::chrono::steady_clock::now();

    for (size_t i = 0; i < init.size(); i++) {
        Job *job = new Job(init[i]->label(), init[i]->family(),
                           init[i]->dueDate(), init[i]->processingTime());

        for (size_t j = i + 1; j < init.size(); j++) {
            const Job *nxt = init[j];

            if (job->family() == nxt->family() &&
                nxt->dueDate() < job->dueDate() + nxt->processingTime()) {
                job->setDueDate(nxt->dueDate());
                job->setProcessingTime(job->processingTime() +
                                       nxt->processingTime());
                i = j;
            } else {
                break;
            }
        }

        reduced.push_back(job);
    }

    const int minTemperature = 1;
    int currentMaxLatness    = calculateMaxLateness(reduced, instance);
    int temperature          = 100;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, reduced.size() - 1);

    bool timeout = false;

    while (temperature > minTemperature) {

        size_t i = dist(gen);
        size_t j = dist(gen);
        while (i == j) {
            j = dist(gen);
        }

        std::swap(reduced[i], reduced[j]);
        int newMaxLateness = calculateMaxLateness(reduced, instance);
        int delta          = newMaxLateness - currentMaxLatness;

        if (delta < 0 || exp(-static_cast<double>(delta) / temperature) >
                             (static_cast<double>(rand()) / RAND_MAX)) {
            currentMaxLatness = newMaxLateness;
        } else {
            std::swap(reduced[i], reduced[j]);
        }

        temperature *= 0.99;
        timeout = std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::steady_clock::now() - start)
                      .count() >= timeLimit;
    }

    this->_executionTimeMS =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start)
            .count();

    if (timeout && enableLogging)
        std::cerr << "Timeout!" << "\n";

    Solution s(reduced, instance);

    return s;
}