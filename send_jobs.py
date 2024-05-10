import subprocess

''' run.batch
#!/bin/bash -l
#SBATCH --job-name $4-$1-$2         # this will be shown in the queueing system
#SBATCH --output "$4-$1-$2.out"   # stdout redirection
#SBATCH --error "$4-$1-$2.err"    # stderr redirection
#SBATCH --account "g96-1905"          # the number of our grant
#SBATCH --nodes $1                    # how many nodes we want
#SBATCH --tasks-per-node 1           # each node is 2 socket, 12 core, so we want 24 tasks on each node
#SBATCH --time $3              # if the job runs longer than this, it'll be killed
#SBATCH --min-cpus 1
#SBATCH --mem 1024   # Try MB maybe if doesnt work

srun ./build/$4 $2
'''

graph_sizes = [10,100,1_000,10_000]

is_test = True

num_nodes = 1
while num_nodes <= 32:
    for s in graph_sizes:
        if is_test:
            subprocess.run(['echo', 'run.batch', f'{num_nodes}', f'{s}', '00:05:00', 'laplace-par-sync'])
        else:
            subprocess.run(['sbatch', 'run.batch', f'{num_nodes}', f'{s}', '00:05:00', 'laplace-par-sync'])
    num_nodes <<= 1