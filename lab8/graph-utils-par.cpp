/*
 * A template for the 2019 MPI lab at the University of Warsaw.
 * Copyright (C) 2016, Konrad Iwanicki.
 * Refactoring 2019, Łukasz Rączkowski
 */

#include <cassert>
#include <mpi.h>
#include <algorithm>
#include <cstring>
#include "graph-base.h"
#include "graph-utils.h"

int getFirstGraphRowOfProcess(int numVertices, int numProcesses, int myRank)
{
    if (myRank == numProcesses)
    {
        return numVertices;
    }
    int rem = numVertices % numProcesses;
    int div = numVertices / numProcesses;
    int res = div * myRank + std::min(myRank, rem);
    return res;
}

Graph *createAndDistributeGraph(int numVertices, int numProcesses, int myRank)
{
    assert(numProcesses >= 1 && myRank >= 0 && myRank < numProcesses);

    auto graph = allocateGraphPart(
        numVertices,
        getFirstGraphRowOfProcess(numVertices, numProcesses, myRank),
        getFirstGraphRowOfProcess(numVertices, numProcesses, myRank + 1));

    if (graph == nullptr)
    {
        return nullptr;
    }

    assert(graph->numVertices > 0 && graph->numVertices == numVertices);
    assert(graph->firstRowIdxIncl >= 0 && graph->lastRowIdxExcl <= graph->numVertices);
    int l = graph->firstRowIdxIncl;
    int r = graph->lastRowIdxExcl;
    int buf[(graph->lastRowIdxExcl - graph->firstRowIdxIncl) * numVertices];
    if (!myRank)
    {
        for (int j = l; j < r; j++)
        {
            initializeGraphRow(graph->data[j - l], j, graph->numVertices);
        }
        for (int i = 1; i < numProcesses; i++)
        {
            l = r;
            r = getFirstGraphRowOfProcess(numVertices, numProcesses, i + 1);
            for (int j = l; j < r; j++)
            {
                initializeGraphRow(&buf[(j - l) * numVertices], j, numVertices);
            }
            MPI_Send(buf, (r - l) * numVertices, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }
    else
    {
        MPI_Recv(buf, (r - l) * numVertices, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int j = 0; j < r - l; j++)
        {
            memcpy(graph->data[j], &buf[j * numVertices], numVertices * sizeof(int));
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    return graph;
}

void collectAndPrintGraph(Graph *graph, int numProcesses, int myRank)
{
    assert(numProcesses >= 1 && myRank >= 0 && myRank < numProcesses);
    assert(graph->numVertices > 0);
    assert(graph->firstRowIdxIncl >= 0 && graph->lastRowIdxExcl <= graph->numVertices);

    int l = graph->firstRowIdxIncl;
    int r = graph->lastRowIdxExcl;
    int buf[(graph->lastRowIdxExcl - graph->firstRowIdxIncl) * graph->numVertices];
    if (!myRank)
    {
        for (int j = l; j < r; j++)
        {
            printGraphRow(graph->data[j - l], j, graph->numVertices);
        }
        for (int i = 1; i < numProcesses; i++)
        {
            l = r;
            r = getFirstGraphRowOfProcess(graph->numVertices, numProcesses, i + 1);
            MPI_Recv(buf, (r - l) * graph->numVertices, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for (int j = l; j < r; j++)
            {
                printGraphRow(&buf[(j - l) * graph->numVertices], j, graph->numVertices);
            }
        }
    }
    else
    {
        for (int j = 0; j < r - l; j++)
        {
            memcpy(&buf[j * graph->numVertices], graph->data[j], graph->numVertices * sizeof(int));
        }
        MPI_Send(buf, (r - l) * graph->numVertices, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);
}

void destroyGraph(Graph *graph, int numProcesses, int myRank)
{
    freeGraphPart(graph);
}
