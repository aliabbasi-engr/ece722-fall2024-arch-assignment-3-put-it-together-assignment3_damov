/*
 * Assignment 1: ROB and LSQ Integration
 * Student: Ali Abbasi
 * Course: Advanced Computer Architecture (ECE722)
 * Insturcture: Dr. Mohamed Hassan
 * Fall 2024, McMaster University
*/

#ifndef ROB_H
#define ROB_H

#include <vector>
#include "Instruction.h"

class ROB {

public:
    ROB(int max_entries, int ipc);

    bool canAccept() const;
    void allocate(const Instruction& inst);
    void commit(uint64_t msgId);
    int retire();
    int step();
    const Instruction& getFrontElement() const;
    Instruction& getElementAt(int index);

    int getNumEntries() const;
    int getMaxEntries() const;
    int getIPC() const;

    void setIPC(int ipc);

private:
    int max_entries;
    int num_entries;
    int IPC;
    std::vector<Instruction> rob_q;
};

#endif // ROB_H
