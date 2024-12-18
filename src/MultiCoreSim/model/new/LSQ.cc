/*
 * Assignment 1: ROB and LSQ Integration
 * Student: Ali Abbasi
 * Course: Advanced Computer Architecture (ECE722)
 * Insturcture: Dr. Mohamed Hassan
 * Fall 2024, McMaster University
*/

#include "LSQ.h"

LSQ::LSQ(int max_entries) 
    : max_entries(max_entries), num_entries(0) {
    lsq_q.reserve(max_entries);
}

bool LSQ::canAccept() const {
    return num_entries < max_entries;
}

void LSQ::allocate(const Instruction& inst) {
    if (canAccept()) {
        lsq_q.push_back(inst);
        num_entries++;
    }
}

void LSQ::commit(uint64_t msgId) {
    for (auto& inst : lsq_q) {
        if (inst.getMsgID() == msgId && inst.isReady() == false) {
            inst.setReady(true);
            break;
        }
    }
}

int LSQ::retire() {
    int retire_count = 0;
    auto iterator = lsq_q.begin();
    while (iterator != lsq_q.end()) {
        if (iterator->isReady()) {
            iterator = lsq_q.erase(iterator);
            retire_count++;
            num_entries--;
        }
        else {
            ++iterator;
        }
    }
    return retire_count;
}

int LSQ::retire(ROB& rob) {

    std::cout << "HERE 2" << std::endl;
    int retire_count = 0;
    auto lsq_iterator = lsq_q.begin();
    while (lsq_iterator != lsq_q.end()) {
        std::cout << "HERE 3" << std::endl;
        if (lsq_iterator->isReady()) {
            std::cout << "HERE 4" << std::endl;
            if (lsq_iterator->getType() == Instruction::LOAD) {
                std::cout << "HERE 5" << std::endl;
                lsq_iterator = lsq_q.erase(lsq_iterator);
                retire_count++;
                num_entries--;
            }
            else {
                std::cout << "HERE 6" << std::endl;
                for (int i = 0; i < rob.getNumEntries() && i < rob.getIPC(); i++) {
                    std::cout << "HERE 7" << std::endl;
                    Instruction& rob_iterator = rob.getElementAt(i);
                    if (rob_iterator.getMsgID() == lsq_iterator->getMsgID())
                    {
                        std::cout << "HERE 8" << std::endl;
                        lsq_iterator = lsq_q.erase(lsq_iterator);
                        retire_count++;
                        num_entries--;
                        break;;
                    }
                }
            }
        }
        else {
            std::cout << "HERE 9" << std::endl;
            ++lsq_iterator;
        }
    }
    return retire_count;
}

bool LSQ::storeAvailable(uint64_t addr) {
    for (auto& inst : lsq_q) {
        if (inst.getAddr() == addr && inst.getType() == Instruction::STORE) {
            return true;
        }
    }
    return false;
}

const Instruction& LSQ::getFrontElement() const {
    if (lsq_q.empty()) {
        std::cout << "[abbasa46] LSQ is empty. Cannot peek at the front instruction." << std::endl;
    }
    return lsq_q.front();
}

Instruction& LSQ::getElementAt(int index) {
    if (index >= num_entries || index < 0) {
        throw std::out_of_range("Index is out of bounds in LSQ.");
    }
    return lsq_q[index];
}

int LSQ::step() {
    return retire();
}

int LSQ::step(ROB& rob) {
    std::cout << "HERE 1" << std::endl;
    return retire(rob);
}

int LSQ::getNumEntries() const {
    return num_entries;
}

int LSQ::getMaxEntries() const {
    return max_entries;
}
