/*
 * File  :      MCoreSimProject.cc
 * Author:      Salah Hessien
 * Email :      salahga@mcmaster.ca
 *
 * Created On February 15, 2020
 */

#include "../header/MCoreSimProject.h"
#include "ns3/simulator.h"

using namespace std;
using namespace ns3;

/*
 * Create the MCoreSimProject for the supplied configuration data
 */

MCoreSimProject::MCoreSimProject(MCoreSimProjectXml projectXmlCfg)
{
  // Set the project xml
  m_projectXmlCfg = projectXmlCfg;

  // Get clock frequency
  m_dt = projectXmlCfg.GetBusClkInNanoSec();
  m_busCycle = 0;

  // Get Run Till Sim End Flag
  m_runTillSimEnd = projectXmlCfg.GetRunTillSimEnd();

  // Get Simulation time to run
  m_totalTimeInSeconds = (m_runTillSimEnd == true) ? std::numeric_limits<int>::max() : projectXmlCfg.GetTotalTimeInSeconds();

  // Enable Log File Generation
  m_logFileGenEnable = projectXmlCfg.GetLogFileGenEnable();

  // setup1(projectXmlCfg);
  // setup2(projectXmlCfg);
  setup4(projectXmlCfg);
  // setup5(projectXmlCfg);
  // setup6(projectXmlCfg);
}

MCoreSimProject::~MCoreSimProject()
{
  for (CpuFIFO *cpuFIFO_ptr : m_cpuFIFO)
  {
    delete cpuFIFO_ptr;
  }
  m_cpuFIFO.clear();

  // delete m_sharedCacheBusIfFIFO;
  // delete m_sharedCacheDRAMBusIfFIFO;
}

void MCoreSimProject::setup1(MCoreSimProjectXml projectXmlCfg)
{
  //   /* Setup 1:
  // - Fixed Latency DRAM
  // - Private L1
  // - One (non-bankized) L2 as LLC*/

  // // initialize Simulator components
  // m_cpuCoreGens = list<Ptr<CpuCoreGenerator>>();
  // m_cpuFIFO = list<CpuFIFO *>();
  // m_cpuCacheCtrl = list<CacheController *>();

  // // Get all cpu configurations from xml
  // list<CacheXml> xmlPrivateCaches = projectXmlCfg.GetPrivateCaches();
  // list<CacheXml> xmlSharedCaches;

  // CacheXml xmlSharedCache = projectXmlCfg.GetSharedCache();
  // xmlSharedCaches.push_back(xmlSharedCache);

  // // Get L1Bus configurations
  // L1BusCnfgXml L1BusCnfg = projectXmlCfg.GetL1BusCnfg();

  // char path_array[256];
  // getcwd (path_array, sizeof(path_array));

  // string path(path_array);
  // //int app_name_index = path.rfind(APP_NAME);
  
  // //m_fsm_protocol_path = path.substr(0, app_name_index + sizeof(APP_NAME) - 1) + string("/Protocols_FSM/");
  // //m_fsm_llc_protocol_path = path.substr(0, app_name_index + sizeof(APP_NAME) - 1) + string("/Protocols_FSM/");
  // m_fsm_protocol_path = path+string("/Protocols_FSM/");
  // m_fsm_llc_protocol_path = path+string("/Protocols_FSM/");
  // cout <<"FSM path: " << m_fsm_protocol_path << endl;

  // // Get Coherence protocol type
  // GetCohrProtocolType();

  // m_maxPendReq = 0;

  // bus = new TripleBus(xmlPrivateCaches, xmlSharedCaches, 
  //   projectXmlCfg.GetBusFIFOSize(), L1BusCnfg.GetReqBusLatcy(), L1BusCnfg.GetRespBusLatcy());

  // // iterate over each core
  // for (list<CacheXml>::iterator it = xmlPrivateCaches.begin(); it != xmlPrivateCaches.end(); it++)
  // {
  //   CacheXml PrivateCacheXml = *it;

  //   /*
  //    * instantiate cpu FIFOs
  //    */
  //   CpuFIFO *newCpuFIFO = new CpuFIFO(PrivateCacheXml.GetCacheId(), projectXmlCfg.GetCpuFIFOSize());
  //   m_cpuFIFO.push_back(newCpuFIFO);

  //   /*
  //    * instantiate cpu cores
  //    */
  //   Ptr<CpuCoreGenerator> newCpuCore = CreateObject<CpuCoreGenerator>(newCpuFIFO);
  //   stringstream bmTraceFile, cpuTraceFile, ctrlTraceFile;
  //   bmTraceFile << projectXmlCfg.GetBMsPath() << "/trace_C" << PrivateCacheXml.GetCacheId() << ".trc.shared";
  //   cpuTraceFile << projectXmlCfg.GetBMsPath() << "/" << projectXmlCfg.GetCpuTraceFile() << PrivateCacheXml.GetCacheId() << ".txt";
  //   ctrlTraceFile << projectXmlCfg.GetBMsPath() << "/" << projectXmlCfg.GetCohCtrlsTraceFile() << PrivateCacheXml.GetCacheId() << ".txt";
  //   double cpuClkPeriod = PrivateCacheXml.GetCpuClkNanoSec();
  //   double cpuClkSkew = cpuClkPeriod * PrivateCacheXml.GetCpuClkSkew() / 100.00;
  //   newCpuCore->SetCoreId(PrivateCacheXml.GetCacheId());
  //   newCpuCore->SetBmFileName(bmTraceFile.str());
  //   newCpuCore->SetCpuTraceFile(cpuTraceFile.str());
  //   newCpuCore->SetCtrlsTraceFile(ctrlTraceFile.str());
  //   newCpuCore->SetDt(cpuClkPeriod);
  //   newCpuCore->SetClkSkew(cpuClkSkew);
  //   newCpuCore->SetLogFileGenEnable(m_logFileGenEnable);
  //   newCpuCore->SetOutOfOrderStages(projectXmlCfg.GetOutOfOrderStages());
  //   m_cpuCoreGens.push_back(newCpuCore);

  //   bm_paths.push_back(bmTraceFile.str());

  //   CommunicationInterface* bus_interface = bus->getInterfaceFor(PrivateCacheXml.GetCacheId());

  //   /*
  //    * instantiate cache controllers
  //    */
  //   CacheController *newCacheCtrl;
  //   if (m_cohrProt == CohProtType::SNOOP_MESI || m_cohrProt == CohProtType::SNOOP_MOESI)
  //     newCacheCtrl = new CacheControllerExclusive(PrivateCacheXml, m_fsm_protocol_path, bus_interface, newCpuFIFO,
  //                                                 projectXmlCfg.GetCache2Cache(), xmlSharedCache.GetCacheId(), m_cohrProt);
  //   else
  //     newCacheCtrl = new CacheController(PrivateCacheXml, m_fsm_protocol_path, bus_interface, newCpuFIFO,
  //                                        projectXmlCfg.GetCache2Cache(), xmlSharedCache.GetCacheId(), m_cohrProt);

  //   m_cpuCacheCtrl.push_back(newCacheCtrl);

  //   if (m_maxPendReq < PrivateCacheXml.GetNPendReq())
  //   {
  //     m_maxPendReq = PrivateCacheXml.GetNPendReq();
  //   }
  // }

  // bus2 = new Bus(xmlSharedCaches, projectXmlCfg.GetDRAMId(), projectXmlCfg.GetBusFIFOSize(), bus->getLowerLevelIds());
  // CommunicationInterface* LLC_bus_interface = bus->getInterfaceFor(xmlSharedCache.GetCacheId());
  // CommunicationInterface* LLC_DRAM_interface = bus2->getInterfaceFor(xmlSharedCache.GetCacheId());

  // m_SharedCacheCtrl = new CacheController_End2End(xmlSharedCache, m_fsm_llc_protocol_path, LLC_DRAM_interface, LLC_bus_interface,
  //                                         projectXmlCfg.GetCache2Cache(), projectXmlCfg.GetDRAMId(), m_llcCohrProt, bus->getLowerLevelIds());


  // CommunicationInterface* DRAM_LLC_interface = bus2->getInterfaceFor(projectXmlCfg.GetDRAMId());
  // m_main_memory = new MainMemoryController(projectXmlCfg, DRAM_LLC_interface, xmlSharedCache.GetCacheId());
  // // m_mcsim_interface = new MCsimInterface(projectXmlCfg, DRAM_LLC_interface, xmlSharedCache.GetCacheId());

  // Logger::getLogger()->registerReportPath(projectXmlCfg.GetBMsPath() + string("/newLogger"));   
}

