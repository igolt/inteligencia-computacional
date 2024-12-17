#ifndef INSTANCE_HPP
#define INSTANCE_HPP

#include "Job.hpp"

#include <istream>
#include <ostream>
#include <string>
#include <vector>

class Instance {
public:
    using JobList = std::vector<Job>;

private:
    JobList _jobs;
    std::vector<int> _s;
    unsigned _numberOfFamilies;

public:
    static Instance fromFile(const std::string& fileName);
    static Instance fromFile(std::istream& is);

    const JobList& jobs() const;

    int s(int i, int j) const;

    std::ostream& print(std::ostream& os) const;
};

inline std::ostream& operator<<(std::ostream& os, const Instance& i)
{
    return i.print(os);
}

#endif /* INSTANCE_HPP */
