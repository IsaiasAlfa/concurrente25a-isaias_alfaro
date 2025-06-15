include ../../common/Makefile

CC=mpicc
XC=mpic++

FLAG += -fopenmp

ARGS=tests/job001.txt 8
RUNPRE=mpiexec -np 4

REMOVES += out/