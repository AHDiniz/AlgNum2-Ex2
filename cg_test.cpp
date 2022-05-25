#include "octave/oct.h"
#include "octave/parse.h"
#include <vector>
#include <chrono>
#include <cstdio>
#include <limits>
#include <thread>

typedef struct conjGradIn
{
    double tol;
    int nMaxIter;
} ConjGradIn;

void pcg(Matrix A, ColumnVector b, ConjGradIn c, octave_value_list &result)
{
    octave_value_list inputList;
    octave_value_list outputList;

    inputList(0) = A;
    inputList(1) = b;
    inputList(2) = c.tol;
    inputList(3) = c.nMaxIter;

    auto start = std::chrono::high_resolution_clock::now();
    outputList = (octave::feval("pcg", inputList));
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    result(0) = duration.count();
    result(1) = outputList(0);
    result(2) = outputList(1);
    result(3) = outputList(2);
    result(4) = outputList(3);
    result(5) = outputList(4);
}

DEFUN_DLD(cg_test, args, nargout, "Experience with conjugated gradients method")
{
    octave_value_list result;

    Matrix A = args(0).matrix_value();
    ColumnVector b = args(1).column_vector_value();

    double tolerances[] = {0.000001, 0.0000001, 0.00000001, 0.000000001, 0.0000000001, 0.00000000001};
    int nMaxIters[] = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};

    std::vector<ConjGradIn> conjGrad;

    for (int t = 0; t < 6; ++t)
    {
        for (int i = 0; i < 10; ++i)
        {
            double tol = tolerances[t];
            int nMaxIter = nMaxIters[i];

            ConjGradIn cIn;
            cIn.tol = tol;
            cIn.nMaxIter = nMaxIter;

            conjGrad.push_back(cIn);
        }
    }

    int threadCount = std::thread::hardware_concurrency();
    int usedThreads = 0;
    std::vector<std::thread*> threads;
    ConjGradIn bestCGIn;
    int bestTimeCG_ms;
    octave_value_list bestCGOut;
    double bestResCG = std::numeric_limits<double>::max();
    for (ConjGradIn c : conjGrad)
    {
        octave_value_list inputList;
        octave_value_list outputList;
        int timeMS;

        if (usedThreads < threadCount)
        {
            std::thread* t = new std::thread(pcg, A, b, c, outputList);
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

        timeMS = outputList(0).int_value();
        ColumnVector x = outputList(1).column_vector_value();
        int flag = outputList(2).int_value();
        double relres = outputList(3).double_value();
        Matrix iter = outputList(4).matrix_value();
        ColumnVector resvec = outputList(5).column_vector_value();

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        timeMS = duration.count();

        if (relres < bestResCG)
        {
            bestResCG = relres;
            bestTimeCG_ms = timeMS;
            bestCGOut = outputList;
            bestCGIn = c;
        }
    }

    result(0) = bestCGIn.tol;
    result(1) = bestCGIn.nMaxIter;
    result(2) = (float)bestTimeCG_ms / 1000;
    result(3) = bestCGOut(1);
    result(4) = bestCGOut(2);
    result(5) = bestCGOut(3);
    result(6) = bestCGOut(4);
    result(7) = bestCGOut(5);

    return result;
}
