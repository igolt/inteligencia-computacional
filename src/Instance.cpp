#include "Instance.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

using IntList = std::vector<int>;

struct ReadResult {
    IntList p, d, f, s, a, b;
    double height;
    unsigned numberOfJobs;
    unsigned numberOfFamilies;
    char setupTimeClass;
};

static ReadResult readFile(std::istream& is);

static int getNonBlankChar(std::istream&);

static std::istream& readList(IntList& v, std::istream& is);

// =============================================================================

Instance Instance::fromFile(const std::string& fileName)
{
    std::ifstream inputFile;

    inputFile.open(fileName);
    if (!inputFile.is_open()) {
        throw std::runtime_error("failed to open " + fileName);
    }
    return Instance::fromFile(inputFile);
}

Instance Instance::fromFile(std::istream& is)
{
    // PERF(igolt): provavelmente tem como otimizar se não usar uma estrutura
    // intermediária
    ReadResult rr = readFile(is);
    Instance instance;

    instance._jobs.reserve(rr.numberOfJobs);
    for (unsigned i = 0; i < rr.numberOfJobs; ++i) {
        instance._jobs.push_back(Job(i + 1, rr.f[i], rr.d[i], rr.p[i]));
    }
    instance._numberOfFamilies = rr.numberOfFamilies;
    instance._setupTimeClass   = rr.setupTimeClass;
    instance._s                = std::move(rr.s);
    return instance;
}

template <typename T>
static inline void tryReadField(std::istream& is, T& field,
                                const std::string& fieldName)
{
    if (!(is >> field)) {
        throw std::runtime_error("could not read " + fieldName);
    }
}

static ReadResult readFile(std::istream& is)
{
    ReadResult r;

    tryReadField(is, r.numberOfJobs, "number of jobs");
    tryReadField(is, r.numberOfFamilies, "number of families");
    tryReadField(is, r.height, "height");
    tryReadField(is, r.setupTimeClass, "setup time class");

    int remainingFields = 6;
    int c;
    while ((c = getNonBlankChar(is)) != EOF && remainingFields) {
        IntList *v;
        size_t sz      = r.numberOfJobs;
        char fieldName = char(c);
        switch (fieldName) {
        case 'P':
            v = &r.p;
            break;
        case 'D':
            v = &r.d;
            break;
        case 'F':
            v = &r.f;
            break;
        case 'S':
            v  = &r.s;
            sz = r.numberOfFamilies * r.numberOfFamilies;
            break;
        case 'A':
            v = &r.a;
            break;
        case 'B':
            v = &r.b;
            break;
        default:
            throw std::runtime_error(std::string("invalid field ") + fieldName);
        }
        if (v->size() != 0) {
            throw std::runtime_error(fieldName +
                                     std::string(" already initialized"));
        }
        v->reserve(sz);
        readList(*v, is);
        --remainingFields;
    }

    if (remainingFields) {
        // TODO(igolt): listar os campos que não foram lidos
        throw std::runtime_error("could not read all fields from file");
    }

    return r;
}

static inline int getNonBlankChar(std::istream& is)
{
    int c;

    while (isspace(c = is.get()))
        continue;
    return c;
}

static inline void expectChar(int expected, std::istream& is)
{
    int c;

    c = getNonBlankChar(is);
    if (expected != c) {
        std::string got = "";
        if (c == EOF) {
            got += "EOF";
        } else {
            got += char(c);
        }
        throw std::runtime_error("expected " +
                                 (char(expected) + ((" got ") + got)));
    }
}

static std::istream& readList(IntList& v, std::istream& is)
{
    expectChar(':', is);
    expectChar('[', is);

    std::string s;
    while (true) {
        if (!(is >> s)) {
            throw std::runtime_error("expected a number or ] got EOF");
        }
        if (s == "]") {
            break;
        }
        try {
            v.push_back(std::stoi(s, nullptr, 10));
        } catch (const std::invalid_argument& e) {
            throw std::runtime_error("failed parsing list: " + s +
                                     " is not a valid integer");
        }
    }
    return is;
}

const Instance::JobList& Instance::jobs() const { return this->_jobs; }

int Instance::s(int i, int j) const
{
    return this->_s[(i - 1) * this->_numberOfFamilies + (j - 1)];
}

std::ostream& operator<<(std::ostream& os, const IntList& v)
{
    os << '[';
    if (v.size()) {
        os << v[0];
        for (auto it = ++v.begin(); it < v.end(); it++) {
            os << ", " << *it;
        }
    }
    return os << ']';
}

std::ostream& Instance::print(std::ostream& os) const
{
    os << "s: " << this->_s << '\n';
    for (const Job& j : this->_jobs) {
        os << "Job(label=" << j.label() << ",family=" << j.family()
           << ",dueDate=" << j.dueDate()
           << ",processingTime=" << j.processingTime() << ")\n";
    }
    return os;
}

/*
   csv << "File,Number of jobs,Numer of families,Setup Class,Setup "
           "Times,Distance "
           "Index,Lateness,Jobs order,Jobs families, Execution Time\n";
*/
std::ostream& Instance::toCsv(std::ostream& csv) const
{
    csv << this->jobs().size() << ";" << this->_numberOfFamilies << ";"
        << this->_setupTimeClass << ";" << this->_s << ";"
        << "(distance idx);";
    return csv;
}
