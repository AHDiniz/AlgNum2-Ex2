#include "octave/oct.h"
#include "octave/parse.h"
#include <vector>
#include <chrono>

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
    int ksGmres[] = {10, 20, 30, 40, 50, 100, 150, 200};

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

            for (int k = 0; k < 8; ++k)
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

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        timeMS = duration.count();
    }

    GMRESIn bestGMRESIn;
    int bestTimeGMRES_ms;
    octave_value_list bestGMRESOut;
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

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        timeMS = duration.count();
    }

    return result;
}
