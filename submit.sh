#!/bin/bash -l

# Batch script to run an OpenMP threaded job on Legion with the upgraded
# software stack under SGE.

# 1. Force bash as the executing shell.
#$ -S /bin/bash

# 2. Request ten minutes of wallclock time (format hours:minutes:seconds).
#$ -l h_rt=0:10:0

# 3. Request 2 gigabyte of RAM for the entire job (independent of thread number)
#$ -l mem=2G

# 4. Request 5 gigabyte of TMPDIR space (default is 10 GB)
#$ -l tmpfs=5G

# 5. Set the name of the job.
#$ -N ES

# 6. Select 12 threads (the most possible on Legion).
#$ -l thr=12

# 7. Set the working directory to somewhere in your scratch space.  This is
# a necessary step with the upgraded software stack as compute nodes cannot
# write to $HOME.
#$ -wd /home/zcahmge/Scratch/output

# 8. Run the application.
$HOME/src/exhaustive-search
