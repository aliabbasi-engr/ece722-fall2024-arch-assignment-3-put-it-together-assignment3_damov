#!/bin/bash

set -e

if [ ! -f "$1" ]; then
  echo "Error: $1 is not a valid file"
  exit 1
fi

cd ../../../

BM_PATH=tools/Assignment2/cache_test_16KB_STRIDE4_ITER5
CFG_PATH=tools/Assignment2/scripts/$1
CFG_FILE_NAME="${CFG_PATH##*/}"
LOG_FILE_NAME="${CFG_FILE_NAME%.xml}.log"
LOG_PATH="tools/Assignment2/logs/$LOG_FILE_NAME"

./waf --run "scratch/MultiCoreSimulator --CfgFile=$CFG_PATH --BMsPath=$BM_PATH --LogFileGenEnable=1" &> $LOG_PATH

cd tools/Assignment2/scripts/

./parser.sh ../logs/$LOG_FILE_NAME
