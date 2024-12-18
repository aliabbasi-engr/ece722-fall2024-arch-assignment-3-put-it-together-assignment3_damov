/*
 * Assignment 1: ROB and LSQ Integration
 * Student: Ali Abbasi
 * Course: Advanced Computer Architecture (ECE722)
 * Insturcture: Dr. Mohamed Hassan
 * Fall 2024, McMaster University
*/

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

class Instruction {

public:
    enum Type { LOAD, STORE, COMPUTE, INVALID };

    Instruction(Type type, uint64_t addr, uint64_t msgId, bool ready);

    Type getType() const;
    std::string getTypeString() const;
    void setType(Type type);

    uint64_t getAddr() const;
    void setAddr(uint64_t addr);

    uint64_t getMsgID() const;
    void setMsgId(uint64_t msgId);

    bool isReady() const;
    void setReady(bool ready);

    bool isPushedToCache() const;
    void setPushedToCache(bool pushedToCache);

    static std::string TypeToString(Type type);

private:
    Type type;
    uint64_t addr;
    uint64_t msgId;
    bool ready;
    bool pushedToCache;
};

#endif // INSTRUCTION_H
