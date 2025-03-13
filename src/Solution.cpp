#include "Instance.hpp"
#include "Job.hpp"

#include <Solution.hpp>
#include <algorithm>
#include <climits>
#include <functional>
#include <iostream>
#include <list>
#include <random>
#include <unordered_map>
#include <vector>

using CandidateList  = std::list<const Job *>;
using SelectFunction = std::function<CandidateList::iterator(CandidateList&)>;
using IS             = Solution::InitialSolution;

#define LATENESS_MIN (INT_MIN)

Solution::Solution(std::vector<const Job *> jobs, const Instance& instance)
    : _jobSequence(jobs)
{
    this->calculateLateness(instance);
}

void Solution::calculateLateness(const Instance& instance)
{
    _completionTime = 0;
    _completionTimes.clear();
    _maxLateness         = LATENESS_MIN;
    _maxLatenessJobLabel = -1;

    int lastFamily       = 0;
    for (const Job *job : _jobSequence) {
        int setup = instance.s(lastFamily, job->family());

        _completionTime += job->processingTime() + setup;
        _completionTimes.push_back(_completionTime);
        lastFamily   = job->family();

        int lateness = _completionTime - job->dueDate();
        if (lateness > _maxLateness) {
            _maxLateness         = lateness;
            _maxLatenessJobLabel = job->label();
        }
    }
}

void Solution::sortEarliestDueDate(const Instance& instance)
{
    for (const auto& job : instance.jobs()) {
        _jobSequence.push_back(&job);
    }
    std::stable_sort(
        _jobSequence.begin(), _jobSequence.end(),
        [](const Job *a, const Job *b) { return a->dueDate() < b->dueDate(); });

    this->calculateLateness(instance);
}

void Solution::sortMaxLateness(const Instance& instance)
{
    int accTime    = 0;
    int lastFamily = -1;
    int lateness, setup, maxLateness, latestSetup;

    CandidateList candidates;
    for (const auto& job : instance.jobs()) {
        candidates.push_back(&job);
    }

    while (!candidates.empty()) {
        auto latestIt = candidates.begin();
        maxLateness   = LATENESS_MIN;
        latestSetup   = 0;

        for (auto it = candidates.begin(); it != candidates.end(); ++it) {
            const Job *job = *it;
            setup = lastFamily < 0 ? 0 : instance.s(lastFamily, job->family());
            lateness = accTime + setup + job->processingTime() - job->dueDate();

            if (lateness > maxLateness) {
                latestIt    = it;
                maxLateness = lateness;
                latestSetup = setup;
            }
        }

        const Job *latestJob = *latestIt;

        accTime += latestJob->processingTime() + latestSetup;
        candidates.erase(latestIt);
        _jobSequence.push_back(latestJob);

        lastFamily                               = latestJob->family();
        _completionTimes[latestJob->label() - 1] = accTime;

        if (maxLateness > _maxLateness) {
            _maxLateness         = maxLateness;
            _maxLatenessJobLabel = latestJob->label();
        }
    }

    _completionTime = accTime;
}

