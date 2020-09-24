#include "pmm.hpp"

#include <iostream>
#include <memory.h>
#include <time.h>

#define MAX(X,Y) ((X > Y) ? X : Y)

#define MODO_TESTE // comentar para NÃO executar os testes

const int PESO = 100;


int main()
{
    #ifdef MODO_TESTE
        teste_memset();
        teste_Estruturas();
    #else
        lerDados("pmm1.txt");
        //testarDados("");

        Solucao sol;
        heuConAle(sol);
        calcFO(sol);
        escreverSol(sol, 0);
    #endif
    return 0;
}



//-------------------------------------------------
void heuConAle(Solucao &s)
{
    for(int j = 0; j < numObj; j++)
        s.vetSol[j] = (rand()%(numMoc + 1)) - 1;
}

//-------------------------------------------------
void calcFO(Solucao &s)
{
    memset(&s.vetPes, 0, sizeof(s.vetPes));
    s.funObj = 0;
    for(int j = 0; j < numObj; j++)
        if(s.vetSol[j] != -1)
        {
            s.vetPes[s.vetSol[j]] += vetPesObj[j];
            s.funObj += vetValObj[j];
        }
    for(int i = 0; i < numMoc; i++)
        s.funObj -= PESO * MAX(0, s.vetPes[i] - vetCapMoc[i]);
}

//-------------------------------------------------
void escreverSol(const Solucao &s, const bool &flag)
{
    printf("\nFO: %d\n", s.funObj);
    if(flag)
    {
        printf("VETSOL: ");
        for(int j = 0; j < numObj; j++)
            printf("%d  ",s.vetSol[j]);
        printf("\nPES_MOC: ");
        for(int i = 0; i < numMoc; i++)
            printf("%d  ", s.vetPes[i]);
    }
}

//-------------------------------------------------
void lerDados(char *arq)
{
    FILE *f = fopen(arq,"r");
    fscanf(f,"%d %d\n",&numObj,&numMoc);
    for(int j = 0; j < numObj; j++)
        fscanf(f,"%d",&vetValObj[j]);
    for(int j = 0; j < numObj; j++)
        fscanf(f,"%d",&vetPesObj[j]);
    for(int i = 0; i < numMoc; i++)
        fscanf(f,"%d",&vetCapMoc[i]);
    fclose(f);
}

//-------------------------------------------------
void testarDados(char *arq)
{
    FILE *f;
    if(arq == "")
        f = stdout;
    else
        f = fopen(arq,"w");
    fprintf(f,"%d %d\n",numObj,numMoc);
    for(int j = 0; j < numObj; j++)
        fprintf(f,"%d ",vetValObj[j]);
    fprintf(f,"\n");
    for(int j = 0; j < numObj; j++)
        fprintf(f,"%d ",vetPesObj[j]);
    fprintf(f,"\n");
    for(int i = 0; i < numMoc; i++)
        fprintf(f,"%d ",vetCapMoc[i]);
    if(arq != "")
        fclose(f);
}
//=================================================




//======== ESTRUTURAS E METODOS AUXILIARES ========
const int PESO2 = 1000; // peso usado para objetos levados em mais de uma mochila

//-------------------------------------------------
void teste_Estruturas()
{
    const int repeticoes = 1000;
    //---
    clock_t h;
    Solucao sol;
    SolucaoBIN solB;
    double tempo;

    printf("\n\n>>> COMPARACAO DA ESTRUTURA SOLUCAO - PMM3 - %d REPETICOES\n\n", repeticoes);

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
        calFOBIN(solB);
    h = clock() - h;
    tempo = (double)h/CLOCKS_PER_SEC;
    printf("Calculo da FO (BINARIA).........................: %.5f seg.\n",tempo);
}


//-------------------------------------------------
void teste_memset()
{
    const int tam = 500;
    const int repeticoes = 1000;
    //---
    clock_t h;
    double tempo;
    int **mat;
    int matriz[tam][tam];

    printf("\n>>> TESTE COM UMA MATRIZ %d x %d - %d REPETICOES\n\n", tam, tam, repeticoes);

    //---
    h = clock();
    for(int r = 0; r < repeticoes; r++)
    {
        mat = (int**)malloc(sizeof(int*) * tam);
        for(int i = 0; i < tam; i++)
            mat[i] = (int*)malloc(sizeof(int) * tam);
        for(int i = 0; i < tam; i++)
            free(mat[i]);
        free(mat);
    }
    h = clock() - h;
    tempo = (double)h/CLOCKS_PER_SEC;
    printf("Tempo para alocar e desalocar usando MALLOC.....: %.5f seg.\n",tempo);

    //---
    h = clock();
    for(int r = 0; r < repeticoes; r++)
    {
        mat = new int*[tam];
        for(int i = 0; i < tam; i++)
            mat[i] = new int[tam];
        for(int i = 0; i < tam; i++)
            delete[] mat[i];
        delete[] mat;
    }
    h = clock() - h;
    tempo = (double)h/CLOCKS_PER_SEC;
    printf("Tempo para alocar e desalocar usando NEW........: %.5f seg.\n",tempo);

    //---
    h = clock();
    for(int r = 0; r < repeticoes; r++)
        memset(&matriz,0 , sizeof(matriz));
    h = clock() - h;
    tempo = (double)h/CLOCKS_PER_SEC;
    printf("\nTempo para zerar a matriz estatica com MEMSET...: %.5f seg.\n",tempo);

    //---
    h = clock();
    for(int r = 0; r < repeticoes; r++)
        for(int i = 0; i < tam; i++)
            for(int j = 0; j < tam; j++)
                matriz[i][j] = 0;
    h = clock() - h;
    tempo = (double)h/CLOCKS_PER_SEC;
    printf("Tempo para zerar a matriz estatica com FOR......: %.5f seg.\n",tempo);
}

void heuConAleBIN(SolucaoBIN &s)
{
    for(int i = 0; i < numMoc; i++)
        for(int j = 0; j < numObj; j++)
            s.matSol[i][j] = rand()%2;
}

//-------------------------------------------------
void calFOBIN(SolucaoBIN &s)
{
    memset(&s.vetQtdMocObj, 0, sizeof(s.vetQtdMocObj));
    memset(&s.vetPes, 0, sizeof(s.vetPes));
    s.funObj = 0;
    for(int i = 0; i < numMoc; i++)
        for(int j = 0; j < numObj; j++)
        {
            s.funObj += s.matSol[i][j] * vetValObj[j];
            s.vetPes[i] += s.matSol[i][j] * vetPesObj[j];
            s.vetQtdMocObj[j] += s.matSol[i][j];
        }
    for(int i = 0; i < numMoc; i++)
        s.funObj -= PESO * MAX(0, s.vetPes[i] - vetCapMoc[i]);
    for(int j = 0; j < numObj; j++)
        s.funObj -= PESO2 * MAX(0, s.vetQtdMocObj[j] - 1);
}

//-------------------------------------------------
void escSolBin(SolucaoBIN &s, const bool &flag)
{
    printf("\nFO: %d\n",s.funObj);
    if(flag)
    {
        printf("MATSOL:\n");
        for(int i = 0; i < numMoc; i++)
        {
            for(int j = 0; j < numObj; j++)
                printf("%d  ",s.matSol[i][j]);
            printf("\n");
        }
        printf("\nPES_MOC: ");
        for(int i = 0; i < numMoc; i++)
            printf("%d  ",s.vetPes[i]);

        printf("\nQTD_MOC_OBJ: ");
        for(int j = 0; j < numObj; j++)
            printf("%d  ",s.vetQtdMocObj[j]);
    }
}
