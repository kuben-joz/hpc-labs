'''
#!/bin/bash -l
#SBATCH --job-name mim-hello          # this will be shown in the queueing system
#SBATCH --output "floyd-%j.out"   # stdout redirection
#SBATCH --error "floyd-%j.err"    # stderr redirection
#SBATCH --account "g96-1905"          # the number of our grant
#SBATCH --nodes 16                     # how many nodes we want
#SBATCH --tasks-per-node 1           # each node is 2 socket, 12 core, so we want 24 tasks on each node
#SBATCH --time 00:05:00               # if the job runs longer than this, it'll be killed

srun ./floyd-warshall-par.exe 10
'''


num_nodes = 1
while num_nodes <= 16:
    
    num_nodes <<= 1
