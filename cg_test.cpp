#include "octave/oct.h"
#include "octave/parse.h"
#include <vector>
#include <chrono>
#include <cstdio>
#include <limits>

#define PCG(inputList) \
    (octave::feval("pcg", inputList));

typedef struct conjGradIn
{
    double tol;
    int nMaxIter;
} ConjGradIn;

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

    ConjGradIn bestCGIn;
    int bestTimeCG_ms;
    octave_value_list bestCGOut;
    double bestResCG = std::numeric_limits<double>::max();
    for (ConjGradIn c : conjGrad)
    {
        octave_value_list inputList;
        octave_value_list outputList;
        int timeMS;

        inputList(0) = A;
        inputList(1) = b;
        inputList(2) = 0.00000000001;
        inputList(3) = 1000;

        auto start = std::chrono::high_resolution_clock::now();
        outputList = PCG(inputList);
        auto end = std::chrono::high_resolution_clock::now();

        ColumnVector x = outputList(0).column_vector_value();
        int flag = outputList(1).int_value();
        double relres = outputList(2).double_value();
        Matrix iter = outputList(3).matrix_value();
        ColumnVector resvec = outputList(4).column_vector_value();
        ColumnVector eigest = outputList(5).column_vector_value();

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
    result(3) = bestCGOut(0);
    result(4) = bestCGOut(1);
    result(5) = bestCGOut(2);
    result(6) = bestCGOut(3);
    result(7) = bestCGOut(4);

    return result;
}