void MCoreSimProject::setup2(MCoreSimProjectXml projectXmlCfg)
{
   // Setup3 is the more generic
   // setting number of Shared Caches to 1 will lead to Setup2

  /* Setup 2:
  - Fixed Latency DRAM
  - Private L1
  - Single (non-bankized) L2 as LLC*/

  /* Setup 3:
  - Fixed Latency DRAM
  - Private L1
  - Bankized L2 as LLC*/

  // initialize Simulator components
  m_cpuCoreGens = list<Ptr<CpuCoreGenerator>>();
  m_cpuFIFO = list<CpuFIFO *>();
  m_cpuCacheCtrl = list<CacheController *>();

  // Get all cpu configurations from xml
  list<CacheXml> xmlPrivateCaches = projectXmlCfg.GetL1Caches();
  list<CacheXml> xmlSharedCaches = projectXmlCfg.GetSharedCache();

  // extract cacheIds
  std::vector<int> sharedMemIds;
  for (auto& it : xmlSharedCaches) {
    sharedMemIds.push_back(it.GetCacheId());
  } 

  // convert int to list<int>
  vector<int> dramIds;
  dramIds.push_back(projectXmlCfg.GetDRAMId());

  // Get L1Bus configurations
  L1BusCnfgXml L1BusCnfg = projectXmlCfg.GetL1BusCnfg();

  char path_array[256];
  getcwd (path_array, sizeof(path_array));

  string path(path_array);
  //int app_name_index = path.rfind(APP_NAME);
  
  //m_fsm_protocol_path = path.substr(0, app_name_index + sizeof(APP_NAME) - 1) + string("/Protocols_FSM/");
  //m_fsm_llc_protocol_path = path.substr(0, app_name_index + sizeof(APP_NAME) - 1) + string("/Protocols_FSM/");
  m_fsm_protocol_path = path+string("/Protocols_FSM/");
  m_fsm_llc_protocol_path = path+string("/Protocols_FSM/");
  cout <<"FSM path: " << m_fsm_protocol_path << endl;

  // Get Coherence protocol type
  GetCohrProtocolType();

  m_maxPendReq = 0;

  /* Interconnects Setup*/
  // L1s <--> LLC
  bus = new TripleBus(
    xmlPrivateCaches, xmlSharedCaches, projectXmlCfg.GetBusFIFOSize(), L1BusCnfg.GetReqBusLatcy(), L1BusCnfg.GetRespBusLatcy());

  // LLC <--> DRAM
  bus2 = new Bus(
    xmlSharedCaches, projectXmlCfg.GetDRAMId(), projectXmlCfg.GetBusFIFOSize(), bus->getLowerLevelIds());

  /* CPUs and L1s Setup*/
  for (list<CacheXml>::iterator it = xmlPrivateCaches.begin(); it != xmlPrivateCaches.end(); it++)
  {
    CacheXml PrivateCacheXml = *it;

    /*
     * instantiate cpu FIFOs
     */
    CpuFIFO *newCpuFIFO = new CpuFIFO(PrivateCacheXml.GetCacheId(), projectXmlCfg.GetCpuFIFOSize());
    m_cpuFIFO.push_back(newCpuFIFO);

    /*
     * instantiate cpu cores
     */
    Ptr<CpuCoreGenerator> newCpuCore = CreateObject<CpuCoreGenerator>(newCpuFIFO);
    stringstream bmTraceFile, cpuTraceFile, ctrlTraceFile;
    bmTraceFile << projectXmlCfg.GetBMsPath() << "/trace_C" << PrivateCacheXml.GetCacheId() << ".trc.shared";
    cpuTraceFile << projectXmlCfg.GetBMsPath() << "/" << projectXmlCfg.GetCpuTraceFile() << PrivateCacheXml.GetCacheId() << ".txt";
    ctrlTraceFile << projectXmlCfg.GetBMsPath() << "/" << projectXmlCfg.GetCohCtrlsTraceFile() << PrivateCacheXml.GetCacheId() << ".txt";
    double cpuClkPeriod = PrivateCacheXml.GetCpuClkNanoSec();
    double cpuClkSkew = cpuClkPeriod * PrivateCacheXml.GetCpuClkSkew() / 100.00;
    newCpuCore->SetCoreId(PrivateCacheXml.GetCacheId());
    newCpuCore->SetBmFileName(bmTraceFile.str());
    newCpuCore->SetCpuTraceFile(cpuTraceFile.str());
    newCpuCore->SetCtrlsTraceFile(ctrlTraceFile.str());
    newCpuCore->SetDt(cpuClkPeriod);
    newCpuCore->SetClkSkew(cpuClkSkew);
    newCpuCore->SetLogFileGenEnable(m_logFileGenEnable);
    newCpuCore->SetOutOfOrderStages(projectXmlCfg.GetOutOfOrderStages());
    m_cpuCoreGens.push_back(newCpuCore);

    bm_paths.push_back(bmTraceFile.str());

    // L1s Interface towards LLC Interconnect
    CommunicationInterface* bus_interface = bus->getInterfaceFor(PrivateCacheXml.GetCacheId());

    /*
     * instantiate cache controllers
     */
    CacheController *newCacheCtrl;
    if (m_cohrProt == CohProtType::SNOOP_MESI || m_cohrProt == CohProtType::SNOOP_MOESI)
      newCacheCtrl = new CacheControllerExclusive(PrivateCacheXml, m_fsm_protocol_path, bus_interface, newCpuFIFO,
                                                  projectXmlCfg.GetCache2Cache(), sharedMemIds, m_cohrProt);
    else
      newCacheCtrl = new CacheController(PrivateCacheXml, m_fsm_protocol_path, bus_interface, newCpuFIFO,
                                         projectXmlCfg.GetCache2Cache(), sharedMemIds, m_cohrProt);

    m_cpuCacheCtrl.push_back(newCacheCtrl);

    if (m_maxPendReq < PrivateCacheXml.GetNPendReq())
    {
      m_maxPendReq = PrivateCacheXml.GetNPendReq();
    }
  }

  /* LLC Setup */
  // iterate over each LLC
  // this variable assigns bank number for each LLC
  for (auto& it : xmlSharedCaches) {
    CommunicationInterface* LLC_bus_interface = bus->getInterfaceFor(it.GetCacheId());
    CommunicationInterface* LLC_DRAM_interface = bus2->getInterfaceFor(it.GetCacheId());
    
    m_SharedCacheCtrl.push_back(new CacheController_End2End(it, m_fsm_llc_protocol_path, LLC_DRAM_interface, LLC_bus_interface,
                                          projectXmlCfg.GetCache2Cache(), dramIds, m_llcCohrProt, bus->getLowerLevelIds()));
  }

  /* DRAM Setup */
  // DRAM Interface to LLC Bus
  CommunicationInterface* DRAM_LLC_interface = bus2->getInterfaceFor(projectXmlCfg.GetDRAMId());

  // DRAM Controller
  // m_main_memory = new MainMemoryController(projectXmlCfg, DRAM_LLC_interface, sharedMemIds);
  m_mcsim_interface = new MCsimInterface(projectXmlCfg, DRAM_LLC_interface, sharedMemIds);

  Logger::getLogger()->registerReportPath(projectXmlCfg.GetBMsPath() + string("/newLogger"));   

}

