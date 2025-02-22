#include "Instance.hpp"
#include "Job.hpp"

#include <Solution.hpp>
#include <algorithm>
#include <climits>
#include <functional>
#include <list>
#include <vector>

using CandidateList  = std::list<const Job *>;
using SelectFunction = std::function<CandidateList::iterator(CandidateList&)>;
using IS             = Solution::InitialSolution;

// TODO(paulo-rozatto): codigo de lateness esta duplicado em varios lugares
Solution::Solution(std::vector<const Job *> jobs, const Instance& instance)
    : _jobSequence(jobs)
{
    _completionTime = 0;
    _completionTimes.clear();
    _maxLateness         = 0;
    _maxLatenessJobLabel = -1;

    int lastFamily       = 0;
    for (const Job *job : jobs) {
        int setup =
            (lastFamily > 0) ? instance.s(lastFamily, job->family()) : 0;

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
        maxLateness   = INT_MIN;
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

Solution Solution::generateInitialSolution(
    const Instance& instance,
    Solution::InitialSolution initalSoluationAlgorithm)
{
    Solution s;

    s._completionTime = 0;
    s._completionTimes.resize(instance.jobs().size());
    s._maxLateness         = INT_MIN;
    s._maxLatenessJobLabel = 0;
    s._jobSequence.reserve(instance.jobs().size());

    switch (initalSoluationAlgorithm) {
    case IS::EDD:
        s.sortEarliestDueDate(instance);
        break;
    case IS::ML:
        s.sortMaxLateness(instance);
        break;
    }
    return s;
}

void Solution::addJob(const Job& j, const Instance& instance)
{
    int delay;
    int lateness;

    delay = this->_jobSequence.empty()
                ? 0
                : instance.s(j.family(), this->_jobSequence.back()->family());
    this->_jobSequence.push_back(&j);
    this->_completionTime += delay + j.processingTime();
    this->_completionTimes[j.label() - 1] = this->_completionTime;
    lateness                              = this->_completionTime - j.dueDate();
    if (lateness > this->_maxLateness) {
        this->_maxLateness         = lateness;
        this->_maxLatenessJobLabel = j.label();
    }
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

Solution Solution::swap(unsigned a, unsigned b, const Instance& instance)
{
    Solution s;
    s._completionTime = 0;
    s._completionTimes.resize(this->_jobSequence.size());
    s._maxLateness         = INT_MIN;
    s._maxLatenessJobLabel = 0;
    s._jobSequence.reserve(this->_jobSequence.size());
    for (size_t i = 0; i < this->_jobSequence.size(); i++) {
        if (i == a) {
            s.addJob(*(this->_jobSequence[b]), instance);
        } else if (i == b) {
            s.addJob(*(this->_jobSequence[a]), instance);
        } else {
            s.addJob(*(this->_jobSequence[i]), instance);
        }
    }
    return s;
}
