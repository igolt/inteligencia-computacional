#include "GA.hpp"
#include "Instance.hpp"
#include "LocalSearch.hpp"
#include "SimulatedAnnealing.hpp"
#include "Solution.hpp"

#include <fstream>
#include <iostream>

inline std::ostream& operator<<(std::ostream& o, const Solution& s)
{
    s.printJobSequence(o);
    return o;
}

int main(int argc, char *argv[])
{
    std::ofstream csv;

    csv.open("benchmark.csv");

    if (!csv.is_open()) {
        std::cerr << "falha ao abrir o arquivo CSV\n";
        return 1;
    }

    csv << "Instance;EDD;EDD + LS Lmax;EDD + LS Order;GA Lmax;GA Imp (%);GA "
           "Order;GA Time (ms);SA "
           "Lmax;SA Imp (%);SA Order;SA Time "
           "(ms)\n";

    GA ga;
    SimulatedAnnealing sa;
    LocalSearch ls;
    for (int i = 1; i < argc; i++) {
        const char *fileName = argv[i];

        std::cout << "rodando para: " << fileName << "...\n";

        auto instance   = Instance::fromFile(fileName);
        Solution eddSol = Solution::generateInitialSolution(
            instance, Solution::InitialSolution::EDD);
        auto eddLsSol = ls.run(eddSol, 10000, instance, false);

        auto gaSol    = ga.run(100, 100, 0.01, false, instance);
        auto saSol    = sa.run(eddLsSol, 1000, instance, false);

        double eddLsSolLateness =
            eddLsSol.maxLateness() == 0 ? 1 : eddLsSol.maxLateness();

        double gaImp = ((eddLsSol.maxLateness() - gaSol.maxLateness()) /
                        eddLsSolLateness) *
                       100;
        double saImp = ((eddLsSol.maxLateness() - saSol.maxLateness()) /
                        eddLsSolLateness) *
                       100;

        std::cout << "GA IMP: " << gaImp << "\n";
        std::cout << "SA IMP: " << saImp << "\n";

        csv << fileName << ";" << eddSol.maxLateness() << ";"
            << eddLsSol.maxLateness() << ";" << eddLsSol << ";"
            << gaSol.maxLateness() << ";" << gaImp << ';' << gaSol << ';'
            << ga.executionTimeMS() << ";" << saSol.maxLateness() << ";"
            << saImp << ';' << saSol << ';' << sa.executionTimeMS() << '\n';
    }
    return 0;
}
