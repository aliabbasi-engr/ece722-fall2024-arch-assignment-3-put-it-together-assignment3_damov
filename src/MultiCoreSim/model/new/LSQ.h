/*
 * Assignment 1: ROB and LSQ Integration
 * Student: Ali Abbasi
 * Course: Advanced Computer Architecture (ECE722)
 * Insturcture: Dr. Mohamed Hassan
 * Fall 2024, McMaster University
*/

#ifndef LSQ_H
#define LSQ_H

#include <vector>
#include "Instruction.h"
#include "../header/CpuCoreGenerator.h"

class LSQ {

public:
    LSQ(int max_entries);

    bool canAccept() const;
    void allocate(const Instruction& inst);
    void commit(uint64_t msgId);
    int retire();
    int retire(ROB& rob);
    int step();
    int step(ROB& rob);
    bool storeAvailable(uint64_t addr);
    const Instruction& getFrontElement() const;
    Instruction& getElementAt(int index);

    int getNumEntries() const;
    int getMaxEntries() const;

private:
    int max_entries;
    int num_entries;
    std::vector<Instruction> lsq_q;
};

#endif // LSQ_H
