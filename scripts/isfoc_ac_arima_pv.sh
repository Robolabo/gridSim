#!/bin/bash
#
#$ -N isfoc_ac_arima_pv
#$ -pe mpich 58
#$ -cwd
#$ -q all.q
#
/opt/openmpi/bin/mpirun -np 58 /home/manuel/gridSim/gridSim_parallel -p /home/manuel/gridSim/cnf_parallel/isfoc_ac_arima_pv_scn.xml