void Solution::sortRandomOrder(const Instance& instance)
{
    for (const auto& job : instance.jobs()) {
        _jobSequence.push_back(&job);
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(_jobSequence.begin(), _jobSequence.end(), g);

    int accTime    = 0;
    int lastFamily = -1;
    _maxLateness   = INT_MIN;

    for (const auto& job : _jobSequence) {
        int setup =
            (lastFamily > 0) ? instance.s(lastFamily, job->family()) : 0;
        lastFamily = job->family();
        accTime += setup + job->processingTime();
        _completionTimes[job->label() - 1] = accTime;

        int lateness                       = accTime - job->dueDate();
        if (lateness > _maxLateness) {
            _maxLateness         = lateness;
            _maxLatenessJobLabel = job->label();
        }
    }

    _completionTime = accTime;
}

Solution Solution::generateInitialSolution(
    const Instance& instance,
    Solution::InitialSolution initalSoluationAlgorithm)
{
    Solution s;

    s._completionTime = 0;
    s._completionTimes.resize(instance.jobs().size());
    s._maxLateness         = LATENESS_MIN;
    s._maxLatenessJobLabel = 0;
    s._jobSequence.reserve(instance.jobs().size());

    switch (initalSoluationAlgorithm) {
    case IS::EDD:
        s.sortEarliestDueDate(instance);
        break;
    case IS::ML:
        s.sortMaxLateness(instance);
        break;
    case IS::RAND:
        s.sortRandomOrder(instance);
        break;
    }
    return s;
}

/**
 * Faz o cálculo do completion time considerando `job`como se estivesse sendo
 * inserido como o último job da sequência, sendo precedido por `prevJob`
 */
void Solution::calculateCompletionTime(const Job *job, const Job *prevJob,
                                       const Instance& instance)
{
    int delay;

    delay = !prevJob ? 0 : instance.s(prevJob->family(), job->family());
    this->_completionTime += delay + job->processingTime();
    this->_completionTimes[job->label() - 1] = this->_completionTime;
}

/**
 * Verifica a lateness de um job já inserido na sequência e atualiza a max
 * lateness caso necessário
 */
void Solution::verifyMaxLateness(const Job& job)
{
    int lateness = this->_completionTimes[job.label() - 1] - job.dueDate();
    if (lateness > this->_maxLateness) {
        this->_maxLateness         = lateness;
        this->_maxLatenessJobLabel = job.label();
    }
}

void Solution::addJob(const Job& j, const Instance& instance)
{
    this->_jobSequence.push_back(&j);
    this->calculateCompletionTime(
        &j, this->_jobSequence.empty() ? nullptr : this->_jobSequence.back(),
        instance);
    this->verifyMaxLateness(j);
}

int Solution::completionTime() const { return this->_completionTime; }

const std::vector<int>& Solution::completionTimes() const
{
    return this->_completionTimes;
}

int Solution::maxLateness() const { return this->_maxLateness; }

int Solution::maxLatenessJobLabel() const { return this->_maxLatenessJobLabel; }

const std::vector<const Job *>& Solution::jobSequence() const
{
    return this->_jobSequence;
}

void Solution::swap(unsigned idxA, unsigned idxB, const Instance& instance)
{
    const Job *job, *prevJob;
    unsigned minIdx;

    std::swap(this->_jobSequence[idxA], this->_jobSequence[idxB]);

    minIdx = std::min(idxA, idxB);
    job    = this->_jobSequence[minIdx];
    if (minIdx == 0) {
        prevJob               = nullptr;
        this->_completionTime = 0;
    } else {
        prevJob               = this->_jobSequence[minIdx - 1];
        this->_completionTime = this->_completionTimes[prevJob->label() - 1];
    }

    unsigned i = minIdx;
    while (true) {
        this->calculateCompletionTime(job, prevJob, instance);
        if (++i >= this->_jobSequence.size()) {
            break;
        }
        prevJob = job;
        job     = this->_jobSequence[i];
    }

    this->_maxLateness = LATENESS_MIN;
    for (i = 0; i < this->_jobSequence.size(); i++) {
        this->verifyMaxLateness(*this->_jobSequence[i]);
    }
}

Solution Solution::pmxCrossover(const Solution& parent2, unsigned cut1,
                                unsigned cut2, const Instance& instance)
{
    unsigned j = 0;

    // Criando Solution
    Solution child;
    child._completionTime = 0;
    child._completionTimes.resize(this->_jobSequence.size());
    child._maxLateness         = INT_MIN;
    child._maxLatenessJobLabel = 0;
    child._jobSequence.reserve(this->_jobSequence.size());

    // Criando vetor auxiliar
    std::vector<const Job *> jobSequence;
    jobSequence.resize(this->_jobSequence.size());

    std::unordered_map<unsigned, unsigned> mapping;

    for (unsigned i = cut1; i <= cut2; ++i) {
        jobSequence[i] = this->_jobSequence[i];
        mapping[this->_jobSequence[i]->label()] = i;
    }

    for (unsigned i = 0; i < jobSequence.size(); i++) {
        if (!(i >= cut1 && i <= cut2)){
            j = i;
            while(mapping.find(parent2.jobSequence()[j]->label()) != mapping.end()){
                j = mapping[parent2.jobSequence()[j]->label()];
            }
            jobSequence[i] = parent2.jobSequence()[j];
        }
    }

    for (const auto& job : jobSequence) {
        child.addJob(*job, instance);
    }

    return child;
}

void Solution::printJobSequence() const
{
    for (const auto& job : _jobSequence) {
        std::cout << job->label() << " ";
    }
    std::cout << std::endl;
}
