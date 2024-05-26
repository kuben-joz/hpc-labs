// Illustrates tasks and task groups in TBB.
//
// adopted from https://www.threadingbuildingblocks.org/tutorial-intel-tbb-task-based-programming
// HPC course, MIM UW
// Krzysztof Rzadca, LGPL

#include <iostream>
#include "tbb/tbb.h"
#include "tbb/task_group.h"

using namespace std;

// yes, there are better ways to compute Fibonacci numbers
long seq_fib(long n)
{
    if (n < 2)
    {
        return n;
    }
    else
    {
        long x, y;
        x = seq_fib(n - 1);
        y = seq_fib(n - 2);
        return x + y;
    }
}

long par_fib(long n)
{
    if (n < 2)
    {
        return n;
    }
    else
    {
        long x, y;
        tbb::task_group g;
        g.run([&]
              { x = par_fib(n - 1); }); // spawn a task
        g.run([&]
              { y = par_fib(n - 2); }); // spawn another task
        g.wait();                       // wait for both tasks to complete
        return x + y;
    }
}

long seq_trib(long n)
{
    if (n < 2)
    {
        return n > 0 ? n : 0;
    }
    else
    {
        long x, y, z;
        x = seq_trib(n - 1);
        y = seq_trib(n - 2);
        z = seq_trib(n - 3);
        return x + y + z;
    }
}

long par_trib(long n)
{
    if (n < 2)
    {
        return n > 0 ? n : 0;
    }
    else
    {
        long x, y, z;
        tbb::task_group g;
        g.run([&]
              { x = par_trib(n - 1); }); // spawn a task
        g.run([&]
              { y = par_trib(n - 2); }); // spawn another task
        g.run([&]
              { z = par_trib(n - 3); }); // spawn another task
        g.wait();                        // wait for all tasks to complete
        return x + y + z;
    }
}

int main()
{
    const long n = 30;
    tbb::tick_count seq_start_time = tbb::tick_count::now();
    long s_fib = seq_trib(n);
    tbb::tick_count seq_end_time = tbb::tick_count::now();
    double seq_time = (seq_end_time - seq_start_time).seconds();
    std::cout << "trib " << s_fib << std::endl;
    std::cout << "seq time " << seq_time << "[s]" << std::endl;

    tbb::tick_count par_start_time = tbb::tick_count::now();
    long p_fib = par_trib(n);
    tbb::tick_count par_end_time = tbb::tick_count::now();
    double par_time = (par_end_time - par_start_time).seconds();
    std::cout << "p_trib " << p_fib << std::endl;
    std::cout << "par time " << par_time << "[s]" << std::endl;
}
