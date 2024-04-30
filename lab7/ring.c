#include <mpi.h>
#include <stdint.h>
#include <stdio.h>

int main(int argc, char *argv[])
{

    MPI_Init(&argc, &argv);
    int numProcesses, myRank;

    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    int dest = (myRank + 1) % numProcesses;
    int src = (myRank - 1 + numProcesses) % numProcesses;
    int64_t message;
    int status;

    if (!myRank)
    {
        message = 1;
        status = MPI_Send(&message,      /* pointer to the message */
                          1,             /* number of items in the message */
                          MPI_INT64_T,   /* type of data in the message */
                          dest,          /* rank of the destination process */
                          0,             /* app-defined message type */
                          MPI_COMM_WORLD /* communicator to use */

        );
        MPI_Recv(&message,         /* where the message will be saved */
                 1,                /* max number of elements we expect */
                 MPI_INT64_T,      /* type of data in the message */
                 src,              /* if not MPI_ANY_SOURCE, receive only from source with the given rank  */
                 0,                /* if not MPI_ANY_TAG, receive only with a certain tag */
                 MPI_COMM_WORLD,   /* communicator to use */
                 MPI_STATUS_IGNORE /* if not MPI_STATUS_IGNORE, write comm info here */
        );
        printf("%ld\n", message);
    }
    else
    {
        MPI_Recv(&message,         /* where the message will be saved */
                 1,                /* max number of elements we expect */
                 MPI_INT64_T,      /* type of data in the message */
                 src,              /* if not MPI_ANY_SOURCE, receive only from source with the given rank  */
                 0,                /* if not MPI_ANY_TAG, receive only with a certain tag */
                 MPI_COMM_WORLD,   /* communicator to use */
                 MPI_STATUS_IGNORE /* if not MPI_STATUS_IGNORE, write comm info here */
        );
        message *= myRank;
        status = MPI_Send(&message,      /* pointer to the message */
                          1,             /* number of items in the message */
                          MPI_INT64_T,   /* type of data in the message */
                          dest,          /* rank of the destination process */
                          0,             /* app-defined message type */
                          MPI_COMM_WORLD /* communicator to use */

        );
    }

    MPI_Finalize();

    return 0;
}