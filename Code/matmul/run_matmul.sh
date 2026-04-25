#!/bin/bash

export GOMP_CPU_AFFINITY="0-119:8 1-119:8"
vals="1 2 4 8 15 30"
for x in $vals; do
  OMP_NUM_THREADS=$x make matmul.out && mv matmul.out matmul.out.$x
done

export GOMP_CPU_AFFINITY="0-959:8 1-959:8 2-959:8 3-959:8 4-959:8 5-959:8 6-959:8 7-959:8"
vals="60 120 240"
for x in $vals; do
  OMP_NUM_THREADS=$x make matmul.out && mv matmul.out matmul.out.$x
done

