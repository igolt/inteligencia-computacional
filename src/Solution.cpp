#include <Solution.hpp>
#include <climits>
#include <list>

using CandidateList = std::list<const Job *>;

inline CandidateList candidateListFromInstance(const Instance& instance)
{
    CandidateList cl;
    cl.resize(instance.jobs().size());
    auto jobIt = instance.jobs().begin();
    for (auto candidateIt = cl.begin(); candidateIt != cl.end();
         ++candidateIt, ++jobIt) {
        *candidateIt = &*jobIt;
    }
    return cl;
}

CandidateList::iterator selectCandidateMaxLateness(CandidateList& cl)
{
    auto jobIt              = cl.begin();
    auto jobWithMaxLateness = jobIt;
    int maxLateness         = (*jobWithMaxLateness)->processingTime() -
                      (*jobWithMaxLateness)->dueDate();
    for (++jobIt; jobIt != cl.end(); ++jobIt) {
        int jobLateness = (*jobIt)->processingTime() - (*jobIt)->dueDate();
        if (jobLateness > maxLateness) {
            maxLateness        = jobLateness;
            jobWithMaxLateness = jobIt;
        }
    }
    return jobWithMaxLateness;
}

CandidateList::iterator selectCandidateMinLateness(CandidateList& cl)
{
    auto jobIt              = cl.begin();
    auto jobWithMinLateness = jobIt;
    int minLateness         = (*jobWithMinLateness)->processingTime() -
                      (*jobWithMinLateness)->dueDate();
    for (++jobIt; jobIt != cl.end(); ++jobIt) {
        int jobLateness = (*jobIt)->processingTime() - (*jobIt)->dueDate();
        if (jobLateness < minLateness) {
            minLateness        = jobLateness;
            jobWithMinLateness = jobIt;
        }
    }
    return jobWithMinLateness;
}

CandidateList::iterator selectCandidateEDD(CandidateList& cl)
{
    auto jobIt      = cl.begin();
    auto jobWithEDD = jobIt;
    int edd         = (*jobWithEDD)->dueDate();
    for (++jobIt; jobIt != cl.end(); ++jobIt) {
        if ((*jobIt)->dueDate() < edd) {
            edd        = (*jobIt)->dueDate();
            jobWithEDD = jobIt;
        }
    }
    return jobWithEDD;
}

Solution Solution::generateInitialSolution(const Instance& instance)
{
    // REFACTOR(igolt): isso aqui com certeza tem como melhorar em todos os
    // aspectos
    Solution s;

    s._completionTime = 0;
    s._completionTimes.resize(instance.jobs().size());
    s._maxLateness         = INT_MIN;
    s._maxLatenessJobLabel = 0;
    s._jobSequence.reserve(instance.jobs().size());

    // PERF(igolt): lista de candidatos pode ser uma estrutura que permita achar
    // o maior de forma mais eficiente
    auto candidateList = candidateListFromInstance(instance);

    while (!candidateList.empty()) {
        auto candidate = selectCandidateMaxLateness(candidateList);
        s.addJob(**candidate, instance);
        candidateList.erase(candidate);
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
