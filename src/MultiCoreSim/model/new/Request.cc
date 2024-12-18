/*
 * Assignment 1: ROB and LSQ Integration
 * Student: Ali Abbasi
 * Course: Advanced Computer Architecture (ECE722)
 * Insturcture: Dr. Mohamed Hassan
 * Fall 2024, McMaster University
*/

#include "Request.h"

Request::Request(int num_compute_inst, uint64_t addr, Type type)
    : num_compute_inst(num_compute_inst), addr(addr), type(type) {}

int Request::getNumComputeInst() const {
    return num_compute_inst;
}

void Request::setNumComputeInst(int num_compute_inst) {
    this->num_compute_inst = num_compute_inst;
}

uint64_t Request::getAddr() const {
    return addr;
}

void Request::setAddr(uint64_t addr) {
    this->addr = addr;
}

Request::Type Request::getType() const {
    return type;
}

void Request::setType(Type type) {
    this->type = type;
}
