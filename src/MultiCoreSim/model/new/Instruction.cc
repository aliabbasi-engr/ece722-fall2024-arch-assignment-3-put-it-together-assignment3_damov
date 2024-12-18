/*
 * Assignment 1: ROB and LSQ Integration
 * Student: Ali Abbasi
 * Course: Advanced Computer Architecture (ECE722)
 * Insturcture: Dr. Mohamed Hassan
 * Fall 2024, McMaster University
*/

#include "Instruction.h"

Instruction::Instruction(Type type, uint64_t addr, uint64_t msgId, bool ready)
    : type(type), addr(addr), msgId(msgId), ready(ready) {}

Instruction::Type Instruction::getType() const {
    return type;
}

std::string Instruction::getTypeString() const {
    switch (type) {
        case LOAD: return "LOAD";
        case STORE: return "STORE";
        case COMPUTE: return "COMPUTE";
        default: return "INVALID";
    }
}

void Instruction::setType(Type type) {
    this->type = type;
}

uint64_t Instruction::getAddr() const {
    return addr;
}

void Instruction::setAddr(uint64_t addr) {
    this->addr = addr;
}

uint64_t Instruction::getMsgID() const {
    return msgId;
}

void Instruction::setMsgId(uint64_t msgId) {
    this->msgId = msgId;
}

bool Instruction::isReady() const {
    return ready;
}

void Instruction::setReady(bool ready) {
    this->ready = ready;
}

bool Instruction::isPushedToCache() const {
    return pushedToCache;
}

void Instruction::setPushedToCache(bool pushedToCache) {
    this->pushedToCache = pushedToCache;
}