void MCoreSimProject::setup4(MCoreSimProjectXml projectXmlCfg)
{

  /* Setup 4:
  - Fixed Latency DRAM
  - Private L1 and L2
  - Shared (Bankized or non-Bankized) LLC*/

 // initialize Simulator components
  m_cpuCoreGens = list<Ptr<CpuCoreGenerator>>();
  m_cpuFIFO = list<CpuFIFO *>();
  m_cpuCacheCtrl = list<CacheController *>();
  m_l2CacheCtrl = list<CacheController *>();

  // Get all cpu configurations from xml
  list<CacheXml> xmlL1Caches = projectXmlCfg.GetL1Caches();
  list<CacheXml> xmlL2Caches = projectXmlCfg.GetL2Caches();
  list<CacheXml> xmlSharedCaches = projectXmlCfg.GetSharedCache();

  // extract cacheIds
  std::vector<int> sharedMemIds;
  for (auto& it : xmlSharedCaches) {
    sharedMemIds.push_back(it.GetCacheId());
  } 

  // convert int to list<int>
  vector<int> dramIds;
  dramIds.push_back(projectXmlCfg.GetDRAMId());

  // Get L2Bus configurations
  // it reads L1Bus configuration
  L1BusCnfgXml L2BusCnfg = projectXmlCfg.GetL1BusCnfg();

  char path_array[256];
  getcwd (path_array, sizeof(path_array));

  string path(path_array);
  m_fsm_protocol_path = path+string("/Protocols_FSM/");
  string l1_fsm = m_fsm_protocol_path + "MSI_L1.csv";
  string l2_fsm = m_fsm_protocol_path + "MSI_L2.csv";
  m_fsm_llc_protocol_path = m_fsm_protocol_path + "MSI_LLC.csv";
  cout <<"FSM path: " << m_fsm_protocol_path << endl;

  // Get Coherence protocol type
  // GetCohrProtocolType();

  m_maxPendReq = 0;

  /* Interconnects Setup*/
  // L2s <--> LLC
  l2_llc_bus = new TripleBus(
    xmlL2Caches, xmlSharedCaches, projectXmlCfg.GetBusFIFOSize(), L2BusCnfg.GetReqBusLatcy(), L2BusCnfg.GetRespBusLatcy());

  // LLC <--> DRAM
  llc_dram_bus = new Bus(
    xmlSharedCaches, projectXmlCfg.GetDRAMId(), projectXmlCfg.GetBusFIFOSize(), l2_llc_bus->getLowerLevelIds());

  /* LLC Setup */
  // iterate over each LLC
  for (auto& it : xmlSharedCaches) {
    CommunicationInterface* llc_l2_interface = l2_llc_bus->getInterfaceFor(it.GetCacheId());
    CommunicationInterface* llc_dram_interface = llc_dram_bus->getInterfaceFor(it.GetCacheId());
    
    CacheController_End2End* newLlcCacheCtrl = new CacheController_End2End(it, m_fsm_llc_protocol_path, llc_dram_interface, llc_l2_interface,
                                          projectXmlCfg.GetCache2Cache(), dramIds, CohProtType::SNOOP_LLC_MSI, l2_llc_bus->getLowerLevelIds());
    m_SharedCacheCtrl.push_back(newLlcCacheCtrl);
  }

  /* DRAM Setup */
  // DRAM Interface to LLC Bus
  CommunicationInterface* dram_llc_interface = llc_dram_bus->getInterfaceFor(projectXmlCfg.GetDRAMId());

  // DRAM Controller
  // m_main_memory = new MainMemoryController(projectXmlCfg, dram_llc_interface, sharedMemIds);
  m_mcsim_interface = new MCsimInterface(projectXmlCfg, dram_llc_interface, sharedMemIds);

  /* CPUs, L1s,  and L2s Setup*/
  int core_num = 0;
  for (list<CacheXml>::iterator L1 = xmlL1Caches.begin(), L2 = xmlL2Caches.begin(); L1 != xmlL1Caches.end() && core_num < projectXmlCfg.GetNumPrivCore(); L1++, L2++, core_num++)
  {    
    CacheXml L1CacheXml = *L1;
    CacheXml L2CacheXml = *L2;

    /*
     * instantiate cpu FIFOs
     */
    CpuFIFO *newCpuFIFO = new CpuFIFO(L1CacheXml.GetCacheId(), projectXmlCfg.GetCpuFIFOSize());
    m_cpuFIFO.push_back(newCpuFIFO);

    /*
     * instantiate cpu cores
     */
    Ptr<CpuCoreGenerator> newCpuCore = CreateObject<CpuCoreGenerator>(newCpuFIFO);
    stringstream bmTraceFile, cpuTraceFile, ctrlTraceFile;
    bmTraceFile << projectXmlCfg.GetBMsPath() << "/trace_C" << L1->GetCacheId() << ".trc.shared";
    cpuTraceFile << projectXmlCfg.GetBMsPath() << "/" << projectXmlCfg.GetCpuTraceFile() <<L1->GetCacheId() << ".txt";
    ctrlTraceFile << projectXmlCfg.GetBMsPath() << "/" << projectXmlCfg.GetCohCtrlsTraceFile() << L1->GetCacheId() << ".txt";
    double cpuClkPeriod = L1->GetCpuClkNanoSec();
    double cpuClkSkew = cpuClkPeriod * L1->GetCpuClkSkew() / 100.00;
    newCpuCore->SetCoreId(L1->GetCacheId());
    newCpuCore->SetBmFileName(bmTraceFile.str());
    newCpuCore->SetCpuTraceFile(cpuTraceFile.str());
    newCpuCore->SetCtrlsTraceFile(ctrlTraceFile.str());
    newCpuCore->SetDt(cpuClkPeriod);
    newCpuCore->SetClkSkew(cpuClkSkew);
    newCpuCore->SetLogFileGenEnable(m_logFileGenEnable);
    newCpuCore->SetOutOfOrderStages(projectXmlCfg.GetOutOfOrderStages());
    m_cpuCoreGens.push_back(newCpuCore);

    bm_paths.push_back(bmTraceFile.str());

    // L1 <--> L2
    DirectInterconnect* l1_l2_bus = new DirectInterconnect(L1CacheXml.GetCacheId(), L2CacheXml.GetCacheId(), projectXmlCfg.GetBusFIFOSize());
    l1_l2_direct_bus_list.push_back(l1_l2_bus);
    CommunicationInterface* l1_l2_interface = l1_l2_bus->getInterfaceFor(L1CacheXml.GetCacheId());
    CommunicationInterface* l2_l1_interface = l1_l2_bus->getInterfaceFor(L2CacheXml.GetCacheId());
    
    // L2 <--> LLC
    CommunicationInterface* l2_llc_interface = l2_llc_bus->getInterfaceFor(L2CacheXml.GetCacheId());

    /*
    * instantiate L2 cache controllers
    */
    vector<int> curL1Id;
    curL1Id.push_back(L1CacheXml.GetCacheId());
    CacheController *newL2CacheCtrl;
    newL2CacheCtrl = new CacheController(L2CacheXml, l2_fsm, l2_llc_interface, l2_l1_interface,
                                        projectXmlCfg.GetCache2Cache(), sharedMemIds, CohProtType::L2_MSI, 
                                        &curL1Id);
    if (m_maxPendReq < L2CacheXml.GetNPendReq())
    {
      m_maxPendReq = L2CacheXml.GetNPendReq();
    }

    /*
    * instantiate L1 cache controllers
    */
    vector<int> curL2Id;
    curL2Id.push_back(L2CacheXml.GetCacheId());
    CacheController *newL1CacheCtrl;
    newL1CacheCtrl = new CacheController(L1CacheXml, l1_fsm, l1_l2_interface, newCpuFIFO,
                                        projectXmlCfg.GetCache2Cache(), curL2Id, CohProtType::L1_MSI);
    if (m_maxPendReq < L1CacheXml.GetNPendReq())
    {
      m_maxPendReq = L1CacheXml.GetNPendReq();
    }

    // push L2 children controllers
    newL2CacheCtrl->m_children_controllers.push_back(newL1CacheCtrl);

    m_l2CacheCtrl.push_back(newL2CacheCtrl);
    m_cpuCacheCtrl.push_back(newL1CacheCtrl);

    // push LLC children controllers
    for (auto& it : m_SharedCacheCtrl) {  
      it->m_children_controllers.push_back(newL2CacheCtrl);
    }
  }

  Logger::getLogger()->registerReportPath(projectXmlCfg.GetBMsPath() + string("/newLogger"));   
}

