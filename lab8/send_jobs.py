import subprocess

''' run.batch
#!/bin/bash -l
#SBATCH --job-name mim-hello          # this will be shown in the queueing system
#SBATCH --output floyd-$1-$2.out   # stdout redirection
#SBATCH --error floyd-$1-$2.err    # stderr redirection
#SBATCH --account g96-1905          # the number of our grant
#SBATCH --nodes $1                    # how many nodes we want
#SBATCH --tasks-per-node 1           # each node is 2 socket, 12 core, so we want 24 tasks on each node
#SBATCH --time $3              # if the job runs longer than this, it'll be killed
#SBATCH --min-cpus 1
#SBATCH --mem 1024   # Try MB maybe if doesnt work

srun ./floyd-warshall-par.exe $2
'''

graph_sizes_small = [16,128,512]
graph_sizes_big = [1024]

is_test = False

#num_nodes = 1
#while num_nodes <= 16:
#    for s in graph_sizes_small:
#        if is_test:
#            subprocess.run(['echo', '-N',f'{num_nodes}', '-J', f'floyd-{num_nodes}-{s}', '-o', f'fl-{num_nodes}-{s}.out', 
#                '-e', f'fl-{num_nodes}-{s}.err', '--time', '00:01:00', 'run.batch', f'{s}'])
#        else:
#            subprocess.run(['sbatch', '-N',f'{num_nodes}', '-J', f'floyd-{num_nodes}-{s}', '-o', f'fl-{num_nodes}-{s}.out', 
#                            '-e', f'fl-{num_nodes}-{s}.err', '--time', '00:01:00', 'run.batch', f'{s}'])
#    for s in graph_sizes_big:
#        if is_test:
#            subprocess.run(['echo', '-N',f'{num_nodes}', '-J', f'floyd-{num_nodes}-{s}', '-o', f'fl-{num_nodes}-{s}.out', 
#                            '-e', f'fl-{num_nodes}-{s}.err', '--time', '00:04:00', 'run.batch', f'{s}'])
#        else:
#            subprocess.run(['sbatch', '-N',f'{num_nodes}', '-J', f'floyd-{num_nodes}-{s}', '-o', f'fl-{num_nodes}-{s}.out', 
#                            '-e', f'fl-{num_nodes}-{s}.err', '--time', '00:04:00', 'run.batch', f'{s}'])
#
#    num_nodes <<= 1

num_nodes = 1

for s in graph_sizes_small:
    if is_test:
        subprocess.run(['echo', '-N',f'{num_nodes}', '-J', f'floyd-seq-{s}', '-o', f'fl-seq-{s}.out', 
                        '-e', f'fl-seq-{s}.err', '--time', '00:01:00', 'seq.batch', f'{s}'])
    else:
        subprocess.run(['sbatch', '-N',f'{num_nodes}', '-J', f'floyd-seq-{s}', '-o', f'fl-seq-{s}.out', 
                        '-e', f'fl-seq-{s}.err', '--time', '00:01:00', 'seq.batch', f'{s}'])
for s in graph_sizes_big:
    if is_test:
        subprocess.run(['echo', '-N',f'{num_nodes}', '-J', f'floyd-seq-{s}', '-o', f'fl-seq-{s}.out', 
                        '-e', f'fl-seq-{s}.err', '--time', '00:04:00', 'seq.batch', f'{s}'])
    else:
        subprocess.run(['sbatch', '-N',f'{num_nodes}', '-J', f'floyd-seq-{s}', '-o', f'fl-seq-{s}.out', 
                        '-e', f'fl-seq-{s}.err', '--time', '00:04:00', 'seq.batch', f'{s}'])