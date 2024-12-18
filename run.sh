#!/bin/bash

if [ "$3" == "-gdb" ]; then
    ./waf --run scratch/MultiCoreSimulator \
          --command-template="gdb --args %s --CfgFile=$1 --BMsPath=$2 --LogFileGenEnable=1"
else
    ./waf --run "scratch/MultiCoreSimulator --CfgFile=$1 --BMsPath=$2 --LogFileGenEnable=1"
fi

#./waf configure
#./waf --run "scratch/MultiCoreSimulator --CfgFile=$1 --BMsPath=$2 --LogFileGenEnable=1"
#./waf --run scratch/MultiCoreSimulator --command-template="gdb --args %s --CfgFile=$1 --BMsPath=$2 --LogFileGenEnable=1"
#./waf --run "scratch/MultiCoreSimulator --CfgFile=tools/Assignment3/damov.xml --BMsPath=tools/Assignment2/cache_test_16KB_STRIDE4_ITER5 --LogFileGenEnable=0"
#./waf --run scratch/MultiCoreSimulator --command-template="gdb --args %s --CfgFile=tools/Assignment3/damov.xml --BMsPath=tools/Assignment2/cache_test_16KB_STRIDE4_ITER5 --LogFileGenEnable=1"
#./waf --run scratch/MultiCoreSimulator --command-template="gdb --args %s --CfgFile=$1 --BMsPath=$2 --LogFileGenEnable=1"
#./waf --run scratch/MultiCoreSimulator --command-template="gdb --args %s --CfgFile=tools/Assignment2/example_test.xml --BMsPath=tools/Assignment2/cache_test_16KB_STRIDE4_ITER5 --LogFileGenEnable=1"