void MCoreSimProject::setup5(MCoreSimProjectXml projectXmlCfg)
{
  /* Setup 5:
  - Cluster
  - Fixed Latency DRAM
  - Private L1
  - Shared L2 within Cluster
  - Shared (Bankized or non-Bankized) LLC*/

  // initialize Simulator components
  m_cpuCoreGens = list<Ptr<CpuCoreGenerator>>();
  m_cpuFIFO = list<CpuFIFO *>();
  m_cpuCacheCtrl = list<CacheController *>();
  m_l2CacheCtrl = list<CacheController *>();

  // Get all cpu configurations from xml
  list<CacheXml> xmlL1Caches = projectXmlCfg.GetL1Caches();
  list<CacheXml> xmlL2Caches = projectXmlCfg.GetL2Caches();
  list<CacheXml> xmlSharedCaches = projectXmlCfg.GetSharedCache();

  int numClusters = projectXmlCfg.GetNumCluster();

  // extract cacheIds
  vector<int> sharedMemIds;
  for (auto& it : xmlSharedCaches) {
    sharedMemIds.push_back(it.GetCacheId());
  } 

  // convert int to list<int>
  vector<int> dramIds;
  dramIds.push_back(projectXmlCfg.GetDRAMId());

  // Get bus configurations
  // both read L1Bus configuration
  L1BusCnfgXml L1BusCnfg = projectXmlCfg.GetL1BusCnfg();
  L1BusCnfgXml L2BusCnfg = projectXmlCfg.GetL1BusCnfg();

  char path_array[256];
  getcwd (path_array, sizeof(path_array));

  string path(path_array);
  m_fsm_protocol_path = path+string("/Protocols_FSM/");
  m_fsm_llc_protocol_path = path+string("/Protocols_FSM/");
  cout <<"FSM path: " << m_fsm_protocol_path << endl;

  // Get Coherence protocol type
  GetCohrProtocolType();

  m_maxPendReq = 0;

  /* Interconnects Setup*/
  // L2s <--> LLC
  l2_llc_bus = new TripleBus(
    xmlL2Caches, xmlSharedCaches, projectXmlCfg.GetBusFIFOSize(), L2BusCnfg.GetReqBusLatcy(), L2BusCnfg.GetRespBusLatcy());

  // LLC <--> DRAM
  llc_dram_bus = new Bus(
    xmlSharedCaches, projectXmlCfg.GetDRAMId(), projectXmlCfg.GetBusFIFOSize(), l2_llc_bus->getLowerLevelIds());

  /* LLC Setup */
  // iterate over each LLC
  for (auto& it : xmlSharedCaches) {
    CommunicationInterface* llc_l2_interface = l2_llc_bus->getInterfaceFor(it.GetCacheId());
    CommunicationInterface* llc_dram_interface = llc_dram_bus->getInterfaceFor(it.GetCacheId());
    
    CacheController_End2End* newLlcCacheCtrl = new CacheController_End2End(it, m_fsm_llc_protocol_path, llc_dram_interface, llc_l2_interface,
                                          projectXmlCfg.GetCache2Cache(), dramIds, m_llcCohrProt, l2_llc_bus->getLowerLevelIds());
    m_SharedCacheCtrl.push_back(newLlcCacheCtrl);
  }

  /* DRAM Setup */
  // DRAM Interface to LLC Bus
  CommunicationInterface* dram_llc_interface = llc_dram_bus->getInterfaceFor(projectXmlCfg.GetDRAMId());

  // DRAM Controller
  m_main_memory = new MainMemoryController(projectXmlCfg, dram_llc_interface, sharedMemIds);

  int l1_itr = 0;
  for (list<CacheXml>::iterator L2 = xmlL2Caches.begin(); L2 != xmlL2Caches.end(); L2++)
  {
    CacheXml L2CacheXml = *L2;

    list<CacheXml> L2CacheXml_list;
    L2CacheXml_list.push_back(L2CacheXml);

    list<CacheController *> currenL1CacheCtrls_list;

    list<CacheXml> currentL1CacheXml_list;
    vector<int> currentL1Ids_list;
    list<CacheXml>::iterator it = xmlL1Caches.begin();
    for (int l1_per_l2_itr = 0; l1_per_l2_itr < (int)(xmlL1Caches.size()/xmlL2Caches.size()); l1_per_l2_itr++, l1_itr++)
    {
      std::advance(it, l1_itr);
      currentL1CacheXml_list.push_back(*it);
      currentL1Ids_list.push_back(it->GetCacheId());
    }

    l1_l2_bus = new TripleBus(
      currentL1CacheXml_list, L2CacheXml_list, projectXmlCfg.GetBusFIFOSize(), L1BusCnfg.GetReqBusLatcy(), L1BusCnfg.GetRespBusLatcy());
    l1_l2_bus_list.push_back(l1_l2_bus);
    
    for (list<CacheXml>::iterator L1 = currentL1CacheXml_list.begin(); L1 != currentL1CacheXml_list.end(); L1++)
    {
      CacheXml L1CacheXml = *L1;

      /*
      *  instantiate cpu FIFOs
      */
      CpuFIFO *newCpuFIFO = new CpuFIFO(L1CacheXml.GetCacheId(), projectXmlCfg.GetCpuFIFOSize());
      m_cpuFIFO.push_back(newCpuFIFO);

      /*
       * instantiate cpu cores
       */
      Ptr<CpuCoreGenerator> newCpuCore = CreateObject<CpuCoreGenerator>(newCpuFIFO);
      stringstream bmTraceFile, cpuTraceFile, ctrlTraceFile;
      bmTraceFile << projectXmlCfg.GetBMsPath() << "/trace_C" << L1->GetCacheId() << ".trc.shared";
      cpuTraceFile << projectXmlCfg.GetBMsPath() << "/" << projectXmlCfg.GetCpuTraceFile() <<L1->GetCacheId() << ".txt";
      ctrlTraceFile << projectXmlCfg.GetBMsPath() << "/" << projectXmlCfg.GetCohCtrlsTraceFile() << L1->GetCacheId() << ".txt";
      double cpuClkPeriod = L1->GetCpuClkNanoSec();
      double cpuClkSkew = cpuClkPeriod * L1->GetCpuClkSkew() / 100.00;
      newCpuCore->SetCoreId(L1->GetCacheId());
      newCpuCore->SetBmFileName(bmTraceFile.str());
      newCpuCore->SetCpuTraceFile(cpuTraceFile.str());
      newCpuCore->SetCtrlsTraceFile(ctrlTraceFile.str());
      newCpuCore->SetDt(cpuClkPeriod);
      newCpuCore->SetClkSkew(cpuClkSkew);
      newCpuCore->SetLogFileGenEnable(m_logFileGenEnable);
      newCpuCore->SetOutOfOrderStages(projectXmlCfg.GetOutOfOrderStages());
      m_cpuCoreGens.push_back(newCpuCore);

      bm_paths.push_back(bmTraceFile.str());

      // L1 <--> L2
      CommunicationInterface* l1_l2_interface = l1_l2_bus->getInterfaceFor(L1CacheXml.GetCacheId());

      /*
       * instantiate L1 cache controllers
       */
      vector<int> curL2Id;
      curL2Id.push_back(L2CacheXml.GetCacheId());
      CacheController *newL1CacheCtrl;
      newL1CacheCtrl = new CacheController(L1CacheXml, m_fsm_protocol_path, l1_l2_interface, newCpuFIFO,
                                          projectXmlCfg.GetCache2Cache(), curL2Id, m_cohrProt);
      if (m_maxPendReq < L1CacheXml.GetNPendReq())
      {
        m_maxPendReq = L1CacheXml.GetNPendReq();
      }
      m_cpuCacheCtrl.push_back(newL1CacheCtrl);
      currenL1CacheCtrls_list.push_back(newL1CacheCtrl);
    }

    // L2 <--> LLC
    CommunicationInterface* l2_llc_interface = l2_llc_bus->getInterfaceFor(L2CacheXml.GetCacheId());

    // L2 <--> L1
    CommunicationInterface* l2_l1_interface = l1_l2_bus->getInterfaceFor(L2CacheXml.GetCacheId());

    /*
     * instantiate L2 cache controllers
     */
    CacheController *newL2CacheCtrl;
    newL2CacheCtrl = new CacheController(L2CacheXml, m_fsm_llc_protocol_path, l2_llc_interface, l2_l1_interface,
                                        projectXmlCfg.GetCache2Cache(), sharedMemIds, m_llcCohrProt, 
                                        &currentL1Ids_list);
    if (m_maxPendReq < L2CacheXml.GetNPendReq())
    {
      m_maxPendReq = L2CacheXml.GetNPendReq();
    }
    
    // push L2 children controllers
    for (auto& it : currenL1CacheCtrls_list) {
      newL2CacheCtrl->m_children_controllers.push_back(it);
    }

    m_l2CacheCtrl.push_back(newL2CacheCtrl);\

    // push LLC children controllers
    for (auto& it : m_SharedCacheCtrl) {  
      it->m_children_controllers.push_back(newL2CacheCtrl);
    }
  }

  Logger::getLogger()->registerReportPath(projectXmlCfg.GetBMsPath() + string("/newLogger")); 
}

