/*
 * File  :      MCoreSimProjectXml.h
 * Author:      Salah Hessien
 * Email :      salahga@mcmaster.ca
 *
 * Created On February 16, 2020
 */

/*
 * Modified as a part of a course assignemnt
 * Assignment 1: ROB and LSQ Integration
 * Student: Ali Abbasi
 * Course: Advanced Computer Architecture (ECE722)
 * Insturcture: Dr. Mohamed Hassan
 * Fall 2024, McMaster University
*/

#include "../header/CpuCoreGenerator.h"

// include new classes
#include "../new/ROB.cc"
#include "../new/LSQ.cc"
#include "../new/Request.cc"
#include "../new/Instruction.cc"

// simulation parameters
#define CUSTOM_DEBUG 0
#define OOO_STAGES 4
#define IPC 4
#define LSQ_SIZE 32
#define ROB_SIZE 128

namespace ns3 {

    // define ROB and LSQ
    ROB m_rob(ROB_SIZE, IPC);
    LSQ m_lsq(LSQ_SIZE);

    // define template memory and compute instructions
    Instruction memInstruction(Instruction::LOAD, 0, 0, false);
    Instruction computeInstruction(Instruction::COMPUTE, 0, 0, false);

    // define global variables
    int total_num_compute_inst = 0;
    int total_num_mem_inst = 0;
    int current_num_compute_inst = 0;
    int rob_retired_count = 0;
    int lsq_retired_count = 0;

    // override ns3 type
    TypeId CpuCoreGenerator::GetTypeId(void) {
        static TypeId tid = TypeId("ns3::CpuCoreGenerator")
               .SetParent<Object > ();
        return tid;
    }

    // The only constructor
    CpuCoreGenerator::CpuCoreGenerator(CpuFIFO* associatedCpuFIFO) {
        // default
        m_coreId         = 1;
        m_cpuCycle       = 1;
        m_bmFileName     = "trace_C0.trc";
        m_dt             = 1;
        m_clkSkew        = 0;
        m_cpuMemReq      = CpuFIFO::ReqMsg();
        m_cpuMemResp     = CpuFIFO::RespMsg();
        m_cpuFIFO        = associatedCpuFIFO;
        m_cpuReqDone     = false;
        m_newSampleRdy   = false;
        m_cpuCoreSimDone = false;
        m_logFileGenEnable = false;
        m_prevReqFinish    = true;
        m_prevReqFinishCycle = 0;
        m_prevReqArriveCycle = 0;
        m_cpuReqCnt      = 0;
        m_cpuRespCnt     = 0;
        m_number_of_OoO_requests = 1;
    }

    // We don't do any dynamic allocations
    CpuCoreGenerator::~CpuCoreGenerator() {
    }

    // set Benchmark file name
    void CpuCoreGenerator::SetBmFileName (std::string bmFileName) {
        m_bmFileName = bmFileName;
    }

    void CpuCoreGenerator::SetCpuTraceFile (std::string fileName) {
        m_cpuTraceFileName = fileName; 
    }

    void CpuCoreGenerator::SetCtrlsTraceFile (std::string fileName) {
        m_CtrlsTraceFileName = fileName;
    }

    // set CoreId
    void CpuCoreGenerator::SetCoreId (int coreId) {
      m_coreId = coreId;
    }

    // get core id
    int CpuCoreGenerator::GetCoreId () {
      return m_coreId;
    }

    // set dt
    void CpuCoreGenerator::SetDt (double dt) {
      m_dt = dt;
    }

    // get dt
    int CpuCoreGenerator::GetDt () {
      return m_dt;
    }

    // set clk skew
    void CpuCoreGenerator::SetClkSkew (double clkSkew) {
       m_clkSkew = clkSkew;
    }

    // get simulation done flag
    bool CpuCoreGenerator::GetCpuSimDoneFlag() {
      return m_cpuCoreSimDone;
    }

    void CpuCoreGenerator::SetLogFileGenEnable (bool logFileGenEnable ) {
      m_logFileGenEnable = logFileGenEnable;
    }

    void CpuCoreGenerator::SetOutOfOrderStages(int stages)
    {
      //m_number_of_OoO_requests = stages;
      m_number_of_OoO_requests = OOO_STAGES;
      std::cout<<"stages="<<m_number_of_OoO_requests<<std::endl;
    }
    
    // The init function starts the generator calling once very m_dt NanoSeconds.
    void CpuCoreGenerator::init() {
        m_bmTrace.open(m_bmFileName.c_str());
        Simulator::Schedule(NanoSeconds(m_clkSkew), &CpuCoreGenerator::Step, Ptr<CpuCoreGenerator > (this));
    }

