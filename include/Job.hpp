#ifndef JOB_HPP
#define JOB_HPP

class Job {
private:
    int _label;
    int _family;
    int _dueDate;
    int _processingTime;

public:
    Job(int label, int family, int dueDate, int processingTime);

    int label() const;
    int family() const;
    int dueDate() const;
    int processingTime() const;

    Job& setLabel(int label);
    Job& setFamily(int family);
    Job& setDueDate(int dueDate);
    Job& setProcessingTime(int processingTime);
};

#endif /* JOB_HPP */