void MCoreSimProject::setup6(MCoreSimProjectXml projectXmlCfg)
{
  /* Setup 6:
  - Tiled
  - Fixed Latency DRAM
  - Private L1
  - Shared L2 within Tile
  - Shared Bankized LLC*/

  // initialize Simulator components
  m_cpuCoreGens = list<Ptr<CpuCoreGenerator>>();
  m_cpuFIFO = list<CpuFIFO *>();
  m_cpuCacheCtrl = list<CacheController *>();
  m_l2CacheCtrl = list<CacheController *>();

  // Get all cpu configurations from xml
  list<CacheXml> xmlL1Caches = projectXmlCfg.GetL1Caches();
  list<CacheXml> xmlL2Caches = projectXmlCfg.GetL2Caches();
  list<CacheXml> xmlSharedCaches = projectXmlCfg.GetSharedCache();

  int numTiles = projectXmlCfg.GetNumTiles();

  // extract cacheIds
  vector<int> sharedMemIds;
  for (auto& it : xmlSharedCaches) {
    sharedMemIds.push_back(it.GetCacheId());
  } 

  // convert int to list<int>
  vector<int> dramIds;
  dramIds.push_back(projectXmlCfg.GetDRAMId());

  // Get bus configurations
  // both read L1Bus configuration
  L1BusCnfgXml L1BusCnfg = projectXmlCfg.GetL1BusCnfg();
  L1BusCnfgXml L2BusCnfg = projectXmlCfg.GetL1BusCnfg();

  char path_array[256];
  getcwd (path_array, sizeof(path_array));

  string path(path_array);
  m_fsm_protocol_path = path+string("/Protocols_FSM/");
  m_fsm_llc_protocol_path = path+string("/Protocols_FSM/");
  cout <<"FSM path: " << m_fsm_protocol_path << endl;

  // Get Coherence protocol type
  GetCohrProtocolType();

  m_maxPendReq = 0;

  // LLC <--> DRAM
  llc_dram_bus = new Bus(
    xmlSharedCaches, projectXmlCfg.GetDRAMId(), projectXmlCfg.GetBusFIFOSize());

  // L2s <--> L2s
  l2_l2_bus = new TripleBus(
    xmlL2Caches, xmlL2Caches, projectXmlCfg.GetBusFIFOSize(), L2BusCnfg.GetReqBusLatcy(), L2BusCnfg.GetRespBusLatcy());

  /* DRAM Setup */
  // DRAM Interface to LLC Bus
  CommunicationInterface* dram_llc_interface = llc_dram_bus->getInterfaceFor(projectXmlCfg.GetDRAMId());

  // DRAM Controller
  m_main_memory = new MainMemoryController(projectXmlCfg, dram_llc_interface, sharedMemIds);

  int l1_itr = 0;
  for (list<CacheXml>::iterator L2 = xmlL2Caches.begin(), LLC_BNK = xmlSharedCaches.begin(); L2 != xmlL2Caches.end(), LLC_BNK != xmlSharedCaches.end(); L2++, LLC_BNK++)
  {
    CacheXml L2CacheXml = *L2;
    CacheXml LlcCacheXml = *LLC_BNK;

    // LLC <--> DRAM
    CommunicationInterface* llc_dram_interface = llc_dram_bus->getInterfaceFor(LlcCacheXml.GetCacheId());

    list<CacheXml> L2CacheXml_list;
    L2CacheXml_list.push_back(L2CacheXml);

    list<CacheController *> currenL1CacheCtrls_list;

    list<CacheXml> currentL1CacheXml_list;
    vector<int> currentL1Ids_list;
    list<CacheXml>::iterator it = xmlL1Caches.begin();
    for (int l1_per_l2_itr = 0; l1_per_l2_itr < (int)(xmlL1Caches.size()/xmlL2Caches.size()); l1_per_l2_itr++, l1_itr++)
    {
      std::advance(it, l1_itr);
      currentL1CacheXml_list.push_back(*it);
      currentL1Ids_list.push_back(it->GetCacheId());
    }

    l1_l2_bus = new TripleBus(
      currentL1CacheXml_list, L2CacheXml_list, projectXmlCfg.GetBusFIFOSize(), L1BusCnfg.GetReqBusLatcy(), L1BusCnfg.GetRespBusLatcy());
    l1_l2_bus_list.push_back(l1_l2_bus);
    
    for (list<CacheXml>::iterator L1 = currentL1CacheXml_list.begin(); L1 != currentL1CacheXml_list.end(); L1++)
    {
      CacheXml L1CacheXml = *L1;

      /*
      *  instantiate cpu FIFOs
      */
      CpuFIFO *newCpuFIFO = new CpuFIFO(L1CacheXml.GetCacheId(), projectXmlCfg.GetCpuFIFOSize());
      m_cpuFIFO.push_back(newCpuFIFO);

      /*
       * instantiate cpu cores
       */
      Ptr<CpuCoreGenerator> newCpuCore = CreateObject<CpuCoreGenerator>(newCpuFIFO);
      stringstream bmTraceFile, cpuTraceFile, ctrlTraceFile;
      bmTraceFile << projectXmlCfg.GetBMsPath() << "/trace_C" << L1->GetCacheId() << ".trc.shared";
      cpuTraceFile << projectXmlCfg.GetBMsPath() << "/" << projectXmlCfg.GetCpuTraceFile() <<L1->GetCacheId() << ".txt";
      ctrlTraceFile << projectXmlCfg.GetBMsPath() << "/" << projectXmlCfg.GetCohCtrlsTraceFile() << L1->GetCacheId() << ".txt";
      double cpuClkPeriod = L1->GetCpuClkNanoSec();
      double cpuClkSkew = cpuClkPeriod * L1->GetCpuClkSkew() / 100.00;
      newCpuCore->SetCoreId(L1->GetCacheId());
      newCpuCore->SetBmFileName(bmTraceFile.str());
      newCpuCore->SetCpuTraceFile(cpuTraceFile.str());
      newCpuCore->SetCtrlsTraceFile(ctrlTraceFile.str());
      newCpuCore->SetDt(cpuClkPeriod);
      newCpuCore->SetClkSkew(cpuClkSkew);
      newCpuCore->SetLogFileGenEnable(m_logFileGenEnable);
      newCpuCore->SetOutOfOrderStages(projectXmlCfg.GetOutOfOrderStages());
      m_cpuCoreGens.push_back(newCpuCore);

      bm_paths.push_back(bmTraceFile.str());

      // L1 <--> L2
      CommunicationInterface* l1_l2_interface = l1_l2_bus->getInterfaceFor(L1CacheXml.GetCacheId());

      /*
       * instantiate L1 cache controllers
       */
      vector<int> curL2Id;
      curL2Id.push_back(L2CacheXml.GetCacheId());
      CacheController *newL1CacheCtrl;
      newL1CacheCtrl = new CacheController(L1CacheXml, m_fsm_protocol_path, l1_l2_interface, newCpuFIFO,
                                          projectXmlCfg.GetCache2Cache(), curL2Id, m_cohrProt);
      if (m_maxPendReq < L1CacheXml.GetNPendReq())
      {
        m_maxPendReq = L1CacheXml.GetNPendReq();
      }
      m_cpuCacheCtrl.push_back(newL1CacheCtrl);
      currenL1CacheCtrls_list.push_back(newL1CacheCtrl);
    }

    // L2s <--> L2s
    CommunicationInterface* l2_l2_interface = l2_l2_bus->getInterfaceFor(L2CacheXml.GetCacheId());

    // L2 <--> L1
    CommunicationInterface* l2_l1_interface = l1_l2_bus->getInterfaceFor(L2CacheXml.GetCacheId());

    // L2 <--> LLC
    DirectInterconnect* l2_llc_bus = new DirectInterconnect(L2CacheXml.GetCacheId(), LlcCacheXml.GetCacheId(), projectXmlCfg.GetBusFIFOSize());
    l2_llc_direct_bus_list.push_back(l2_llc_bus);
    CommunicationInterface* l2_llc_interface = l2_llc_bus->getInterfaceFor(L2CacheXml.GetCacheId());
    CommunicationInterface* llc_l2_interface = l2_llc_bus->getInterfaceFor(LlcCacheXml.GetCacheId());

    /*
     * instantiate L2 cache controllers
     */
    CacheController* newL2CacheCtrl = new CacheController(L2CacheXml, m_fsm_llc_protocol_path, l2_llc_interface, l2_l1_interface,
                                      projectXmlCfg.GetCache2Cache(), sharedMemIds, m_llcCohrProt, 
                                      &currentL1Ids_list);
    if (m_maxPendReq < L2CacheXml.GetNPendReq())
    {
      m_maxPendReq = L2CacheXml.GetNPendReq();
    }

    // push L2 children controllers
    for (auto& it : currenL1CacheCtrls_list) {
      newL2CacheCtrl->m_children_controllers.push_back(it);
    }

    m_l2CacheCtrl.push_back(newL2CacheCtrl);

    /*
     * instantiate LLC cache controllers
     */
    vector<int> currentL2Id_list;
    currentL2Id_list.push_back(L2CacheXml.GetCacheId());
    CacheController_End2End* newLlcCacheCtrl = new CacheController_End2End(LlcCacheXml, m_fsm_llc_protocol_path, llc_dram_interface, llc_l2_interface,
                                          projectXmlCfg.GetCache2Cache(), dramIds, CohProtType::SNOOP_LLC_MSI, &currentL2Id_list);

    // push LLC children controllers
    newLlcCacheCtrl->m_children_controllers.push_back(newL2CacheCtrl);

    m_SharedCacheCtrl.push_back(newLlcCacheCtrl);
  }

  Logger::getLogger()->registerReportPath(projectXmlCfg.GetBMsPath() + string("/newLogger")); 
}


