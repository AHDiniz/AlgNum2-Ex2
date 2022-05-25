#include "octave/oct.h"
#include "octave/parse.h"
#include <vector>
#include <chrono>
#include <cstdio>
#include <limits>
#include <thread>

typedef struct gmresIn
{
    double tol;
    int nMaxIter;
    int k;
} GMRESIn;

void gmres(Matrix A, ColumnVector b, GMRESIn g, octave_value_list &result)
{
    octave_value_list inputList;
    octave_value_list outputList;

    inputList(0) = A;
    inputList(1) = b;
    inputList(2) = g.tol;
    inputList(3) = g.nMaxIter;
    inputList(4) = g.k;

    auto start = std::chrono::high_resolution_clock::now();
    outputList = (octave::feval("gmres", inputList));
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    result(0) = duration.count();
    result(1) = outputList(0);
    result(2) = outputList(1);
    result(3) = outputList(2);
    result(4) = outputList(3);
    result(5) = outputList(4);
}

DEFUN_DLD(gmres_test, args, nargout, "GMRES method test")
{
    octave_value_list result;

    Matrix A = args(0).matrix_value();
    ColumnVector b = args(1).column_vector_value();

    double tolerances[] = {0.000001, 0.0000001, 0.00000001, 0.000000001, 0.0000000001, 0.00000000001};
    int maxIterationsVec[] = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
    int ksGmres[] = {2, 3, 5, 10, 20, 30, 40, 50, 100, 150, 200};

    std::vector<GMRESIn> gmresIns;

    for (int t = 0; t < 6; ++t)
    {
        for (int i = 0; i < 10; ++i)
        {
            double tol = tolerances[t];
            int nMaxIter = maxIterationsVec[i];

            for (int k = 0; k < 11; ++k)
            {
                int kIn = ksGmres[k];

                GMRESIn gIn;
                gIn.k = kIn;
                gIn.nMaxIter = nMaxIter;
                gIn.tol = tol;

                gmresIns.push_back(gIn);
            }
        }
    }

    GMRESIn bestGMRESIn;
    int bestTimeGMRES_ms;
    octave_value_list bestGMRESOut;
    double bestResGMRES = std::numeric_limits<double>::max();
    for (GMRESIn g : gmresIns)
    {
        octave_value_list inputList;
        octave_value_list outputList;
        int timeMS;

        if (usedThreads < threadCount)
        {
            std::thread* t = new std::thread(gmres, A, b, g, outputList);
            threads.push_back(t);
            ++usedThreads;
        }
        else
        {
            for (std::thread* t : threads)
            {
                t->join();
            }
            usedThreads = 0;
        }

        ColumnVector x = outputList(0).column_vector_value();
        int flag = outputList(1).int_value();
        double relres = outputList(2).double_value();
        Matrix iter = outputList(3).matrix_value();
        ColumnVector resvec = outputList(4).column_vector_value();

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        timeMS = duration.count();

        if (relres < bestResGMRES)
        {
            bestResGMRES = relres;
            bestTimeGMRES_ms = timeMS;
            bestGMRESOut = outputList;
            bestGMRESIn = g;
        }
    }

    result(0) = bestGMRESIn.tol;
    result(1) = bestGMRESIn.nMaxIter;
    result(2) = bestGMRESIn.k;
    result(3) = bestTimeGMRES_ms;
    result(4) = bestGMRESOut(0);
    result(5) = bestGMRESOut(1);
    result(6) = bestGMRESOut(2);
    result(7) = bestGMRESOut(3);
    result(8) = bestGMRESOut(4);

    return result;
}
