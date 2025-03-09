#include "SimulatedAnnealing.hpp"

#include "Instance.hpp"
#include "Job.hpp"
#include "Solution.hpp"

#include <chrono>
#include <climits>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <math.h>
#include <random>
#include <vector>

#define MAX_RAND_ATTEMPTS 1000
#define MAX_SHAKE_ATTEMPTS 200

bool badSwap(int i, int j, int maxJobIndex, std::vector<const Job *>& reduced)
{
    if (reduced.at(i)->family() == reduced.at(j)->family() &&
        reduced.at(i)->dueDate() < reduced.at(j)->dueDate())
        return true;

    // theorem 3: m2
    if (i == maxJobIndex && j > i)
        return true;

    return false;
}

void groupJobs(const std::vector<const Job *>& init,
               std::vector<const Job *>& reduced)
{
    // Agrupando todos jobs sequencias segundo o Teorema 4 repetidamente
    // Teorema 4: Se jobs i e j sao sequenciais e o jobs estao ordenados em EDD
    // Pode se agrupar repetitivamente i e j se d_j < d_i + p_j;
    // https://doi.org/10.1016/S0167-6377(97)00028-X

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
}

void copy(const std::vector<const Job *>& src, std::vector<const Job *>& dst)
{
    dst.clear();
    for (const auto& job : src) {
        dst.push_back(job);
    }
}

Solution SimulatedAnnealing::run(Solution& initialSolution, unsigned timeLimit,
                                 const Instance& instance, bool enableLogging)
{
    std::vector<const Job *> init = initialSolution.jobSequence();
    std::vector<const Job *> reduced, best;
    reduced.reserve(init.size());

    auto start = std::chrono::steady_clock::now();

    groupJobs(init, reduced);
    best.reserve(reduced.size());
    copy(reduced, best);

    const int minTemperature = 1;
    std::pair<int, int> pair =
        Instance::calculateMaxLateness(reduced, instance);

    int latestJobIndex     = pair.first;
    int bestMaxLateness    = pair.second;
    int currentMaxLateness = pair.second;
    double temperature     = 50000;

    std::random_device rd;
    std::mt19937 gen(rd());

    bool timeout = false;

    int meanL = 0, count = 0;
    int bestSolutionIteration = 0;
    while (temperature > minTemperature && latestJobIndex > 0 && !timeout) {
        int l;
        for (l = 0; l < MAX_SHAKE_ATTEMPTS; l++) {

            std::uniform_int_distribution<size_t> dist(0, latestJobIndex);
            size_t i         = dist(gen);
            size_t j         = dist(gen);
            int randAttempts = 0;

            while (i == j || (randAttempts < MAX_RAND_ATTEMPTS &&
                              badSwap(i, j, latestJobIndex, reduced))) {
                j = dist(gen);
                ++randAttempts;
            }

            std::swap(reduced[i], reduced[j]);
            std::pair<int, int> pair =
                Instance::calculateMaxLateness(reduced, instance);

            // latestJobIndex     = pair.first;
            int newMaxLateness = pair.second;
            double delta       = newMaxLateness - currentMaxLateness;

            if (delta < 0) {
                currentMaxLateness = newMaxLateness;
            } else {
                double prob     = exp(-delta / temperature);
                double randPass = (static_cast<double>(rand()) / RAND_MAX);

                if (prob >= randPass) {
                    currentMaxLateness = newMaxLateness;
                } else {
                    std::swap(reduced[i], reduced[j]);
                }
            }

            if (currentMaxLateness < bestMaxLateness) {
                bestMaxLateness       = currentMaxLateness;
                bestSolutionIteration = count;
                copy(reduced, best);
                break;
            }
        }
        ++count;
        meanL += l;

        temperature *= 0.99;
        timeout = std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::steady_clock::now() - start)
                      .count() >= timeLimit;
    }
    meanL = count > 0 ? meanL / (float) count : 0;
    std::cout << "Mean l: " << meanL << std::endl
              << "Best solution it: " << bestSolutionIteration << std::endl
              << "Total its: " << count << std::endl;

    this->_executionTimeMS =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start)
            .count();

    if (timeout && enableLogging)
        std::cerr << "Timeout!" << "\n";

    Solution s(best, instance);

    return s;
}
