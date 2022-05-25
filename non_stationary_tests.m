% matrices_cg = ["cgin/mesh3em5.mat", "cgin/plat362.mat", "cgin/662_bus.mat", "cgin/s1rmq4m1.mat", "cgin/bcsstk36.mat"];

% matrices_gmres = ["gmresin/olm100.mat", "gmresin/oscil_dcop_02.mat", "gmresin/cavity05.mat", "gmresin/cz2548.mat", "gmresin/coater2.mat", "gmresin/Dubcova1.mat"];

load "cgin/mesh3em5.mat";
A = Problem.A;
n = rows(A);
b = ones(n, 1);

[tol, nMaxIter, execTime, x, f, relres, iter, resvec] = cg_test(A, b);
iterations = 0;
[nc, nr] = size(iter);
if nc == 1 && nr == 1
    iterations = iter(1, 1);
else
    iterations = (iter(1, 1) - 1) * n + iter(2, 1);
endif

f = fopen("cgout/mesh3em5.txt", "w");
fprintf(f, "Matriz: mesh3em5\n");
fprintf(f, "Ordem do sistema = %d\n", n);
fprintf(f, "Não nulos = %d\n", nnz(A));
fprintf(f, "Número de iterações = %d\n", iterations);
fprintf(f, "Norma do máximo da solução = %f\n", norm(x, inf));
fclose(f);

hf = figure();
plot(1 : rows(resvec), log(resvec));
xlabel("Número de iterações");
ylabel("log(Resíduo Relativo)");
title("mesh3em5");
print(hf, "cgout/mesh3em5.png", "-dpng");


load "gmresin/olm100.mat";
A = Problem.A;
n = rows(A);
b = ones(n, 1);

[tol, nMaxIter, execTime, x, f, relres, iter, resvec] = gmres_test(A, b);
iterations = 0;
[nc, nr] = size(iter);
if nc == 1 && nr == 1
    iterations = iter(1, 1);
else
    iterations = (iter(1, 1) - 1) * n + iter(2, 1);
endif

fg = fopen("gmresout/olm100.txt", "w");
fprintf(fg, "Matriz: olm100\n");
fprintf(fg, "Ordem do sistema = %d\n", n);
fprintf(fg, "Não nulos = %d\n", nnz(A));
fprintf(fg, "Número de iterações = %d\n", iterations);
fprintf(fg, "Norma do máximo da solução = %f\n", norm(x, inf));
fclose(fg);

hf = figure();
plot(1 : rows(resvec), log(resvec));
xlabel("Número de iterações");
ylabel("log(Resíduo Relativo)");
title("olm100");
print(hf, "gmresout/olm100.png", "-dpng");
