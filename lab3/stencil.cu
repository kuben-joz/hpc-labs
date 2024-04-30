/**
 * stencil.cu: a simple 1d stencil on GPU and on CPU
 *
 * Implement the basic stencil and make sure it works correctly.
 * Then, play with the code
 * - Experiment with block sizes, various RADIUSes and NUM_ELEMENTS.
 * - Measure the memory transfer time, estimate the effective memory bandwidth.
 * - Estimate FLOPS (floating point operations per second)
 * - Switch from float to double: how the performance changes?
 */

#include <time.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <thread>
#include <fstream>

using namespace std;

#define RADIUS 300
#define NUM_ELEMENTS 1000000

#define THREADS_PER_BLOCK 100

#define NUM_CPU_THREADS 12

static void handleError(cudaError_t err, const char *file, int line)
{
    if (err != cudaSuccess)
    {
        printf("%s in %s at line %d\n", cudaGetErrorString(err), file, line);
        exit(EXIT_FAILURE);
    }
}
#define cudaCheck(err) (handleError(err, __FILE__, __LINE__))

__global__ void stencil_1d(double *in, double *out)
{
    int tid = threadIdx.x;
    int bid = blockIdx.x;
    int i = (bid * THREADS_PER_BLOCK) + tid;
    if (i < 0)
    {
        printf("index too small\n");
    }
    else if (i >= NUM_ELEMENTS)
    {
        printf("index too large\n");
    }
    else
    {
        // printf("all ok %d\n", bid);
        for (int j = max(i - RADIUS, 0); j < min(i + RADIUS + 1, NUM_ELEMENTS); j++)
        {
            out[i] += in[j];
        }
    }
}

void thread_exec(double *in, double *out, int mod, int num_threads)
{
    for (int i = mod; i < NUM_ELEMENTS; i += num_threads)
    {
        for (int j = max(i - RADIUS, 0); j < min(i + RADIUS + 1, NUM_ELEMENTS); j++)
        {
            out[i] += in[j];
        }
    }
}

void cpu_stencil_1d(double *in, double *out)
{
    vector<thread> threads;
    for (int i = 0; i < NUM_CPU_THREADS; i++)
    {
        threads.push_back(move(thread(thread_exec, in, out, i, NUM_CPU_THREADS)));
    }

    for (int i = 0; i < NUM_CPU_THREADS; i++)
    {
        threads[i].join();
    }
}

int main()
{
    ofstream device_transfer_file;
    ofstream mem_transfer_file;
    ofstream cpu_file;
    ofstream gpu_file;

    device_transfer_file.open("device_transfer.txt", ios_base::app);
    mem_transfer_file.open("mem_transfer.txt", ios_base::app);
    cpu_file.open("cpu_time.txt", ios_base::app);
    gpu_file.open("gpu_time.txt", ios_base::app);

    // PUT YOUR CODE HERE - INPUT AND OUTPUT ARRAYS
    static double in[NUM_ELEMENTS];
    static double out[NUM_ELEMENTS];

    for (int i = 0; i < NUM_ELEMENTS; i++)
    {
        in[i] = 42.0;
        out[i] = 0.0;
    }
    double *in_gpu;
    double *out_gpu;

    cudaEvent_t start, stop, device_transfer, mem_transfer;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventCreate(&device_transfer);
    cudaEventCreate(&mem_transfer);
    cudaEventRecord(start, 0);

    // PUT YOUR CODE HERE - DEVICE MEMORY ALLOCATION
    cudaCheck(cudaMalloc((void **)&in_gpu, NUM_ELEMENTS * sizeof(*in)));
    cudaCheck(cudaMalloc((void **)&out_gpu, NUM_ELEMENTS * sizeof(*out)));

    cudaCheck(cudaMemcpy(in_gpu, in, NUM_ELEMENTS * sizeof(*in), cudaMemcpyHostToDevice));
    // not neccesary it seems that cuda arrays are always init to 0
    cudaCheck(cudaMemcpy(out_gpu, out, NUM_ELEMENTS * sizeof(*out), cudaMemcpyHostToDevice));

    cudaEventRecord(device_transfer, 0);
    cudaEventSynchronize(device_transfer);
    float deviceElapsedTime;
    cudaEventElapsedTime(&deviceElapsedTime, start, device_transfer);
    printf("Mem -> Device transfer time:  %.4f ms\n", deviceElapsedTime);
    cudaEventDestroy(device_transfer);

    // PUT YOUR CODE HERE - KERNEL EXECUTION

    stencil_1d<<<NUM_ELEMENTS / THREADS_PER_BLOCK, THREADS_PER_BLOCK>>>(in_gpu, out_gpu);
    cudaDeviceSynchronize();

    cudaCheck(cudaPeekAtLastError());

    // PUT YOUR CODE HERE - COPY RESULT FROM DEVICE TO HOST
    cudaEventRecord(mem_transfer, 0);
    static double out_gpu_cp[NUM_ELEMENTS];
    cudaCheck(cudaMemcpy(out_gpu_cp, out_gpu, NUM_ELEMENTS * sizeof(*out_gpu_cp), cudaMemcpyDeviceToHost));

    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    float memElapsedTime;
    cudaEventElapsedTime(&memElapsedTime, mem_transfer, stop);
    printf("Device -> memory transfer time:  %.4f ms\n", memElapsedTime);
    float gpuElapsedTime;
    cudaEventElapsedTime(&gpuElapsedTime, start, stop);
    printf("Total GPU execution time:  %.4f ms\n", gpuElapsedTime);
    cudaEventDestroy(start);
    cudaEventDestroy(stop);
    cudaEventDestroy(mem_transfer);

    // PUT YOUR CODE HERE - FREE DEVICE MEMORY
    cudaCheck(cudaFree(in_gpu));
    cudaCheck(cudaFree(out_gpu));

    struct timespec cpu_start, cpu_stop;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &cpu_start);

    cpu_stencil_1d(in, out);

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &cpu_stop);
    double cpuElapsedTime = (cpu_stop.tv_sec - cpu_start.tv_sec) * 1e3 + (cpu_stop.tv_nsec - cpu_start.tv_nsec) / 1e6;
    printf("CPU execution time:  %.4f ms\n", cpuElapsedTime);

    // PUT YOUR CODE HERE - VERIFY GPU RESULT EQUALS TO CPU RESULT
    for (int i = 0; i < NUM_ELEMENTS; i++)
    {
        if (out_gpu_cp[i] != out[i])
        {
            cout << "GPU not equal to CPU at element " << i << '\n';
            cout << "GPU val " << out_gpu_cp[i] << '\n';
            cout << "CPU val " << out[i] << '\n';
        }
    }

    device_transfer_file << deviceElapsedTime << '\n';
    mem_transfer_file << memElapsedTime << '\n';
    cpu_file << cpuElapsedTime << '\n';
    gpu_file << gpuElapsedTime << '\n';

    return 0;
}
