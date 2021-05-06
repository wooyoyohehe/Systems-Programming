#!/bin/sh

if [ $# -ne 4 ]; then
  echo "usage: sh run_and_time.sh program test-num(1-8) nruns starting-seed" >&2
  exit 1
fi

if [ ! -f runs.txt ]; then
  echo 'No file runs.txt' >&2
  exit 1
fi

params=`sed -n "$2"p runs.txt`
echo $params

nruns=$3
seed=$4
prog=$1
t=0
ts=0
tf=0
n=0
i=1
failed=0
while [ $i -le $nruns ]; do
  set `$prog $params $seed | tail -n 1`
  n=`echo $n | awk '{ print $1+1 }'`
  if [ $2 != "Simulation" ]; then
    failed=1
    echo "Run $i: Failed at person $5"
  else
    a=`echo $t $4 $6 | awk '{ print $3/$2*100.0 }'`
    t=`echo $t $4 $6 | awk '{ print $1 + $3/$2*100.0 }'`
    ts=`echo $ts $4 | awk '{ print $1+$2 }'`
    tf=`echo $tf $6 | awk '{ print $1+$2 }'`
    avg=`echo $t $n | awk '{ print $1/$2 }'`
    echo "Run $i - $6/$4: $a percent.  Overall Avg: $avg"
  fi
  i=`echo $i | awk '{ print $1+1 }'`
  seed=`echo $seed | awk '{ print $1+1 }'`
done

if [ $failed = 1 ]; then
  echo $params "     Failed "
else
  ts=`echo $ts $n | awk '{ printf "%.2lf\n", $1/$2 }'`
  tf=`echo $tf $n | awk '{ printf "%.2lf\n", $1/$2 }'`
  echo "$params    $tf / $ts : $avg"
fi
