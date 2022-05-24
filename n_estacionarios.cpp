#include "octave/oct.h"
#include "octave/parse.h"
#include <vector>
#include <chrono>
#include <cstdio>
#include <limits>

#define PCG(inputList) \
    (octave::feval("pcg", inputList));

#define GMRES(inputList) \
    (octave::feval("gmres", inputList));

typedef struct conjGradIn
{
    double tol;
    int nMaxIter;
} ConjGradIn;

typedef struct gmresIn
{
    double tol;
    int nMaxIter;
    int k;
} GMRESIn;

DEFUN_DLD(non_stationary_methods, args, nargout, "Non stationary methods test")
{
    octave_value_list result;

    Matrix A = args(0).matrix_value();
    ColumnVector b = args(1).column_vector_value();

    double tolerances[] = {0.0001, 0.00001, 0.000001, 0.0000001};
    int maxIterationsVec[] = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
    int ksGmres[] = {2, 3, 5, 10, 20, 30, 40, 50, 100, 150, 200};

    std::vector<ConjGradIn> conjGrad;
    std::vector<GMRESIn> gmresIns;

    for (int t = 0; t < 4; ++t)
    {
        for (int i = 0; i < 10; ++i)
        {
            double tol = tolerances[t];
            int nMaxIter = maxIterationsVec[i];

            ConjGradIn cIn;
            cIn.tol = tol;
            cIn.nMaxIter = nMaxIter;

            conjGrad.push_back(cIn);

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
        inputList(2) = c.tol;
        inputList(3) = c.nMaxIter;

        auto start = std::chrono::high_resolution_clock::now();
        outputList = PCG(inputList);
        auto end = std::chrono::high_resolution_clock::now();

        ColumnVector x = outputList(0).column_vector_value();
        int flag = outputList(1).int_value();
        double relres = outputList(2).double_value();
        ColumnVector iter = outputList(3).column_vector_value();
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
        ColumnVector iter = outputList(3).column_vector_value();
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



    return result;
}
