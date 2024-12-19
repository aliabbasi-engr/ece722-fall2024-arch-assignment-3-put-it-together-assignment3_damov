# 🌟🎄 **Christmas Quote** 🎅✨
> _"Peace on earth will come to stay when we live Christmas every day."_  
— **Helen Steiner Rice**  


_"...But peace in my mind will come only after I finish this assignment."_ 🎅  
— **Ali Abbasi** 


# ECE722, Assignment 3: Putting It Together!

**Course:** Advanced Computer Architecture (ECE722), Fall 2024  
**Instructor:** Dr. M Hassan  
**Institution:** McMaster University, ON, Canada

**Student:** Ali Abbasi  
**MacID:** abbasa46  
**Date Submitted:** 2024-12-22

## Repository Initialization

Since this assignment uses MCsim DRAM simulator along with the Octupus, MCsim is defined as a submodule of this repository. Please follow the step below to clone sources:

```
git clone git@github.com:aliabbasi-engr/ece722-fall2024-arch-assignment-3-put-it-together-assignment3_damov.git
cd ece722-fall2024-arch-assignment-3-put-it-together-assignment3_damov/
git checkout MCsim
git submodule init
git submodule update
```

P.S. This repository has two branches: The "main" branch holds the Octopus version integrating assignment 1 and assignment 2. The "MCsim" branch is the same as the main branch where the fixed-latency DRAM is replaced with MCsim.

## New Directories and Files

CSV Report:
* ./tools/Assignment3/ece722_abbasa46_assignment3.csv

XML Configurations:
* ./tools/Assignment3/cfgs/

Simulation log files:
* ./tools/Assignment3/logs/

Scripts:
* ./tools/Assignment3/scripts/calculate_metrics.py
  * Used to calculate architecture independant metrics for an specific memory trace, arithmetic intensity, spatial locality, and temporal locality.
  * Usage: ./caclulate_matrics.py \<trace_file>

* ./tools/Assignment3/scripts/req_counter.sh
  * Used to count the number of requests in a memory trace located within a directory.
  * Usage: ./req_counter.sh \<directory>

* ./tools/Assignment3/scripts/copy_trace.sh
  * Used to copy all .trc files within the source directory into the target directory with the Octupus-supported hirerarchy.
  * It reduces the number of compute instructions before the first request of each trace to 0. Additionally, it caps the number of requests in each trace to "max_num_requests".
  * Usage: ./copy_traces.sh \<max_num_requests> \<source_directory> \<destination_directory>"

* ./tools/Assignment3/scripts/run_and_log.sh
  * Used to run simulation on an specific memory trace with the defined config.
  * This script also writes the output of the simulator to a separate file in the "./tools/Assignment3/logs/" directory with the name "[trace_dir_name].log" to be used by parser.
  * Usage: "./run_and_log.sh \<cfg_file> \<trace_dir>
  
* ./tools/Assignment3/scripts/parser.sh
  * Used to parse the "[trace_dir_name].log" file generated by the run_and_log.sh script and generate the "[trace_dir_name]_summary.log" file in the same directory with the cache-specific metrics.
  * Usage: ./parser.sh \<log_file>

## Applications and Memory Traces

The following three applications are generated for this assignment and are uploaded to the provided OneDrive folder:
* OpenMP backprop layerforward generated as follows:
  * ./pin-3.13-98189-g60a6ef199-gcc-linux/pin -t obj-intel64/trace_generator.so -manual 1 -thread 1 -max 100000000 -- workloads/rodinia_3.1/openmp/backprop/backprop_layerforward 10000000 1

* OpenMP backprop adjust weights generated as follows:
  * ./pin-3.13-98189-g60a6ef199-gcc-linux/pin -t obj-intel64/trace_generator.so -manual 1 -thread 1 -max 100000000 -- workloads/rodinia_3.1/openmp/backprop/backprop_adjust_weights 50000000 1

* OpenMP srad_v1 generated as follows:
  * ./pin-3.13-98189-g60a6ef199-gcc-linux/pin -t obj-intel64/trace_generator.so -manual 1 -thread 1 -max 100000000 -- workloads/rodinia_3.1/openmp/srad/srad_v1/srad 100 0.5 502 458 1

