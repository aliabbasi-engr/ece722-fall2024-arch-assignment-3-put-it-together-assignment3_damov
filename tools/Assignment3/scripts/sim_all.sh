#!/bin/bash

if [ -z "$1" ] || [ -z "$2" ]; then
  echo "Usage: $0 <cfg_file> <apps_dir>"
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

cfg_file=$1
apps_dir=$2

log_file="../logs/sim_all.log"

if [ -f $log_file ]; then
  rm $log_file
fi

for app_dir in "$apps_dir"/*/; do
  if [ -d "$app_dir" ]; then
    current_app="$app_dir"
    echo "Running: $0 $cfg_file $app_dir" | tee -a $log_file
    SECONDS=0
    ./run_and_log.sh $cfg_file $current_app
    elapsed_time=$SECONDS
    echo "Execution time: ${elapsed_time} seconds" | tee -a $log_file
  fi
done
