#include <Solution.hpp>
#include <climits>
#include <functional>
#include <list>
#include <stdexcept>
#include <iostream>

using CandidateList  = std::list<const Job *>;
using SelectFunction = std::function<CandidateList::iterator(CandidateList&)>;
using IS             = Solution::InitialSolution;

static inline CandidateList candidateListFromInstance(const Instance& instance)
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

static CandidateList::iterator selectCandidateMaxLateness(CandidateList& cl)
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

static CandidateList::iterator selectCandidateEarlistDueDate(CandidateList& cl)
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

static SelectFunction getSelectCandidate(IS algo)
{
    switch (algo) {
    case IS::EDD:
        return selectCandidateEarlistDueDate;
    case IS::ML:
        return selectCandidateMaxLateness;
    }
    throw std::invalid_argument("invalid initial solution algorithm argument");
}

Solution Solution::generateInitialSolution(
    const Instance& instance,
    Solution::InitialSolution initalSoluationAlgorithm)
{
    // REFACTOR(igolt): isso aqui com certeza tem como melhorar em todos os
    // aspectos
    Solution s;

    s._completionTime = 0;
    s._completionTimes.resize(instance.jobs().size());
    s._maxLateness         = INT_MIN;
    s._maxLatenessJobLabel = 0;
    s._jobSequence.reserve(instance.jobs().size());

    auto selectCandidate = getSelectCandidate(initalSoluationAlgorithm);

    // PERF(igolt): lista de candidatos pode ser uma estrutura que permita
    // achar o maior de forma mais eficiente
    auto candidateList = candidateListFromInstance(instance);

    while (!candidateList.empty()) {
        auto candidate = selectCandidate(candidateList);
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

Solution Solution::swap(unsigned a, unsigned b, const Instance& instance){
    Solution s;
    s._completionTime = 0;
    s._completionTimes.resize(this->_jobSequence.size());
    s._maxLateness         = INT_MIN;
    s._maxLatenessJobLabel = 0;
    s._jobSequence.reserve(this->_jobSequence.size());
    for(size_t i = 0; i < this->_jobSequence.size(); i++){
        if(i == a){
            s.addJob(*(this->_jobSequence[b]), instance);
        } else if(i == b) {
            s.addJob(*(this->_jobSequence[a]), instance);
        } else {
            s.addJob(*(this->_jobSequence[i]), instance);
        }
    }
    return s;
}