/*
 * start simulation engines
 */
void MCoreSimProject::Start()
{

  for (list<Ptr<CpuCoreGenerator>>::iterator it = m_cpuCoreGens.begin(); it != m_cpuCoreGens.end(); it++)
  {
    (*it)->init();
  }


  for (list<ExternalCPU *>::iterator it = m_ext_cpu.begin(); it != m_ext_cpu.end(); it++)
  {
    (*it)->init();
  }

  for (list<CacheController *>::iterator it = m_cpuCacheCtrl.begin(); it != m_cpuCacheCtrl.end(); it++)
  {
    (*it)->init();
  }

  for (list<DirectInterconnect *>::iterator it = l1_l2_direct_bus_list.begin(); it != l1_l2_direct_bus_list.end(); it++)
  {
    (*it)->init();
  }

  for (list<DirectInterconnect *>::iterator it = l2_llc_direct_bus_list.begin(); it != l2_llc_direct_bus_list.end(); it++)
  {
    (*it)->init();
  }

  for (list<Bus *>::iterator it = l1_l2_bus_list.begin(); it != l1_l2_bus_list.end(); it++)
  {
    (*it)->init();
  }

  for (list<CacheController *>::iterator it = m_l2CacheCtrl.begin(); it != m_l2CacheCtrl.end(); it++)
  {
    (*it)->init();
  }

  for (auto it : m_SharedCacheCtrl) {
    it->init();
  }
  // m_SharedCacheCtrl->init();
  // m_SharedCacheCtrl->initializeCacheData(bm_paths);

  // m_dramCtrl->init();
  // m_main_memory->init();
  m_mcsim_interface->init();

  // m_busArbiter->init();
  
  // setup2
  if (bus != nullptr) {
    bus->init();
  }
  if (bus != nullptr) {
    bus2->init();
  }

  // setup4
  if (l2_llc_bus != nullptr) {
    l2_llc_bus->init();
  }
  if (llc_dram_bus != nullptr) {
    llc_dram_bus->init();
  }

  // setup5
  if (l2_l2_bus != nullptr) {
    l2_l2_bus->init();
  }

  Simulator::Schedule(Seconds(0.0), &Step, this);
  Simulator::Stop(MilliSeconds(m_totalTimeInSeconds));
}

