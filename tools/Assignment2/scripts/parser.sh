#!/bin/bash

if [ -z "$1" ]; then
  echo "Usage: $0 <file>"
  exit 1
fi

if [ ! -f "$1" ]; then
  echo "Error: $1 is not a valid file"
  exit 1
fi

inputFile="$1"
summaryFile="${inputFile%.log}_summary.log"

custom_echo() {
  local message="$1"
  echo "$message"
  echo "$message" >> "$summaryFile"
}

if [ -f "$summaryFile" ]; then
  rm "$summaryFile"
fi

declare -A numReq
declare -A numMiss

reqRegExp='add2q_l\ *,(0|[1-7]|1[0-7]|10[0-7]),'
missRegExp='Miss\?\ *,(0|[1-7]|1[0-7]|10[0-7]),'

while IFS= read -r line; do
  if [[ $line =~ $reqRegExp ]]; then
    cacheId=${BASH_REMATCH[1]}
    ((numReq[$cacheId]++))
  elif [[ $line =~ $missRegExp ]]; then
    cacheId=${BASH_REMATCH[1]}
    ((numMiss[$cacheId]++))
  fi
done < "$inputFile"

for cacheId in "${!numMiss[@]}"; do

  if (( cacheId >= 0 && cacheId <= 9 )); then
    cacheLevel="L1"
  elif (( cacheId >= 10 && cacheId <= 19 )); then
    cacheLevel="L2"
  elif (( cacheId >= 100 && cacheId <= 109 )); then
    cacheLevel="LLC"
  else
    cacheLevel="Unknown"
  fi

  reqCount=${numReq[$cacheId]:-0}
  missCount=${numMiss[$cacheId]:-0}
  if (( reqCount > 0 )); then
    missRate=$(printf "%.2f" "$(bc -l <<< "scale=4; ($missCount / $reqCount) * 100")")
    hitRate=$(printf "%.2f" "$(bc -l <<< "scale=4; (1 - $missCount / $reqCount) * 100")")
  else
    missRate="undefined"
    hitRate="undefined"
  fi
  custom_echo "cache: $cacheLevel[$cacheId], numMiss: $missCount, numReq: $reqCount, hitRate: $hitRate%, missRate: $missRate%"

done

simDoneLine=$(tail -n 10 "$inputFile" | grep -o 'Current Simulation Done at Bus Clock Cycle # [0-9]\+')

if [[ $simDoneLine =~ ([0-9]+)$ ]]; then
  cycleCount=${BASH_REMATCH[1]}
  custom_echo "cycles: $cycleCount"
else
  custom_echo "Unable to grep simDoneLine!"
fi

lastReqLine=$(tail -n 10 "$inputFile" | grep -E '^[0-9]+,[0-9]+,[^,]+,[0-9]+,[0-9]+$' | tail -n 1)

if [[ $lastReqLine =~ ^([0-9]+), ]]; then
  numReqTotal=${BASH_REMATCH[1]}
  custom_echo "numReqTotal: $numReqTotal"
else
  custom_echo "Unable to lastReqLine!"
fi