# CacheSim
CacheSim is a trace-base cycle-accurate simulator for cache-coherent interconnect architectures. CacheSim enables the user to build reconfigurable simulation infrastructure for multi-core processor chip with a high degree of flexibility of controlling system's configuration parameters such as cache organization, coherence protocol models, and interconnect bus architecture as well as bus arbitration policies. CacheSim is implemented in C++ using object-oriented programming concepts to support a modular, expansible, configurable, and integrable design.

# Getting started
* The simulator is tested on both Linux Ubuntu 18.04.4 LTS and Ubuntu 20.04.01 releases. You may consider using Virtual Machine VM to install Ubuntu on your machine if it is not your primary operating system.  
* `$CacheSim` refers to the top level directory where CacheSim resides.
* `CacheSim/scratch/MultiCoreSimulator.cc` is the main function of the simulator.  
* Directory `$CacheSim/src/MultiCoreSim` contains source code and test cases for building and running CacheSim.
* CacheSim accepts test-case configuration parameters in XML format, `$CacheSim/src/MultiCoreSim/test` directory contains examples of various experiments that can be used to run the simulator.

## Building CacheSim
CacheSim uses [WAF building automation tool](https://waf.io/) to configure, build, and run simulator. In order to build CacheSim, you need to install the following packages.

```shell
sudo apt update
sudo apt upgrade
sudo apt-get install build-essential autoconf automake libxmu-dev python
```
you also need to install Lcov tool in order to run the simulator in the code coverage mode,

```shel
sudo apt-get install lcov
```

Waf tool allows the user to configure and run CacheSim in three different operation modes as follows.

* **Experimental mode** is the fastest simulation mode where all debug messages and coverage report features are disabled. This mode is used during architectural explorations once the developed feature is stable, and the main focus is on evaluating developed algorithms' performance. The following command is used to run the simulator in experiemental mode.

```shell
cd $CacheSim
./waf --run "scratch/MultiCoreSimulator --CfgFile=./src/MultiCoreSim/test/piscot/piscot_tc17.xml --BMsPath=../../../BMs/eembc-traces/iirflt01-trace"
```
The command line option `--CfgFile` used to configure the testcase file name, and `--BMsPath` use to configure the path of the memory request trace files. The trace based simulation reads from a file `trace_C<n>.trc.shared` that consists of lines of requests of the form `Addr OP time`. OP is of type RD for read and WR for write, and time is a positive integer value that denotes the arrival time of the memory request to the memory hierarchy. The parameter `n` indicates the processing core index that use to process this trace file.

* **Debug mode** allows the user to dump the internal states of the simulator every clock cycle. For instance, dump the internal states of cache coherence protocols and handshaking signals between processor controllers and upper-level controllers (i.e. shared memory controllers). To enable the debug mode, you need to enable `--LogFileGenEnable=1` flag as 

```shell
./waf --run "scratch/MultiCoreSimulator --CfgFile=./src/MultiCoreSim/test/piscot/piscot_tc17.xml --BMsPath=../../../BMs/eembc-traces/iirflt01-trace --LogFileGenEnable=1"
```

For advanced debugging, the following command can be used to attach `gdb` into CacheSim to allow the user see what is going on inside the C++ code, and run the program line by line in addition to set breakpoints.

```shell
./waf --run scratch/MultiCoreSimulator --command-template="gdb --args %s --CfgFile=./src/MultiCoreSim/test/piscot/piscot_tc19.xml --BMsPath=../../../BMs/synth/L2Replc6"
 ```

* **Coverage mode** provides information on the execution of the code line by line in terms of line and function coverage. To run the simulator with code coverage. To run code coverage, you need to add this option `--enable-gcov` to the waf command

```shell
./waf --enable-gcov --run "scratch/MultiCoreSimulator --CfgFile=./src/MultiCoreSim/test/piscot/piscot_tc17.xml --BMsPath=../../../BMs/eembc-traces/iirflt01-trace --LogFileGenEnable=1"
```
Then, run the following commands to generate `html` coverage analysis reports.
```shell
lcov --capture --directory . --output-file coverage.rpt
genhtml coverage.rpt --output-directory covOut
```

