// Solves the n-quees puzzle on an n x x checkerboard.
//
// This sequential implementation is to be extended with TBB to get a
// parallel implementation.
//
// HPC course, MIM UW
// Krzysztof Rzadca, LGPL

#include "tbb/tbb.h"
#include <iostream>
#include <list>
#include <vector>
#include <cmath>
#include <atomic>
#include <mutex>

using namespace std;

int board_size = 13;

// Indexed by column. Value is the row where the queen is placed,
// or -1 if no queen.
typedef std::vector<int> Board;

const int par_depth = 3;

void pretty_print(const Board &board)
{
    for (int row = 0; row < (int)board.size(); row++)
    {
        for (const auto &loc : board)
        {
            if (loc == row)
                std::cout << "*";
            else
                std::cout << " ";
        }
        std::cout << std::endl;
    }
}

// Checks the location of queen in column 'col' against queens in cols [0, col).
bool check_col(Board &board, int col_prop, int col_val)
{
    int row_prop = col_val;
    int col_queen = 0;
    for (auto i = board.begin();
         (i != board.end()) && (col_queen < col_prop);
         ++i, ++col_queen)
    {
        int row_queen = *i;
        if (row_queen == row_prop)
        {
            return false;
        }
        if (abs(row_prop - row_queen) == col_prop - col_queen)
        {
            return false;
        }
    }
    return true;
}

void initialize(Board &board, int size)
{
    board.reserve(size);
    for (int col = 0; col < size; ++col)
        board.push_back(-1);
}

// Solves starting from a partially-filled board (up to column col).
void recursive_solve_par(Board &partial_board, int col, atomic_long &num_solutions)
{
    // std::cout << "rec solve col " << col << std::endl;
    // pretty_print(b_partial);

    int b_size = partial_board.size();
    if (col == b_size)
    {
        // num_solutions.fetch_add(1, memory_order_relaxed);
        num_solutions.fetch_add(1, memory_order_relaxed);
    }
    else if (col < par_depth)
    {
        tbb::task_group g;
        for (int tested_row = 0; tested_row < b_size; tested_row++)
        {

            if (check_col(partial_board, col, tested_row))
            {
                g.run([&partial_board, col, &num_solutions, tested_row]
                      { Board board_cp(partial_board.begin(), partial_board.end());
                        board_cp[col] = tested_row;
                        recursive_solve_par(board_cp, col + 1, num_solutions); });
            }
        }
        g.wait();
    }
    else
    {
        for (int tested_row = 0; tested_row < b_size; tested_row++)
        {
            if (check_col(partial_board, col, tested_row))
            {
                partial_board[col] = tested_row;
                recursive_solve_par(partial_board, col + 1, num_solutions);
            }
        }
    }
}

void recursive_solve_seq(Board &partial_board, int col, long &solutions)
{
    // std::cout << "rec solve col " << col << std::endl;
    // pretty_print(b_partial);

    int b_size = partial_board.size();
    if (col == b_size)
    {
        // solutions.push_back(partial_board);
        solutions++;
    }
    else
    {
        for (int tested_row = 0; tested_row < b_size; tested_row++)
        {
            partial_board[col] = tested_row;
            if (check_col(partial_board, col, tested_row))
                recursive_solve_seq(partial_board, col + 1, solutions);
        }
    }
}

int main(int argc, char **argv)
{
    if (argc)
    {
        board_size = strtol(argv[1], nullptr, 0);
    }
    Board board{};
    initialize(board, board_size);
    atomic_long solutions_par(0);

    tbb::tick_count par_start_time = tbb::tick_count::now();
    recursive_solve_par(board, 0, solutions_par);
    tbb::tick_count par_end_time = tbb::tick_count::now();
    double par_time = (par_end_time - par_start_time).seconds();
    cout << "Board size: " << board_size << endl;
    std::cout << "par time: " << par_time << "[s]" << std::endl;

    std::cout << "solution count: " << solutions_par << std::endl;

    long solutions_seq = 0;
    tbb::tick_count seq_start_time = tbb::tick_count::now();
    recursive_solve_seq(board, 0, solutions_seq);
    tbb::tick_count seq_end_time = tbb::tick_count::now();
    double seq_time = (seq_end_time - seq_start_time).seconds();
    std::cout << "seq time: " << seq_time << "[s]" << std::endl;

    std::cout << "solution count: " << solutions_seq << std::endl;
}
