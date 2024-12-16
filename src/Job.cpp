#include "Job.hpp"

#include <stdexcept>

Job::Job(int label): _label(label), _family(0), _dueDate(0), _processingTime(0)
{
}

int Job::label() const { return this->_label; }

int Job::family() const { return this->_family; }

int Job::dueDate() const { return this->_dueDate; }

int Job::processingTime() const { return this->_processingTime; }

void Job::setLabel(int label)
{
    if (label <= 0) {
        throw std::invalid_argument("label should be greater than 0");
    }
    this->_label = label;
}

void Job::setFamily(int family)
{
    if (family <= 0) {
        throw std::invalid_argument("family should be greater than 0");
    }
    this->_family = family;
}

void Job::setDueDate(int dueDate)
{
    if (dueDate <= 0) {
        throw std::invalid_argument("due date should be greater than 0");
    }
    this->_dueDate = dueDate;
}

void Job::setProcessingTime(int processingTime)
{
    if (processingTime <= 0) {
        throw std::invalid_argument("processingTime should be greater than 0");
    }
    this->_processingTime = processingTime;
}