    // This function does most of the functionality.
    void CpuCoreGenerator::ProcessTxBuf() {

        std::string fline;

        Logger::getLogger()->setClkCount(this->m_coreId, this->m_cpuCycle);
        
        // check if a request is read from file
        if (m_newSampleRdy == true) {

          // check if there are remaining compute instructions in this request
          // and if ROB has space to accept
          while (current_num_compute_inst != 0 && m_rob.canAccept() == true){
            
            // set MsgId of the instruction
            computeInstruction.setMsgId(IdGenerator::nextReqId());

            // allocate the instruction to ROB 
            m_rob.allocate(computeInstruction);

            // commit the instruction in ROB immediately
            m_rob.commit(computeInstruction.getMsgID());

            if (CUSTOM_DEBUG) {
              std::cout << "[abbasa46]" << " [Cycle " << m_cpuCycle << "] "
                        << "Compute instruction allocated to ROB and committed: ReqId = "
                        << computeInstruction.getMsgID() << std::endl;
            }

            // increment the total number of issued instructions
            // used for stopping the simulation
            m_cpuReqCnt++;

            // increment the total number of compute instructions
            // used for report generation
            total_num_compute_inst++;

            // decrement the number of remaining compute instructions
            // for the current read sample request
            current_num_compute_inst--;
          }

          // if all compute instructions for the current request is sent
          // and if both ROB and LSQ have space to accept
          if (current_num_compute_inst == 0 && m_rob.canAccept() == true && m_lsq.canAccept() == true){
            
            memInstruction.setMsgId(IdGenerator::nextReqId());

            // allocate the instruction to both ROB and LSQ
            m_rob.allocate(memInstruction);
            m_lsq.allocate(memInstruction);

            if (CUSTOM_DEBUG) {
              std::cout << "[abbasa46]" << " [Cycle " << m_cpuCycle << "] " << "Memory instruction allocated to ROB and LSQ: ReqId = "
                        << memInstruction.getMsgID() << ", addr = " << memInstruction.getAddr() << ", type = " << memInstruction.getTypeString()
                        << ", ready = " << memInstruction.isReady() << std::endl;
            }

            // if the current instruction is a load and a previous store to the same address is present in the LSQ
            // if (memInstruction.getType() == Instruction::LOAD && m_lsq.storeAvailable(memInstruction.getAddr()) == true) {
            //   // commit the load instruction immediately as ldFwd()
            //   m_rob.commit(memInstruction.getMsgID());
            //   m_lsq.commit(memInstruction.getMsgID());
            //   if (CUSTOM_DEBUG) {
            //     std::cout << "[abbasa46]" << " [Cycle " << m_cpuCycle << "] "
            //               << "Memory instruction committed in ROB and LSQ due to ldFwd: ReqId = "
            //               << memInstruction.getMsgID() << ", addr = " << std::to_string(memInstruction.getAddr()) << std::endl;
            //   }
            // }

            // increment the total number of issued instructions
            // to be used for stopping the simulation
            m_cpuReqCnt++;

            // increment the total number of memory instructions
            // used for report generation
            total_num_mem_inst++;

            // invalidate the current sample request
            // since all compute instructions and the memory instruction are sent
            m_newSampleRdy = false;
          }
        }
        
        // if the current sample request is fully issued
        // read from file and construct a new sample
        if (m_newSampleRdy == false) {

          // read one line
          if (getline(m_bmTrace,fline)) {

            // new sample read
            m_newSampleRdy    = true;

            // string formatting
            std::istringstream iss(fline);
            std::string addr;
            char typeChar;
            if (iss >> current_num_compute_inst >> addr >> typeChar) {
              
              // construct the sample request
              Instruction::Type type_enum = (typeChar == 'R') ? Instruction::LOAD : Instruction::STORE;
              memInstruction.setType(type_enum);
              memInstruction.setAddr(std::stoull(addr, nullptr, 16));
              memInstruction.setReady(false);

              if (CUSTOM_DEBUG) {
                std::cout << "[abbasa46]" << " [Cycle " << m_cpuCycle << "] " << "Read a line from file:"
                          << " current_num_compute_inst = " << current_num_compute_inst << ", addr = " << memInstruction.getAddr()
                          << ", type = " << memInstruction.getTypeString() << std::endl;
              }
            }
          }
        }

        // check if the file reached EOF
        // used for stopping the simulation
        if (m_bmTrace.eof()) {
          // std::cout << "[abbasa46]" << " [Cycle " << m_cpuCycle << "] " << "Reached EOF in the trace file." << std::endl;
          m_bmTrace.close();
          m_cpuReqDone = true;
        }           
    } // CpuCoreGenerator::ProcessTxBuf()

