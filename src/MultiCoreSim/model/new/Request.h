/*
 * Assignment 1: ROB and LSQ Integration
 * Student: Ali Abbasi
 * Course: Advanced Computer Architecture (ECE722)
 * Insturcture: Dr. Mohamed Hassan
 * Fall 2024, McMaster University
*/

#ifndef REQUEST_H
#define REQUEST_H

class Request {
public:
    enum Type { WRITE, READ, INVALID };

    Request(int num_compute_inst, const uint64_t addr, Type type);

    int getNumComputeInst() const;
    void setNumComputeInst(int num_compute_inst);

    uint64_t getAddr() const;
    void setAddr(const uint64_t addr);

    Type getType() const;
    void setType(Type type);

private:
    int num_compute_inst;
    uint64_t addr;
    Type type;
};

#endif // REQUEST_H
