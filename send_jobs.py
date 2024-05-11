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

#graph_sizes = [1_000,10_000,20_000]
graph_sizes = [100.1000,10000,20000]
#names = ["laplace-par-async", "laplace-par-sync", "laplace-par-sync-reduce"]
names = ["laplace-par-async"]
is_test = False

num_nodes = 32
while num_nodes <= 32:
    for s in graph_sizes:
        for name in names:
            if is_test:
                subprocess.run(['echo', '-N',f'{num_nodes}', '-J', f'{name}-{num_nodes}-{s}', '-o', f'{name}-{num_nodes}-{s}.out', 
                            '-e', f'{name}-{num_nodes}-{s}.err', '--time', '00:01:00', 'run.batch', f'{s}'])
            else:
                subprocess.run(['sbatch', '-N',f'{num_nodes}', '-J', f'{name}-{num_nodes}-{s}', '-o', f'{name}-{num_nodes}-{s}.out', 
                            '-e', f'{name}-{num_nodes}-{s}.err', '--time', '00:01:00', 'run.batch', f'{s}'])
        #if num_nodes == 1 and not is_test:
        #    name = "laplace-seq"
        #    subprocess.run(['sbatch', '-N',f'{num_nodes}', '-J', f'{name}-{num_nodes}-{s}', '-o', f'{name}-{num_nodes}-{s}.out', 
        #                '-e', f'{name}-{num_nodes}-{s}.err', '--time', '00:05:00', 'run.batch', f'{s} ' f'{name}'])
    num_nodes <<= 1