    void CpuCoreGenerator::ProcessRxBuf() {

        // process received buffer
        if (!m_cpuFIFO->m_rxFIFO.IsEmpty()) {
          m_cpuMemResp = m_cpuFIFO->m_rxFIFO.GetFrontElement();
          m_cpuFIFO->m_rxFIFO.PopElement();
          Logger::getLogger()->updateRequest(m_cpuMemResp.msgId, Logger::EntryId::CPU_RX_CHECKPOINT);
          m_sent_requests--;
          if(m_sent_requests < 0)
            std::cout << "error" << std::endl;
          if (m_logFileGenEnable) {
            std::cout << "Cpu " << m_coreId << " new response is received at cycle " << m_cpuCycle 
                      << " with msgID = " << m_cpuMemResp.msgId << std::endl;
          }

          // commit the corresponding memory request in both LSQ and ROB
          // (if it is not already committed)
          m_lsq.commit(m_cpuMemResp.msgId);
          m_rob.commit(m_cpuMemResp.msgId);

          if (CUSTOM_DEBUG) {
            std::cout << "[abbasa46]" << " [Cycle " << m_cpuCycle << "] "
                      << "Memory instruction committed in ROB and LSQ: ReqId = "
                      << m_cpuMemResp.msgId << ", addr = " << std::to_string(m_cpuMemResp.addr) << std::endl;
          }
        }
 
        // schedule next run or finish simulation if processing end
        if (m_cpuReqDone == true && m_cpuRespCnt >= m_cpuReqCnt) {
          m_cpuCoreSimDone = true;
          std::cout << "[abbasa46]" << " [Cycle " << m_cpuCycle << "] " << "#instructions:         " << m_cpuReqCnt << std::endl;
          std::cout << "[abbasa46]" << " [Cycle " << m_cpuCycle << "] " << "#compute_instructions: " << total_num_compute_inst << std::endl;
          std::cout << "[abbasa46]" << " [Cycle " << m_cpuCycle << "] " << "#memory_instructions:  " << total_num_mem_inst << std::endl;
          Logger::getLogger()->traceEnd(this->m_coreId);
          std::cout << "Cpu " << m_coreId << " Simulation End @ processor cycle # " << m_cpuCycle << std::endl;
        }
        else {
          // Schedule the next run
          Simulator::Schedule(NanoSeconds(m_dt), &CpuCoreGenerator::Step, Ptr<CpuCoreGenerator > (this));
          m_cpuCycle++;
        }

    } // CpuCoreGenerator::ProcessRxBuf()

    // function to push instructions from LSQ to txFifo
    void CpuCoreGenerator::PushToCache() {

      // check if LSQ is not empty
      // and the number of outstanding requests is less than out-of-order stages
      if (m_lsq.getNumEntries() > 0 && m_sent_requests < m_number_of_OoO_requests) {
      
        // set the instruction initially to null
        Instruction* instrToSend = nullptr;

        // iterate through LSQ
        for (int i = 0; i < m_lsq.getNumEntries(); i++) {
          Instruction& lsqInstr = m_lsq.getElementAt(i);

          // check if the lsqInstr is not ready and has not been pushed into cache
          if (lsqInstr.isReady() == false && lsqInstr.isPushedToCache() == false) {
              
            // if it is a load,
            // select the instruction to push into cache and break LSQ iteration
            if (lsqInstr.getType() == Instruction::LOAD) {
              instrToSend = &lsqInstr;
              break; 
            }

            // if it is a store
            else if (lsqInstr.getType() == Instruction::STORE) {
              
              bool foundInROB = false;

              // iterate through ROB to check if the corresponding instruction is
              // in front of the ROB and will be dequeud in the next cycle
              for (int j = 0; j < std::min(m_rob.getIPC(), m_rob.getNumEntries()); j++) {
                Instruction& robInstr = m_rob.getElementAt(j);

                // if the corresponding instruction found,
                // set the flag and break ROB iteration
                if (robInstr.getMsgID() == lsqInstr.getMsgID()) {
                  foundInROB = true;
                  break;
                }
              }

              // if the flag is set
              // select the instruction to push into cache and break LSQ iteration
              if (foundInROB == true) {
                instrToSend = &lsqInstr;
                break;
              }
            }
          }
        }

        // if the instToSend is no longer null
        if (instrToSend != nullptr) {

          // contruct the request
          m_cpuMemReq.addr = instrToSend->getAddr();
          m_cpuMemReq.type = (instrToSend->getType() == Instruction::LOAD) ? CpuFIFO::REQTYPE::READ : CpuFIFO::REQTYPE::WRITE;
          m_cpuMemReq.cycle = 0;
          m_cpuMemReq.msgId = instrToSend->getMsgID();
          m_cpuMemReq.reqCoreId = m_coreId;
          m_cpuMemReq.fifoInserionCycle = m_cpuCycle;

          // push into txFIFO
          m_cpuFIFO->m_txFIFO.InsertElement(m_cpuMemReq);
          m_sent_requests++;

          if (CUSTOM_DEBUG) {
            std::cout << "[abbasa46]" << " [Cycle " << m_cpuCycle << "] "
                      << "Memory instruction inserted into txFIFO: ReqId = " << m_cpuMemReq.msgId << ", addr = " 
                      << m_cpuMemReq.addr << ", type = " << instrToSend->getTypeString() << std::endl;
          }

          // if the pushed instruction is a write
          // commit immediately
          if (m_cpuMemReq.type == CpuFIFO::REQTYPE::WRITE) {
            m_lsq.commit(m_cpuMemReq.msgId);
            m_rob.commit(m_cpuMemReq.msgId);
          }

          // set the flag that the instruction is pushed into cache
          instrToSend->setPushedToCache(true);
          
          Logger::getLogger()->addRequest(this->m_coreId, m_cpuMemReq);

          if (m_logFileGenEnable) {
              std::cout << "Cpu " << m_coreId << " MemReq: ReqId = " << m_cpuMemReq.msgId << ", CpuRefCycle = "
                        << m_cpuMemReq.cycle << ", CpuClkTic ==================================================== " << m_cpuCycle << std::endl;
              std::cout << "\t\tMemAddr = " << m_cpuMemReq.addr << ", ReqType (0:Read, 1:Write) = " << m_cpuMemReq.type
                        << ", CpuTxFIFO Size = " << m_cpuFIFO->m_txFIFO.GetQueueSize() << std::endl << std::endl;
          }
        }
      }
    } // CpuCoreGenerator::PushToCache()

