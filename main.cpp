#include <iostream>
#include <time.h>
#include <memory.h>

#include "pmm.hpp"

#define MAX(X,Y) ((X > Y) ? X : Y)

using namespace std;

int PESO = 100;

int main()
{
    //teste_alocacao();
    testar_estruturas();


    //Solucao sol;
    //lerDados("pmm1.txt");
    //testarDados("");
    //heuConAle(sol);
    //calcFO(sol);
    //escreverSolucao(sol,1);



    return 0;
}



void heuConAle(Solucao &s)
{
    for(int j = 0; j < numObj; j++)
        s.vetIdMocObj[j] = (rand()%(numMoc+1)) - 1;
}

void calcFO(Solucao &s)
{
    memset(&s.vetPesMoc, 0, sizeof(s.vetPesMoc));
    s.funObj = 0;
    for(int j = 0; j < numObj; j++)
        if(s.vetIdMocObj[j] != -1)
        {
            s.funObj += vetValObj[j];
            s.vetPesMoc[s.vetIdMocObj[j]] += vetPesObj[j];
        }
    for(int i = 0; i < numMoc; i++)
        s.funObj -= PESO * MAX(0, s.vetPesMoc[i] - vetCapMoc[i]);
}

void escreverSolucao(Solucao &s, const bool flag)
{
    printf("\nFO: %d\n", s.funObj);
    if(flag)
    {
        printf("VETOR ID MOCHILAS DE CADA OBJETO: ");
        for(int j = 0; j < numObj; j++)
            printf("%d  ", s.vetIdMocObj[j]);
        printf("\nVETOR PESO MOCHILAS: ");
        for(int i = 0; i < numMoc; i++)
            printf("%d  ", s.vetPesMoc[i]);
    }
}

void lerDados(string arq)
{
    FILE *f = fopen(arq.c_str(), "r");
    fscanf(f,"%d %d", &numObj, &numMoc);
    for(int j = 0; j < numObj; j++)
        fscanf(f, "%d", &vetValObj[j]);
    for(int j = 0; j < numObj; j++)
        fscanf(f, "%d", &vetPesObj[j]);
    for(int i = 0; i < numMoc; i++)
        fscanf(f, "%d", &vetCapMoc[i]);
    fclose(f);
}

void testarDados(char *arq)
{
    FILE *f;
    if(arq == "")
        f = stdout;
    else
        f = fopen(arq, "w");
    fprintf(f,"%d %d\n", numObj, numMoc);
    for(int j = 0; j < numObj; j++)
        fprintf(f, "%d ", vetValObj[j]);
    fprintf(f,"\n");
    for(int j = 0; j < numObj; j++)
        fprintf(f, "%d ", vetPesObj[j]);
    fprintf(f,"\n");
    for(int i = 0; i < numMoc; i++)
        fprintf(f, "%d ", vetCapMoc[i]);
    if(arq != "")
        fclose(f);
}










void testar_estruturas()
{
    const int repeticoes = 1000;

    //---
    clock_t h;
    double tempo;
    Solucao sol;
    SolucaoBIN solB;

    //---
    printf("\n>>> TESTE DE COMPARACAO DAS ESTRUTURAS DA SOLUCAO - PMM3 - %d REPETICOES\n\n", repeticoes);
    lerDados("pmm3.txt");

    //---
    h = clock();
    for(int r = 0; r < repeticoes; r++)
        heuConAle(sol);
    h = clock() - h;
    tempo = (double)h/CLOCKS_PER_SEC;
    printf("Heuristica construtiva aleatoria................: %.5f seg.\n",tempo);

    //---
    h = clock();
    for(int r = 0; r < repeticoes; r++)
        heuConAleBIN(solB);
    h = clock() - h;
    tempo = (double)h/CLOCKS_PER_SEC;
    printf("Heuristica construtiva aleatoria (BINARIA)......: %.5f seg.\n",tempo);

    //---
    h = clock();
    for(int r = 0; r < repeticoes; r++)
        calcFO(sol);
    h = clock() - h;
    tempo = (double)h/CLOCKS_PER_SEC;
    printf("\nCalculo da FO...................................: %.5f seg.\n",tempo);

    //---
    h = clock();
    for(int r = 0; r < repeticoes; r++)
        calcFOBIN(solB);
    h = clock() - h;
    tempo = (double)h/CLOCKS_PER_SEC;
    printf("Calculo da FO (BINARIA).........................: %.5f seg.\n",tempo);

    //---
    heuConAle(sol);
    heuConAleBIN(solB);
    printf("\nSolucao:");
    escreverSolucao(sol, 0);
    printf("\nSolucao BINARIA:");
    escreverSolucaoBIN(solB, 0);
}

