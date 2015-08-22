#!/bin/bash -l

# Batch script to run an OpenMP threaded job on Legion with the upgraded
# software stack under SGE.

# 1. Force bash as the executing shell.
#$ -S /bin/bash

# 2. Request ten minutes of wallclock time (format hours:minutes:seconds).
#$ -l h_rt=9:00:00

# 3. Request RAM for the entire job (independent of thread number)
#$ -l mem=4G

# 4. Request 5 gigabyte of TMPDIR space (default is 10 GB)
#$ -l tmpfs=5G

# 5. Set the name of the job.
#$ -N ES

# 6. Select threads.
#$ -l thr=12

# 7. Set the working directory to somewhere in your scratch space.  This is
# a necessary step with the upgraded software stack as compute nodes cannot
# write to $HOME.
#$ -wd /home/zcahmge/Scratch/output

# 8. Run the application.
$HOME/src/exhaustive-search/finder -s $1 -e $2 -n $3 -m $4
