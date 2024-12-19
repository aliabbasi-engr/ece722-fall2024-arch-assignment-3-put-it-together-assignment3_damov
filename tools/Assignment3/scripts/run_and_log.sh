#!/bin/bash

set -e

if [ "$#" -ne 2 ] && [ "$#" -ne 3 ]; then
  echo "Usage: $0 <cfg_file> <trace_dir> [-gdb]>"
  exit 1
fi

if [ ! -f "$1" ]; then
  echo "Error: $1 is not a valid file"
  exit 1
fi

if [ ! -d "$2" ]; then
  echo "Error: $2 is not a valid directory"
  exit 1
fi

cd ../../../

CFG_PATH="tools/Assignment3/scripts/$1"
BM_PATH="tools/Assignment3/scripts/$2"
APP_NAME=$(basename "$BM_PATH")
LOG_FILE_NAME="$APP_NAME.log"
LOG_PATH="tools/Assignment3/logs/$LOG_FILE_NAME"

# echo "CFG_PATH = $CFG_PATH"
# echo "BM_PATH = $BM_PATH"
# echo "APP_NAME = $APP_NAME"
# echo "LOG_FILE_NAME = $LOG_FILE_NAME"
# echo "LOG_PATH = $LOG_PATH"

# SECONDS=0

if [ "$3" == "-gdb" ]; then
    ./waf --run scratch/MultiCoreSimulator \
          --command-template="gdb --args %s --CfgFile=$CFG_PATH --BMsPath=$BM_PATH --LogFileGenEnable=1"
else
    # use the command below to print simulator output on both terminal and the log file
    ./waf --run "scratch/MultiCoreSimulator --CfgFile=$CFG_PATH --BMsPath=$BM_PATH --LogFileGenEnable=1" 2>&1 | tee "$LOG_PATH"
    
    # use the command below to write the simulator output only to the log file
    # ./waf --run "scratch/MultiCoreSimulator --CfgFile=$CFG_PATH --BMsPath=$BM_PATH --LogFileGenEnable=1" > "$LOG_PATH" 2>&1
fi

# elapsed_time=$SECONDS
# echo "Execution time: ${elapsed_time} seconds" | tee -a "$LOG_PATH"

cd tools/Assignment3/scripts/

./parser.sh ../logs/$LOG_FILE_NAME
