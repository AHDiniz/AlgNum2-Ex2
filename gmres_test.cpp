#include "octave/oct.h"
#include "octave/parse.h"
#include <vector>
#include <chrono>
#include <cstdio>
#include <limits>

#define GMRES(inputList) \
    (octave::feval("gmres", inputList));

typedef struct gmresIn
{
    double tol;
    int nMaxIter;
    int k;
} GMRESIn;

DEFUN_DLD(gmres_test, args, nargout, "GMRES method test")
{
    octave_value_list result;

    Matrix A = args(0).matrix_value();
    ColumnVector b = args(1).column_vector_value();

    double tolerances[] = {0.000001, 0.0000001, 0.00000001, 0.000000001, 0.0000000001, 0.00000000001};
    int maxIterationsVec[] = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
    int ksGmres[] = {2, 3, 5, 10, 20, 30, 40, 50, 100, 150, 200};

    std::vector<GMRESIn> gmresIns;

    for (int k = 0; k < 11; ++k)
    {
        int kIn = ksGmres[k];

        GMRESIn gIn;
        gIn.k = kIn;
        gIn.nMaxIter = 1000;
        gIn.tol = 0.00000000001;

        gmresIns.push_back(gIn);
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

        inputList(0) = A;
        inputList(1) = b;
        inputList(2) = g.k;
        inputList(3) = g.tol;
        inputList(4) = g.nMaxIter;

        auto start = std::chrono::high_resolution_clock::now();
        outputList = GMRES(inputList);
        auto end = std::chrono::high_resolution_clock::now();

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
