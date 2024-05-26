import subprocess
import re


with open('nqueens.out', 'w') as f:
    f.write('store_sol,board_size,par_time,seq_time,num_sol\n')
    for i in range(3, 15):
        out = subprocess.run(["build/nqueens_count", f'{i}'], capture_output=True, text=True)
        board_size = int(re.findall(r'Board size: (\d+)', out.stdout)[0])
        par_time = float(re.findall(r'par time: (\d+\.\d+(?:e-?\d+)?)\[s\]', out.stdout)[0])
        seq_time = float(re.findall(r'seq time: (\d+\.\d+(?:e-?\d+)?)\[s\]', out.stdout)[0])
        solutions = re.findall(r'solution count: (\d+)', out.stdout)
        assert len(solutions) == 2
        assert solutions[0] == solutions[1]
        f.write(f'0,{board_size},{par_time},{seq_time},{int(solutions[0])}\n')
        out = subprocess.run(["build/nqueens_report", f'{i}'], capture_output=True, text=True)
        board_size = int(re.findall(r'Board size: (\d+)', out.stdout)[0])
        par_time = float(re.findall(r'par time: (\d+\.\d+(?:e-?\d+)?)\[s\]', out.stdout)[0])
        seq_time = float(re.findall(r'seq time: (\d+\.\d+(?:e-?\d+)?)\[s\]', out.stdout)[0])
        solutions = re.findall(r'solution count: (\d+)', out.stdout)
        assert len(solutions) == 2
        assert solutions[0] == solutions[1]
        f.write(f'1,{board_size},{par_time},{seq_time},{int(solutions[0])}\n')