void MCoreSimProject::Step(MCoreSimProject *project)
{
  project->CycleProcess();
}

void MCoreSimProject::CycleProcess()
{
  bool SimulationDoneFlag = true;

  for (list<Ptr<CpuCoreGenerator>>::iterator it = m_cpuCoreGens.begin(); it != m_cpuCoreGens.end(); it++)
  {
    SimulationDoneFlag &= (*it)->GetCpuSimDoneFlag();
  }

  if (SimulationDoneFlag == true && m_cpuCoreGens.size() > 0)
  {
    cout << "Current Simulation Done at Bus Clock Cycle # " << m_busCycle << endl;
    cerr << "End\n";
    // cout << "L2 Nmiss =  " << m_SharedCacheCtrl->GetShareCacheMisses() << endl;
    // cout << "L2 NReq =  " << m_SharedCacheCtrl->GetShareCacheNReqs() << endl;
    // cout << "L2 Miss Rate =  " << (m_SharedCacheCtrl->GetShareCacheMisses() / (float)m_SharedCacheCtrl->GetShareCacheNReqs()) * 100 << endl;
    exit(0);
  }

  // Schedule the next run
  Simulator::Schedule(NanoSeconds(m_dt), &MCoreSimProject::Step, this);
  m_busCycle++;
}

