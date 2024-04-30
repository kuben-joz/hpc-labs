#include <math.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <math.h>
#include <complex>
#include <omp.h>
#include <thread>

#include "utils/bmp.cpp"

#define NUM_THREADS 8
#define ACCURACY 32

using namespace std;

void compress(const std::vector<uint8_t> &values, std::vector<float> &Xreal, std::vector<float> &Ximag)
{
    // values, Xreal and Ximag are values describing single color of single row of bitmap.
    // This function will be called once per each (color, row) combination.
    size_t valuesCount = values.size();
    int accuracy = Xreal.size();
    for (int k = 0; k < accuracy; k++)
    {
        for (int i = 0; i < valuesCount; i++)
        {
            int theta = (2 * M_PI * k * i) / valuesCount;
            Xreal[k] += values[i] * cos(theta);
            Ximag[k] -= values[i] * sin(theta);
        }
    }
}

void decompress(std::vector<uint8_t> &values, const std::vector<float> &Xreal, const std::vector<float> &Ximag)
{
    // values, Xreal and Ximag are values describing single color of single row of bitmap.
    // This function will be called once per each (color, row) combination.
    int accuracy = Xreal.size();
    size_t valuesCount = values.size();

    std::vector<float> rawValues(valuesCount, 0);

    for (int i = 0; i < valuesCount; i++)
    {
        for (int k = 0; k < accuracy; k++)
        {
            int theta = (2 * M_PI * k * i) / valuesCount;
            rawValues[i] += Xreal[k] * cos(theta) + Ximag[k] * sin(theta);
        }
        values[i] = rawValues[i] / valuesCount;
    }
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "openmp-use-default-none"
void compressPar(const std::vector<uint8_t> &values, std::vector<float> &Xreal, std::vector<float> &Ximag)
{
    size_t valuesCount = values.size();
    int accuracy = Xreal.size();
    auto Xreal_arr = Xreal.data();
    auto Ximag_arr = Ximag.data();
#pragma omp parallel num_threads(NUM_THREADS) default(shared)
    {
#pragma omp for collapse(2) \
    reduction(+ : Xreal_arr[ : accuracy]) reduction(- : Ximag_arr[ : accuracy])
        for (int k = 0; k < accuracy; k++)
        {
            for (int i = 0; i < valuesCount; i++)
            {
                int theta = (2 * M_PI * k * i) / valuesCount;
                Xreal[k] += values[i] * cos(theta);
                Ximag[k] -= values[i] * sin(theta);
            }
        }
    }
}

void compressParSimple(const std::vector<uint8_t> &values, std::vector<float> &Xreal, std::vector<float> &Ximag)
{
    size_t valuesCount = values.size();
    int accuracy = Xreal.size();
    auto Xreal_arr = Xreal.data();
    auto Ximag_arr = Ximag.data();
#pragma omp parallel num_threads(NUM_THREADS) default(shared)
    {
#pragma omp for collapse(2)
        for (int k = 0; k < accuracy; k++)
        {
            for (int i = 0; i < valuesCount; i++)
            {
                int theta = (2 * M_PI * k * i) / valuesCount;
#pragma omp atomic update
                Xreal[k] += values[i] * cos(theta);
#pragma omp atomic update
                Ximag[k] -= values[i] * sin(theta);
            }
        }
    }
}

void decompressPar(std::vector<uint8_t> &values, const std::vector<float> &Xreal, const std::vector<float> &Ximag)
{
    int accuracy = Xreal.size();
    size_t valuesCount = values.size();

    std::vector<float> rawValues(valuesCount, 0);
    // master uses first half while slaves process the second and vice versa
    std::vector<float> sinValues(valuesCount, 0);
    std::vector<float> cosValues(valuesCount, 0);

#pragma omp parallel num_threads(NUM_THREADS) default(shared)
    {
        // prepare first batch of data for master
#pragma omp for
        for (int i = 0; i < valuesCount; i++)
        {
            int theta = (2 * M_PI * 0 * i) / valuesCount;
            sinValues[i] = sin(theta);
            cosValues[i] = cos(theta);
        }

#pragma omp for
        for (int k = 0; k < accuracy; k++)
        {
#pragma omp master {
            for (int i = 0; i < valuesCount; i++)
            {
                rawValues[i] += Xreal[k] * cosValues[k] +
                                Ximag[k] * sinValues[k];
                values[i] += rawValues[i] / valuesCount;
            }
        }
        for (int i = 0; i < valuesCount; i++)
        {
            // rest of threads
            int theta = (2 * M_PI * (k+1) * i) / valuesCount;
            sinValues[i] = sin(theta);
            cosValues[i] = cos(theta);
        }
    }
}
}
#pragma clang diagnostic pop

int main()
{
    BMP bmp;
    bmp.read("example.bmp");

    size_t accuracy = ACCURACY; // We are interested in values from range [8; 32]

    // bmp.{compress,decompress} will run provided function on every bitmap row and color.
    float compressTime = bmp.compress(compressParSimple, accuracy);
    float decompressTime = bmp.decompress(decompressPar);

    printf("Compress time: %.2lfs\nDecompress time: %.2lfs\nTotal: %.2lfs\n",
           compressTime, decompressTime, compressTime + decompressTime);

    ofstream compress_file;
    ofstream decompress_file;
    ofstream total_file;

    compress_file.open("compress.txt", ios_base::app);
    decompress_file.open("decompress.txt", ios_base::app);
    total_file.open("total.txt", ios_base::app);

    compress_file << compressTime << '\n';
    decompress_file << decompressTime << '\n';
    total_file << compressTime + decompressTime << '\n';

    bmp.write("example_result.bmp");

    return 0;
}
