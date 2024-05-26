// Illustrates parallel for in TBB.
//
// HPC course, MIM UW
// Krzysztof Rzadca, LGPL

#include "tbb/tbb.h"
#include <iostream>
#include <math.h>
#include <mutex>
#include <atomic>

using namespace std;

const long limit = 5'000'000;
long start_idxs[limit];
long end_idxs[limit];

bool is_prime(long num)
{
    long sqrt_num = (long)sqrt(num);
    for (long div = 2; div <= sqrt_num; ++div)
    {
        if ((num % div) == 0)
            return false;
    }
    return true;
}

int main()
{

    long num_p = 0;
    tbb::tick_count seq_start_time = tbb::tick_count::now();
    for (long i = 1; i < limit; ++i)
    {
        if (is_prime(i))
        {
            num_p++;
        }
    }
    tbb::tick_count seq_end_time = tbb::tick_count::now();
    double seq_time = (seq_end_time - seq_start_time).seconds();
    atomic_long idx(0);
    atomic_long num_primes(0);
    tbb::tick_count par_start_time = tbb::tick_count::now();
    tbb::parallel_for(                           // execute a parallel for
        tbb::blocked_range<long>(1, limit),      // pon a range from 1 to limit
        [&](const tbb::blocked_range<long> &r) { // inside a loop, for a partial range r,
            long my_idx = idx.fetch_add(1, memory_order_relaxed);
            long temp_num_p = 0;
            start_idxs[my_idx] = r.begin();
            end_idxs[my_idx] = r.end();
            for (long i = r.begin(); i != r.end(); ++i)
            {
                if (is_prime(i))
                {
                    temp_num_p++;
                }
            }
            if (temp_num_p)
            {
                num_primes.fetch_add(temp_num_p, memory_order_relaxed);
            }
        });
    tbb::tick_count par_end_time = tbb::tick_count::now();
    double par_time = (par_end_time - par_start_time).seconds();
    long lim = idx;
    for (int i = 0; i < lim; i++)
    {
        cout << start_idxs[i] << ' ' << end_idxs[i] << '\n';
    }
}