void MCoreSimProject::EnableDebugFlag(bool Enable)
{

  // for (list<Ptr<CacheController>>::iterator it = m_cpuCacheCtrl.begin(); it != m_cpuCacheCtrl.end(); it++)
  // {
  //   (*it)->SetLogFileGenEnable(Enable);
  // }

  // m_SharedCacheCtrl->SetLogFileGenEnable(Enable);
  // m_busArbiter->SetLogFileGenEnable(Enable);
}

void MCoreSimProject::GetCohrProtocolType()
{
  string cohType = m_projectXmlCfg.GetCohrProtType();
  if (cohType == "MSI")
  {
    m_cohrProt = CohProtType::SNOOP_MSI;
    m_llcCohrProt = CohProtType::SNOOP_LLC_MSI;
    m_fsm_protocol_path += "MSI_splitBus_snooping.csv";
    m_fsm_llc_protocol_path += "MSI_LLC.csv";
  }
  else if (cohType == "MESI")
  {
    m_cohrProt = CohProtType::SNOOP_MESI;
    m_llcCohrProt = CohProtType::SNOOP_LLC_MESI;
    m_fsm_protocol_path += "MESI_splitBus_snooping.csv";
    m_fsm_llc_protocol_path += "MESI_LLC.csv";
  }
  else if (cohType == "MOESI")
  {
    m_cohrProt = CohProtType::SNOOP_MOESI;
    m_llcCohrProt = CohProtType::SNOOP_LLC_MOESI;
    m_fsm_protocol_path += "MOESI_splitBus_snooping.csv";
    m_fsm_llc_protocol_path += "MOESI_LLC.csv";
  }
  else if (cohType == "PMSI")
  {
    m_cohrProt = CohProtType::SNOOP_PMSI;
    m_llcCohrProt = CohProtType::SNOOP_LLC_PMSI;
    m_fsm_protocol_path += "PMSI.csv";
    m_fsm_llc_protocol_path += "PMSI_LLC.csv";
  }
  else if (cohType == "PMESI")
  {
    m_cohrProt = CohProtType::SNOOP_PMESI;
    m_llcCohrProt = CohProtType::SNOOP_LLC_PMESI;
    m_fsm_protocol_path += "PMESI.csv";
    m_fsm_llc_protocol_path += "PMESI_LLC.csv";
  }
  else if (cohType == "PMSI_Asterisk")
  {
    m_cohrProt = CohProtType::SNOOP_PMSI_ASTERISK;
    m_llcCohrProt = CohProtType::SNOOP_LLC_PMSI_ASTERISK;
    m_fsm_protocol_path += "PMSI_asterisk.csv";
    m_fsm_llc_protocol_path += "PMSI_asterisk_LLC.csv";
  }
  else if (cohType == "PMESI_Asterisk")
  {
    m_cohrProt = CohProtType::SNOOP_PMESI_ASTERISK;
    m_llcCohrProt = CohProtType::SNOOP_LLC_PMESI_ASTERISK;
    m_fsm_protocol_path += "PMESI_asterisk.csv";
    m_fsm_llc_protocol_path += "PMESI_asterisk_LLC.csv";
  }
  else
  {
    std::cout << "Unsupported Coherence Protocol Cnfg Param = " << cohType << std::endl;
    exit(0);
  }
}
