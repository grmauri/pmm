#include "pmm.hpp"

#include <iostream>
#include <memory.h>
#include <time.h>

#define MAX(X,Y) ((X > Y) ? X : Y)

//#define MODO_TESTE // comentar para NAO executar os testes

const int PESO = 100;


using namespace std;

int main()
{
    //srand(time(NULL));

    #ifdef MODO_TESTE
        teste_memset();
        teste_Estruturas();
    #else
        lerDados("pmm3.txt");
        //testarDados("");
        ordenarObjetos();
        //teste_heuConstrutivas();

        Solucao sol;
        clock_t h;
        double tempo;

        h = clock();
        //heuConAleGul(sol,10);
        heuConAle(sol);
        calcFO(sol);
        h = clock() - h;
        tempo = (double)h/CLOCKS_PER_SEC;
        escreverSol(sol, 0);
        printf("Construtiva Aleatoria Gulosa...: %.5f seg.\n",tempo);

        h = clock();
        heuBLPM(sol);
        h = clock() - h;
        tempo = (double)h/CLOCKS_PER_SEC;
        escreverSol(sol, 0);
        printf("BL PM...: %.5f seg.\n",tempo);







    #endif
    return 0;
}










//-------------------------------------------------
void heuBLPM(Solucao &s)
{
    int mocOri, foOri;
    int melFO = s.funObj;
    INICIO : ;
    foOri = s.funObj;
    for(int j = 0; j < numObj; j++)
    {
        mocOri = s.vetSol[j];
        for(int i = -1; i < numMoc; i++)
        {
            if(i != s.vetSol[j])
            {
                s.vetSol[j] = i;
                calcFO(s);
                if(s.funObj > melFO)
                {
                    melFO = s.funObj;
                    goto INICIO;
                }
                else
                {
                    s.vetSol[j] = mocOri;
                    s.funObj = foOri;
                }
            }
        }
    }
    calcFO(s);
}

//-------------------------------------------------
void heuConAleGul(Solucao &s, const int &percentual)
{
    int tam, pos, aux;
    int vetAux[MAX_OBJ];
    memcpy(&vetAux, &vetIndObjOrd, sizeof(vetIndObjOrd));
    tam = MAX(1, (percentual / 100.0) * numObj);
    for(int j = 0; j < tam; j++)
    {
        pos = j + rand()%(numObj - j);
        aux = vetAux[pos];
        vetAux[pos] = vetAux[j];
        vetAux[j] = aux;
    }
    memset(&s.vetPes, 0, sizeof(s.vetPes));
    memset(&s.vetSol, -1, sizeof(s.vetSol));
    for(int j = 0; j < numObj; j++)
        for(int i = 0; i < numMoc; i++)
            if(s.vetPes[i] + vetPesObj[vetAux[j]] <= vetCapMoc[i])
            {
                s.vetSol[vetAux[j]] = i;
                s.vetPes[i] += vetPesObj[vetAux[j]];
                break;
            }
}

//-------------------------------------------------
void heuConGul(Solucao &s)
{
    memset(&s.vetPes, 0, sizeof(s.vetPes));
    memset(&s.vetSol, -1, sizeof(s.vetSol));
    for(int j = 0; j < numObj; j++)
        for(int i = 0; i < numMoc; i++)
            if(s.vetPes[i] + vetPesObj[vetIndObjOrd[j]] <= vetCapMoc[i])
            {
                s.vetSol[vetIndObjOrd[j]] = i;
                s.vetPes[i] += vetPesObj[vetIndObjOrd[j]];
                break;
            }
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
void ordenarObjetos()
{
    int flag,aux;
    for(int j = 0; j < numObj; j++)
        vetIndObjOrd[j] = j;
    flag = 1;
    while(flag)
    {
        flag = 0;
        for(int j = 0; j < numObj-1;j++)
        {
            if(((double)vetValObj[vetIndObjOrd[j]])/vetPesObj[vetIndObjOrd[j]] <
               ((double)vetValObj[vetIndObjOrd[j+1]])/vetPesObj[vetIndObjOrd[j+1]])
//            if(vetValObj[vetIndObjOrd[j]] <
//               vetValObj[vetIndObjOrd[j+1]])
            {
                flag = 1;
                aux = vetIndObjOrd[j];
                vetIndObjOrd[j] = vetIndObjOrd[j+1];
                vetIndObjOrd[j+1] = aux;
            }
        }
    }
}

//-------------------------------------------------
void lerDados(string arq)
{
    FILE *f = fopen(arq.c_str(),"r");
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
void teste_heuConstrutivas()
{
    Solucao sol;
    clock_t h;
    double tempo;
    const int repeticoes = 1000;

    h = clock();
    for(int r = 0; r < repeticoes; r++)
        heuConAle(sol);
    calcFO(sol);
    h = clock() - h;
    tempo = (double)h/CLOCKS_PER_SEC;
    escreverSol(sol, 0);
    printf("Construtiva Aleatoria...: %.5f seg.\n",tempo);


    h = clock();
    for(int r = 0; r < repeticoes; r++)
        heuConGul(sol);
    calcFO(sol);
    h = clock() - h;
    tempo = (double)h/CLOCKS_PER_SEC;
    escreverSol(sol, 0);
    printf("Construtiva Gulosa...: %.5f seg.\n",tempo);


    h = clock();
    for(int r = 0; r < repeticoes; r++)
        heuConAleGul(sol, 10);
    calcFO(sol);
    h = clock() - h;
    tempo = (double)h/CLOCKS_PER_SEC;
    escreverSol(sol, 0);
    printf("Construtiva Aleatoria Gulosa...: %.5f seg.\n",tempo);
}

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
