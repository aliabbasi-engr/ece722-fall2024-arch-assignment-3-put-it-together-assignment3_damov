/*
 * Assignment 1: ROB and LSQ Integration
 * Student: Ali Abbasi
 * Course: Advanced Computer Architecture (ECE722)
 * Insturcture: Dr. Mohamed Hassan
 * Fall 2024, McMaster University
*/

#include "ROB.h"

ROB::ROB(int max_entries, int ipc) 
    : max_entries(max_entries), num_entries(0), IPC(ipc) {
    rob_q.reserve(max_entries);
}

bool ROB::canAccept() const {
    return num_entries < max_entries;
}

void ROB::allocate(const Instruction& inst) {
    if (canAccept()) {
        rob_q.push_back(inst);
        num_entries++;
    }
}

void ROB::commit(uint64_t msgId) {
    for (auto& inst : rob_q) {
        if (inst.getMsgID() == msgId && inst.isReady() == false) {
            inst.setReady(true);
            break;
        }
    }
}

int ROB::retire() {
    int retire_count = 0;
    while (retire_count < IPC && !rob_q.empty() && rob_q.front().isReady()) {
        rob_q.erase(rob_q.begin());
        retire_count++;
        num_entries--;
    }
    return retire_count;
}

int ROB::step() {
    return retire();
}

const Instruction& ROB::getFrontElement() const {
    if (rob_q.empty()) {
        std::cout << "[abbasa46] ROB is empty. Cannot peek at the front instruction." << std::endl;
    }
    return rob_q.front();
}

Instruction& ROB::getElementAt(int index) {
    if (index >= num_entries || index < 0) {
        throw std::out_of_range("Index is out of bounds in ROB.");
    }
    return rob_q[index];
}


int ROB::getNumEntries() const {
    return num_entries;
}

int ROB::getMaxEntries() const {
    return max_entries;
}

int ROB::getIPC() const {
    return IPC;
}

void ROB::setIPC(int ipc) {
    this->IPC = ipc;
}