#ifndef JOB_HPP
#define JOB_HPP

class Job {
private:
    int _label;
    int _family;
    int _dueDate;
    int _processingTime;

public:
    Job(int label);

    int label() const;
    int family() const;
    int dueDate() const;
    int processingTime() const;

    void setLabel(int label);
    void setFamily(int family);
    void setDueDate(int dueDate);
    void setProcessingTime(int processingTime);
};

#endif /* JOB_HPP */
