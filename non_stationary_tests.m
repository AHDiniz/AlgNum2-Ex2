cgins = cell(7, 1);
cgins_names = cell(7, 1);

cgins{1} = "cgin/mesh3em5.mat";
cgins_names{1} = "mesh3em5";
cgins{2} = "cgin/plat362.mat";
cgins_names{2} = "plat362";
cgins{3} = "cgin/662_bus.mat";
cgins_names{3} = "662bus";
cgins{4} = "cgin/s1rmq4m1.mat";
cgins_names{4} = "s1rmq4m1";
cgins{5} = "cgin/bcsstk36.mat";
cgins_names{5} = "bcsstk36";
cgins{6} = "cgin/pdb1HYS.mat";
cgins_names{6} = "pdb1HYS";
cgins{7} = "cgin/Dubcova3.mat";
cgins_names{7} = "Dubcova3";

for i = 1 : 7
    load(cgins{i});
    A = Problem.A;
    name = cgins_names{i};
    n = rows(A);
    b = ones(n, 1);
    printf(strcat(name, "\n"));
    tic();
    [x, f, relres, iter, resvec] = pcg(A, b, 0.0000000001, 1000);
    elapsed = toc();
    iterations = 0;
    [nc, nr] = size(iter);
    if nc == 1 && nr == 1
        iterations = iter(1, 1);
    else
        iterations = (iter(1, 1) - 1) * n + iter(2, 1);
    endif

    f = fopen(strcat("cgout/", name, ".txt"), "w");
    fprintf(f, strcat("Matriz: ", name, "\n"));
    fprintf(f, "Ordem do sistema = %d\n", n);
    fprintf(f, "Não nulos = %d\n", nnz(A));
    fprintf(f, "Número de iterações = %d\n", iterations);
    fprintf(f, "Norma do máximo da solução = %f\n", norm(x, inf));
    cond_num = 0;
    try
        cond_num = cond(A);
    catch
        cond_num = inf;
    end_try_catch
    fprintf(f, "Número de condicionamento = %f\n", cond_num);
    fprintf(f, "Tempo de execução = %f\n", elapsed);
    fprintf(f, "Resíduo relativo = %f\n", relres);
    fprintf(f, "Flag = %d\n", f);
    fclose(f);

    hf = figure();
    plot(1 : rows(resvec), log(resvec));
    xlabel("Número de iterações");
    ylabel("log(Resíduo Relativo)");
    title(name);
    print(hf, strcat("cgout/", name, ".svg"), "-dsvg");
endfor

% gmresins = cell(6, 1);
% gmres_names = cell(6, 1);

% gmresins{1} = "gmresin/olm100.mat";
% gmres_names{1} = "olm100";
% gmresins{2} = "gmresin/oscil_dcop_02.mat";
% gmres_names{2} = "oscildcop02";
% gmresins{3} = "gmresin/cavity05.mat";
% gmres_names{3} = "cavity05";
% gmresins{4} = "gmresin/cz2548.mat";
% gmres_names{4} = "cz2548";
% gmresins{5} = "gmresin/coater2.mat";
% gmres_names{5} = "coater02.mat";
% gmresins{6} = "gmresin/Dubcova1.mat";
% gmres_names{6} = "Dubcova1";

% for i = 1 : 6
%     load(gmresins{i});
%     A = Problem.A;
%     name = gmres_names{i};
%     n = rows(A);
%     b = ones(n, 1);
%     printf(strcat(name, "\n"));
%     ks = [2, 3, 5, 10, 50, 100, 150, 200];
%     best_k = 0;
%     best_time = 0;
%     best_x = [];
%     best_f = 0;
%     best_relres = inf;
%     best_iter = [];
%     best_resvec = [];
    
%     if strcmp(name, "coater02") == 1
%         fg = fopen(strcat("gmresout/", name, ".txt"), "w");
%         fprintf(fg, strcat("Matriz: ", name, "\n"));
%         fprintf(fg, "Ordem do sistema = %d\n", n);
%         fprintf(fg, "Não nulos = %d\n", nnz(A));
%         fprintf(fg, "Número de condicionamento = %f\n", cond(A));
%         fclose(fg);
%     else
%         for i = 1 : 8
%             tic();
%             [x, f, relres, iter, resvec] = gmres(A, b, ks(i), 0.00000000001, 1000);
%             elapsed = toc();
%             if relres < best_relres
%                 best_k = ks(i);
%                 best_time = elapsed;
%                 best_x = x;
%                 best_f = f;
%                 best_relres = relres;
%                 best_iter = iter;
%                 best_resvec = resvec;
%             endif
%         endfor
%         iterations = 0;
%         [nc, nr] = size(best_iter);
%         if nc == 1 && nr == 1
%             iterations = best_iter(1, 1);
%         else
%             iterations = (best_iter(1, 1) - 1) * n + best_iter(1, 2);
%         endif

%         fg = fopen(strcat("gmresout/", name, ".txt"), "w");
%         fprintf(fg, strcat("Matriz: ", name, "\n"));
%         fprintf(fg, "Ordem do sistema = %d\n", n);
%         fprintf(fg, "Não nulos = %d\n", nnz(A));
%         fprintf(fg, "Número de iterações = %d\n", iterations);
%         fprintf(fg, "Norma do máximo da solução = %f\n", norm(x, inf));
%         cond_num = 0;
%         try
%             cond_num = cond(A);
%         catch
%             cond_num = -1;
%         end_try_catch
%         fprintf(fg, "Número de condicionamento = %f\n", cond_num);
%         fprintf(fg, "Tempo de execução = %f\n", best_time);
%         fprintf(fg, "Resíduo relativo = %f\n", best_relres);
%         fprintf(fg, "Flag = %d\n", best_f);
%         fprintf(fg, "k = %f\n", best_k);
%         fclose(fg);

%         hf = figure();
%         plot(1 : rows(resvec), log(resvec));
%         xlabel("Número de iterações");
%         ylabel("log(Resíduo Relativo)");
%         title(name);
%         print(hf, strcat("gmresout/", name, ".svg"), "-dsvg");
%     endif
% endfor
