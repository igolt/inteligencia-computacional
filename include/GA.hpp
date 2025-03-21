#ifndef GA_HPP
#define GA_HPP

#include "Instance.hpp"
#include "Solution.hpp"

class GA {
private:
    void initialPopulation(int popSize, std::vector<Solution>& population,
                           const Instance& instance);
    void survival(int popSize, std::vector<Solution>& population,
                  std::vector<Solution>& nextPopulation);
    int rouletteSelection(const std::vector<Solution>& population, int start,
                          int end);
    void selection(std::vector<Solution>& population, unsigned& parentA,
                   unsigned& parentBC, unsigned top25, unsigned popSize);
    void defineCuts(unsigned& cut1, unsigned& cut2, unsigned numJobs);
    Solution crossover(std::vector<Solution>& population, unsigned parentA,
                       unsigned parentBC, unsigned numJobs,
                       const Instance& instance);
    void mutation(Solution& child, double mutationRate, unsigned numJobs,
                  const Instance& instance);
    Solution bestSolution(const std::vector<Solution>& population);

    std::vector<double> _executionTimes;
    std::vector<int> _maxLatenesses;
    long long _executionTime;

public:
    Solution run(unsigned popSize, int epochs, double mutationRate,
                 bool localSearch, const Instance& instance);
    std::vector<double> executionTimes();
    std::vector<int> maxLatenesses();

    inline long long executionTimeMS() const { return this->_executionTime; }
};

#endif /* GA_HPP */
