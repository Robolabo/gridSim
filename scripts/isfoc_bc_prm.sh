#!/bin/bash
#
#$ -N isfoc_bc_prm
#$ -pe mpich 58
#$ -cwd
#$ -q all.q
#
/opt/openmpi/bin/mpirun -np 58 /home/manuel/gridSim/gridSim_parallel -p /home/manuel/gridSim/cnf_parallel/isfoc_bc_prm_scn.xml