    /**
     * Runs one mobility Step for the given vehicle generator.
     * This function is called each interval dt
     */
    void CpuCoreGenerator::Step(Ptr<CpuCoreGenerator> cpuCoreGenerator) {

        if (CUSTOM_DEBUG) {
          if (m_lsq.getNumEntries() != 0)
            std::cout << "[abbasa46]" << " [Cycle " << cpuCoreGenerator->m_cpuCycle << "] "
                      << "Front of LSQ before LSQ::Step: ReqId = " << m_lsq.getFrontElement().getMsgID() << std::endl;
          else
            std::cout << "[abbasa46]" << " [Cycle " << cpuCoreGenerator->m_cpuCycle << "] "
                      << "Front of LSQ before LSQ::Step: EMPTY" << std::endl;
        }

        lsq_retired_count = m_lsq.step();

        if (CUSTOM_DEBUG) {
          std::cout << "[abbasa46]" << " [Cycle " << cpuCoreGenerator->m_cpuCycle << "] "
                    << "Retired from LSQ: " << lsq_retired_count << std::endl;
        
          if (m_lsq.getNumEntries() != 0)
            std::cout << "[abbasa46]" << " [Cycle " << cpuCoreGenerator->m_cpuCycle << "] "
                      << "Front of LSQ after LSQ::Step: ReqId = " << m_lsq.getFrontElement().getMsgID() << std::endl;
          else
            std::cout << "[abbasa46]" << " [Cycle " << cpuCoreGenerator->m_cpuCycle << "] "
                      << "Front of LSQ after LSQ::Step: EMPTY" << std::endl;
        }

        // -------------------------------------
        // -------------------------------------

        if (CUSTOM_DEBUG) {
          if (m_rob.getNumEntries() != 0)
            std::cout << "[abbasa46]" << " [Cycle " << cpuCoreGenerator->m_cpuCycle << "] "
                      << "Front of ROB before ROB::Step: ReqId = " << m_rob.getFrontElement().getMsgID() << std::endl;
          else  
            std::cout << "[abbasa46]" << " [Cycle " << cpuCoreGenerator->m_cpuCycle << "] "
                      << "Front of ROB before ROB::Step: EMPTY" << std::endl;
        }

        rob_retired_count = m_rob.step();
        cpuCoreGenerator->m_cpuRespCnt += rob_retired_count;

        if (CUSTOM_DEBUG) {
          std::cout << "[abbasa46]" << " [Cycle " << cpuCoreGenerator->m_cpuCycle << "] "
                    << "Retired from ROB: " << rob_retired_count << std::endl;

          if (m_rob.getNumEntries() != 0)
            std::cout << "[abbasa46]" << " [Cycle " << cpuCoreGenerator->m_cpuCycle << "] "
                      << "Front of ROB after ROB::Step: ReqId = " << m_rob.getFrontElement().getMsgID() << std::endl;
          else
            std::cout << "[abbasa46]" << " [Cycle " << cpuCoreGenerator->m_cpuCycle << "] "
                      << "Front of ROB after ROB::Step: EMPTY" << std::endl;
        }

        // -------------------------------------
        // -------------------------------------

        cpuCoreGenerator->ProcessRxBuf();

        // -------------------------------------
        // -------------------------------------

        cpuCoreGenerator->ProcessTxBuf();
        cpuCoreGenerator->PushToCache();
    }
}