void teste_alocacao()
{
    const int tamanho = 500;
    const int repeticoes = 1000;
    //---
    clock_t h;
    double tempo;
    //---
    int **matDinamica;
    int matEstatica[tamanho][tamanho];

    printf("\n>>> TESTE COM UMA MATRIZ %d x %d - %d REPETICOES\n\n", tamanho, tamanho, repeticoes);

    //---
    h = clock();
    for(int r = 0; r < repeticoes; r++)
    {
        matDinamica = (int**)malloc(sizeof(int*) * tamanho);
        for(int i = 0; i < tamanho; i++)
            matDinamica[i] = (int*)malloc(sizeof(int) * tamanho);
        for(int i = 0; i < tamanho; i++)
            free(matDinamica[i]);
        free(matDinamica);
    }
    h = clock() - h;
    tempo = (double)h/CLOCKS_PER_SEC;
    printf("Tempo para alocar e desalocar usando MALLOC.....: %.5f segundos\n", tempo);
    //---

    //---
    h = clock();
    for(int r = 0; r < repeticoes; r++)
    {
        matDinamica = new int*[tamanho];
        for(int i = 0; i < tamanho; i++)
            matDinamica[i] = new int[tamanho];
        for(int i = 0; i < tamanho; i++)
            delete[] matDinamica[i];
        delete[] matDinamica;
    }
    h = clock() - h;
    tempo = (double)h/CLOCKS_PER_SEC;
    printf("Tempo para alocar e desalocar usando NEW........: %.5f segundos\n", tempo);
    //---

    //---
    h = clock();
    for(int r = 0; r < repeticoes; r++)
    {
        for(int i = 0; i < tamanho; i++)
            for(int j = 0; j < tamanho; j++)
                matEstatica[i][j] = 0;
    }
    h = clock() - h;
    tempo = (double)h/CLOCKS_PER_SEC;
    printf("\nTempo para zerar a matriz estatica com FOR......: %.5f segundos\n", tempo);
    //---

    //---
    h = clock();
    for(int r = 0; r < repeticoes; r++)
        memset(&matEstatica, 0, sizeof(matEstatica));
    h = clock() - h;
    tempo = (double)h/CLOCKS_PER_SEC;
    printf("Tempo para zerar a matriz estatica com MEMSET...: %.5f segundos\n", tempo);
    //---
}

const int PESO2 = 50;

void heuConAleBIN(SolucaoBIN &s)
{
    for(int i = 0; i < numMoc; i++)
        for(int j = 0; j < numObj; j++)
            s.matSol[i][j] = rand()%2;
}

void calcFOBIN(SolucaoBIN &s)
{
    memset(&s.vetPesMoc, 0, sizeof(s.vetPesMoc));
    memset(&s.vetQtdMocObj, 0, sizeof(s.vetQtdMocObj));
    s.funObj = 0;
    for(int i = 0; i < numMoc; i++)
        for(int j = 0; j < numObj; j++)
        {
            s.funObj += s.matSol[i][j] * vetValObj[j];
            s.vetPesMoc[i] += s.matSol[i][j] * vetPesObj[j];
            s.vetQtdMocObj[j] += s.matSol[i][j];
        }
    for(int i = 0; i < numMoc; i++)
        s.funObj -= PESO * MAX(0, s.vetPesMoc[i] - vetCapMoc[i]);
    for(int j = 0; j < numObj; j++)
        s.funObj -= PESO2 * MAX(0, s.vetQtdMocObj[j] - 1);
}

void escreverSolucaoBIN(SolucaoBIN &s, const bool flag)
{
    printf("\nFO: %d\n", s.funObj);
    if(flag)
    {
        printf("VETOR QTD MOCHILAS DE CADA OBJETO: ");
        for(int j = 0; j < numObj; j++)
            printf("%d  ", s.vetQtdMocObj[j]);
        printf("\nVETOR PESO MOCHILAS: ");
        for(int i = 0; i < numMoc; i++)
            printf("%d  ", s.vetPesMoc[i]);\
        printf("\nMATRIZ DE SOLUCAO:\n");
        for(int i = 0; i < numMoc; i++)
        {
            for(int j = 0; j < numObj; j++)
                printf("%d  ", s.matSol[i][j]);
            printf("\n");
        }
    }
}
