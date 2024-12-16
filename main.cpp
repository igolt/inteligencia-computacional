#include <cctype>
#include <cstdio>
#include <exception>
#include <fstream>
#include <iostream>
#include <istream>
#include <stdexcept>
#include <string>
#include <vector>

using IntList = std::vector<int>;

struct ReadResult {
    IntList p, d, f, s, a, b;
    double height;
    int numberOfJobs;
    int numberOfFamilies;
    char setupTimeClass;
};

ReadResult readFile(std::istream& is);

std::ostream& operator<<(std::ostream& os, const IntList& v);

int getNonBlankChar(std::istream&);

std::istream& readList(IntList& v, std::istream& is);

int main(int argc, const char *argv[])
{
    const char *progName = argv[0];
    const char *fileName;
    ReadResult rr;
    std::ifstream inputFile;

    if (argc != 2) {
        std::cerr << "usage: " << progName << " FILE\n";
        return 1;
    }

    fileName = argv[1];
    inputFile.open(fileName);

    if (!inputFile.is_open()) {
        std::cerr << progName << ": failed opening file " << fileName << '\n';
        return 2;
    }

    try {
        rr = readFile(inputFile);
    } catch (const std::exception& e) {
        std::cerr << progName << ": failed to parse input file: " << e.what()
                  << '\n';
        return 3;
    }

    std::cout << "Jobs: " << rr.numberOfJobs << '\n'
              << "Families: " << rr.numberOfFamilies << '\n'
              << "H: " << rr.height << '\n'
              << "Setup time class: " << rr.setupTimeClass << '\n'
              << "P: " << rr.p << '\n'
              << "D: " << rr.d << '\n'
              << "F: " << rr.f << '\n'
              << "S: " << rr.s << '\n'
              << "A: " << rr.a << '\n'
              << "B: " << rr.b << '\n';

    inputFile.close();
    return 0;
}

template <typename T>
inline void tryReadField(std::istream& is, T& field,
                         const std::string& fieldName)
{
    if (!(is >> field)) {
        throw std::runtime_error("could not read " + fieldName);
    }
}

ReadResult readFile(std::istream& is)
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

inline int getNonBlankChar(std::istream& is)
{
    int c;

    while (isspace(c = is.get()))
        continue;
    return c;
}

inline void expectChar(int expected, std::istream& is)
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

std::istream& readList(IntList& v, std::istream& is)
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

std::ostream& operator<<(std::ostream& os, const IntList& v)
{
    os << '[';
    if (v.size()) {
        os << v[0];
        for (int i : v) {
            os << ", " << i;
        }
    }
    return os << ']';
}
