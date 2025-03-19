#include "Job.hpp"

#include <sstream>
#include <stdexcept>

Job::Job(int label, int family, int dueDate, int processingTime)
    : _label(label), _family(family), _dueDate(dueDate),
      _processingTime(processingTime)
{
}

int Job::label() const { return this->_label; }

int Job::family() const { return this->_family; }

int Job::dueDate() const { return this->_dueDate; }

int Job::processingTime() const { return this->_processingTime; }

Job& Job::setLabel(int label)
{
    if (label <= 0) {
        throw std::invalid_argument("label should be greater than 0");
    }
    this->_label = label;
    return *this;
}

Job& Job::setFamily(int family)
{
    if (family <= 0) {
        throw std::invalid_argument("family should be greater than 0");
    }
    this->_family = family;
    return *this;
}

Job& Job::setDueDate(int dueDate)
{
    if (dueDate < 0) {
        std::stringstream strstream;

        strstream << "due date should be greater than 0 (got " << dueDate
                  << ")";
        throw std::invalid_argument(strstream.str());
    }
    this->_dueDate = dueDate;
    return *this;
}

Job& Job::setProcessingTime(int processingTime)
{
    if (processingTime <= 0) {
        throw std::invalid_argument("processingTime should be greater than 0");
    }
    this->_processingTime = processingTime;
    return *this;
}
