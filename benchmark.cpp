#include <iostream>
#include <bits/stdc++.h>
#include <fstream>
#include <mpi.h>

using namespace std;

const int buf_size = 10000000;
const int num_iters = 10;
const int num_measurments = 100;
char buf[buf_size];

int main(int argc, char *argv[])
{

    int num_procs;
    int rank;
    int status;
    double startTime;
    double endTime;

    double executionTime;
    vector<double> times;

    MPI_Init(&argc, &argv); /* intialize the library with parameters caught by the runtime */

    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Status status_mpi;
    MPI_File fh;
    if (rank == 0)
    {
        MPI_File_open(MPI_COMM_SELF, "res.txt", MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fh);
    }
    for (int k = 0; (1 << k) < buf_size; k++)
    {
        int size = 1 << k;
        for (int j = 0; j < num_measurments; j++)
        {
            MPI_Barrier(MPI_COMM_WORLD);
            startTime = MPI_Wtime();
            for (int i = 0; i < num_iters; i++)
            {
                if (!rank)
                {
                    status = MPI_Send(&buf,          /* pointer to the message */
                                      size,          /* number of items in the message */
                                      MPI_CHAR,      /* type of data in the message */
                                      !rank,         /* rank of the destination process */
                                      0,             /* app-defined message type */
                                      MPI_COMM_WORLD /* communicator to use */

                    );
                    status = MPI_Recv(&buf,             /* where the message will be saved */
                                      size,             /* max number of elements we expect */
                                      MPI_CHAR,         /* type of data in the message */
                                      !rank,            /* if not MPI_ANY_SOURCE, receive only from source with the given rank  */
                                      0,                /* if not MPI_ANY_TAG, receive only with a certain tag */
                                      MPI_COMM_WORLD,   /* communicator to use */
                                      MPI_STATUS_IGNORE /* if not MPI_STATUS_IGNORE, write comm info here */
                    );
                }
                else
                {
                    status = MPI_Recv(&buf,             /* where the message will be saved */
                                      size,             /* max number of elements we expect */
                                      MPI_CHAR,         /* type of data in the message */
                                      !rank,            /* if not MPI_ANY_SOURCE, receive only from source with the given rank  */
                                      0,                /* if not MPI_ANY_TAG, receive only with a certain tag */
                                      MPI_COMM_WORLD,   /* communicator to use */
                                      MPI_STATUS_IGNORE /* if not MPI_STATUS_IGNORE, write comm info here */
                    );
                    status = MPI_Send(&buf,          /* pointer to the message */
                                      size,          /* number of items in the message */
                                      MPI_CHAR,      /* type of data in the message */
                                      !rank,         /* rank of the destination process */
                                      0,             /* app-defined message type */
                                      MPI_COMM_WORLD /* communicator to use */

                    );
                }
            }
            endTime = MPI_Wtime();
            executionTime = endTime - startTime;
            if (!rank)
            {
                times.push_back(executionTime / num_iters);
            }
        }
        if (!rank)
        {
            sort(times.begin(), times.end());
            times = vector<double>(times.begin() + 5, times.begin() + times.size() - 5);
            for (int i = 0; i < times.size(); i++)
            {
                snprintf(buf, 1000, "%d %d %f\n", i, size, times[i]);
                MPI_File_write(fh, buf, strlen(buf), MPI_CHAR, &status_mpi);
            }
            // fprintf(f,"%d \n",i);

            //        fclose(f);
        }
    }
    if (!rank)
    {
        MPI_File_close(&fh);
    }

    MPI_Finalize();

    return 0;
}