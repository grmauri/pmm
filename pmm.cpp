#include "pmm.hpp"

#include <iostream>
#include <memory.h>
#include <time.h>
#include <math.h>

#define MAX(X,Y) ((X > Y) ? X : Y)

//#define MODO_TESTE    // executa apenas testes de desempenho
#define DBG             // modo "debug" (mostra a convergência dos métodos)
//#define RND           // modo aleatório (habilita o srand)

const int PESO = 100;   // peso para inviabilidades

using namespace std;

int main()
{
    #ifdef RND
        srand(time(NULL));
    #endif

    #ifdef MODO_TESTE
        teste_memset();
        teste_estruturas();
        teste_heuConstrutivas();
        teste_buscaLocal();
    #else
        lerDados("pmm2.txt");
        //testarDados("");
        ordenarObjetos();

        Solucao sol;
        clock_t h;
        double tempo_limite, tempo_melhor, tempo_total;

        tempo_limite = 5;

        simulated_annealing(0.975, 2*(numObj*(numMoc+1)), 1000, 0.01, tempo_limite, sol, tempo_melhor, tempo_total);
        printf("SA - FO: %d\tTempo melhor: %.5f\tTempo total: %.5f\n", sol.funObj, tempo_melhor, tempo_total);

        busca_tabu((numObj*(numMoc+1)), tempo_limite, sol, tempo_melhor, tempo_total);
        printf("BT - FO: %d\tTempo melhor: %.5f\tTempo total: %.5f\n", sol.funObj, tempo_melhor, tempo_total);

        grasp(15, tempo_limite, sol, tempo_melhor, tempo_total);
        printf("GRASP - FO: %d\tTempo melhor: %.5f\tTempo total: %.5f\n", sol.funObj, tempo_melhor, tempo_total);

        algoritmo_genetico(10*numObj, (numObj*(numMoc+1)), 7, 15, tempo_limite, sol, tempo_melhor, tempo_total);
        printf("AG - FO: %d\tTempo melhor: %.5f\tTempo total: %.5f\n", sol.funObj, tempo_melhor, tempo_total);

        vns(tempo_limite, sol, tempo_melhor, tempo_total);
        printf("VNS - FO: %d\tTempo melhor: %.5f\tTempo total: %.5f\n", sol.funObj, tempo_melhor, tempo_total);

        ils(tempo_limite, sol, tempo_melhor, tempo_total);
        printf("ILS - FO: %d\tTempo melhor: %.5f\tTempo total: %.5f\n", sol.funObj, tempo_melhor, tempo_total);

    #endif
    return 0;
}



//-------------------------------------------------
void simulated_annealing(const double alfa, const int sa_max, const double temp_ini,
                         const double temp_con, const double tempo_max,
                         Solucao &s, double &tempo_melhor, double &tempo_total)
{
    clock_t hI, hF;
    Solucao s_atual, s_vizinha;
    double temp, delta, x;
    printf("\n\n>>> EXECUTANDO O SA...\n\n");
    hI = clock();
    heuConAle(s);
    calcFO(s);
    hF = clock();
    tempo_melhor = ((double)(hF - hI))/CLOCKS_PER_SEC;
    #ifdef DBG
        printf("FO: %d\tTempo: %.2f\n", s.funObj, tempo_melhor);
    #endif
    tempo_total = tempo_melhor;
    memcpy(&s_atual, &s, sizeof(s));
    while(tempo_total < tempo_max)
    {
        temp = temp_ini;
        while(temp > temp_con)
        {
            for(int i = 0; i < sa_max; i++)
            {
                memcpy(&s_vizinha, &s_atual, sizeof(s_atual));
                gerar_vizinha(s_vizinha);
                delta = s_atual.funObj - s_vizinha.funObj;
                if(delta < 0)
                {
                    memcpy(&s_atual, &s_vizinha, sizeof(s_vizinha));
                    if(s_vizinha.funObj > s.funObj)
                    {
                        memcpy(&s, &s_vizinha, sizeof(s_vizinha));
                        hF = clock();
                        tempo_melhor = ((double)(hF - hI))/CLOCKS_PER_SEC;
                        #ifdef DBG
                            printf("FO: %d\tTempo: %.2f\n", s.funObj, tempo_melhor);
                        #endif
                    }
                }
                else
                {
                    x = rand()%1001;
                    x = x / 1000.0;
                    if(x < exp(-delta/temp))
                        memcpy(&s_atual, &s_vizinha, sizeof(s_vizinha));
                }
            }
            temp = temp * alfa;
            hF = clock();
            tempo_total = ((double)(hF - hI))/CLOCKS_PER_SEC;
            if(tempo_total >= tempo_max)
                break;
        }
    }
}

//-------------------------------------------------
void gerar_vizinha(Solucao &s)
{
    int obj, moc;
    obj = rand()%numObj;
    do
        moc = rand()%(numMoc + 1) - 1;
    while(moc == s.vetSol[obj]);
    s.vetSol[obj] = moc;
    calcFO(s);
}

//-------------------------------------------------
void busca_tabu(const int tam_lista, const double tempo_max, Solucao &s, double &tempo_melhor, double &tempo_total)
{
    clock_t hI, hF;
    Solucao s_vizinha;
    int flag, pos, mocOri, foOri, melViz, melObj, melMoc, melPos, qtd_lista, **lista_tabu;
    printf("\n\n>>> EXECUTANDO A BT...\n\n");
    hI = clock();
    heuConAle(s);
    calcFO(s);
    hF = clock();
    tempo_melhor = ((double)(hF - hI))/CLOCKS_PER_SEC;
    #ifdef DBG
        printf("FO: %d\tTempo: %.2f\n", s.funObj, tempo_melhor);
    #endif
    tempo_total = tempo_melhor;
    memcpy(&s_vizinha, &s, sizeof(s));
    //---------------------------
    // Criar a lista tabu - linha 1 guarda o objeto, e linha 2 guarda a mochila
    lista_tabu = new int*[2];
    lista_tabu[0] = new int[tam_lista];
    lista_tabu[1] = new int[tam_lista];
    qtd_lista = 0;
    //---------------------------
    while(tempo_total < tempo_max)
    {
        //---------------------------
        // Verificar a melhor vizinha
        melViz = -INT_MAX;
        flag = -1; // todos os vizinhos na lista e não há melhora
        for(int j = 0; j < numObj; j++)
        {
            mocOri = s_vizinha.vetSol[j];
            foOri = s_vizinha.funObj;
            for(int i = -1; i < numMoc; i++)
            {
                if(i != s_vizinha.vetSol[j])
                {
                    s_vizinha.vetSol[j] = i;
                    calcFO(s_vizinha);
                    pos = procurar_lista(lista_tabu, qtd_lista, j, i);
                    if(pos != -1) // está na lista tabu
                    {
                        if(s_vizinha.funObj > s.funObj) // vizinha melhor que a melhor
                        {
                            flag = 0; // está na lista tabu, mas é melhor do que a melhor
                            melViz = s_vizinha.funObj;
                            melMoc = i;
                            melObj = j;
                            melPos = pos; // posição na lista tabu
                        }
                    }
                    else // não está na lista
                    {
                        if(s_vizinha.funObj > melViz)
                        {
                            flag = 1; // melhor vizinha que não está na lista tabu
                            melViz = s_vizinha.funObj;
                            melMoc = i;
                            melObj = j;
                            melPos = -1; // não está na lista tabu
                        }
                    }
                }
            }
            s_vizinha.vetSol[j] = mocOri;
            s_vizinha.funObj = foOri;
        }
        //---------------------------
        // Atualizar a lista e gerar a melhor vizinha
        if(flag == -1) // todos os vizinhos na lista e nenhuma melhora encontrada, então aplica-se o primeiro movimento da lista (FIFO)
        {
            melObj = lista_tabu[0][0];
            melMoc = lista_tabu[1][0];
            s_vizinha.vetSol[melObj] = melMoc;
            calcFO(s_vizinha);
            remover_lista(lista_tabu, qtd_lista, 0);
        }
        else
        {
            if(flag == 0) // vizinha melhor que a melhor solução, então remove-se o movimento que gerou a vizinha
                remover_lista(lista_tabu, qtd_lista, melPos);
            else // movimento que gerou a vizinha não está na lista, então deve-se inserir
                inserir_lista(lista_tabu, qtd_lista, tam_lista, melObj, melMoc);
            s_vizinha.vetSol[melObj] = melMoc;
            s_vizinha.funObj = melViz;
        }
        //--------------------------------------
        // Atualizar a melhor solução
        if(s_vizinha.funObj > s.funObj)
        {
            memcpy(&s, &s_vizinha, sizeof(s_vizinha));
            hF = clock();
            tempo_melhor = ((double)(hF - hI))/CLOCKS_PER_SEC;
            #ifdef DBG
                printf("FO: %d\tTempo: %.2f\n", s.funObj, tempo_melhor);
            #endif
        }
        //--------------------------------------
        hF = clock();
        tempo_total = ((double)hF - hI)/CLOCKS_PER_SEC;
    }
    delete[] lista_tabu[0];
    delete[] lista_tabu[1];
}

//-------------------------------------------------
int procurar_lista(int **lista, const int qtd, const int o, const int m)
{
    for(int i = 0; i < qtd; i++)
        if((lista[0][i] == o) && (lista[1][i] == m))
            return i;
    return -1;
}

//-------------------------------------------------
void remover_lista(int **lista, int &qtd, const int pos)
{
    for(int i = pos+1; i < qtd; i++)
    {
        lista[0][i-1] = lista[0][i];
        lista[1][i-1] = lista[1][i];
    }
    qtd--;
}

//-------------------------------------------------
void inserir_lista(int **lista, int &qtd, const int tam, const int o, const int m)
{
    if(qtd == tam)
        remover_lista(lista, qtd, 0); // FIFO
    lista[0][qtd] = o;
    lista[1][qtd] = m;
    qtd++;
}

//-------------------------------------------------
void grasp(const int lrc, const double tempo_max, Solucao &s, double &tempo_melhor, double &tempo_total)
{
    clock_t hI, hF;
    Solucao s_vizinha;
    printf("\n\n>>> EXECUTANDO O GRASP...\n\n");
    tempo_total = tempo_melhor = 0;
    hI = clock();
    s.funObj = -INT_MAX;
    while(tempo_total < tempo_max)
    {
        heuConAleGul(s_vizinha, lrc);
        calcFO(s_vizinha);
        heuBLPM(s_vizinha);
        if(s_vizinha.funObj > s.funObj)
        {
            memcpy(&s, &s_vizinha, sizeof(s_vizinha));
            hF = clock();
            tempo_melhor = ((double)(hF - hI))/CLOCKS_PER_SEC;
            #ifdef DBG
                printf("FO: %d\tTempo: %.2f\n", s.funObj, tempo_melhor);
            #endif
        }
        hF = clock();
        tempo_total = ((double)(hF - hI))/CLOCKS_PER_SEC;
    }
}

//-------------------------------------------------
void algoritmo_genetico(const int pop, const int cro, const double mut, const double eli,
                        const double tempo_max, Solucao &s, double &tempo_melhor, double &tempo_total)
{
    int filho, p1, p2;
    clock_t hI, hF;
    Solucao s_vizinha;
    printf("\n\n>>> EXECUTANDO O AG...\n\n");
    hI = clock();
    for(int i = 0; i < pop; i++)
    {
        heuConAle(vet_populacao[i]);
        calcFO(vet_populacao[i]);
    }
    ordenar_populacao(pop);
    hF = clock();
    tempo_melhor = ((double)(hF - hI))/CLOCKS_PER_SEC;
    memcpy(&s, &vet_populacao[0], sizeof(vet_populacao[0]));
    #ifdef DBG
        printf("FO: %d\tTempo: %.2f\n", s.funObj, tempo_melhor);
    #endif
    tempo_total = tempo_melhor;
    while(tempo_total < tempo_max)
    {
       filho = pop;
       for(int i = 0; i < cro; i++)
       {
           //------------------
           // Seleção elite
           p1 = MAX(1,(eli/100.0)*pop);
           p1 = rand()%p1;
           do
               p2 = rand()%pop;
           while(p2 == p1);
           //------------------
           // Cruzamento
           crossover(p1,p2,filho,filho+1);
           if(vet_populacao[filho].funObj > s.funObj)
           {
                memcpy(&s, &vet_populacao[filho], sizeof(vet_populacao[filho]));
                hF = clock();
                tempo_melhor = ((double)(hF - hI))/CLOCKS_PER_SEC;
                #ifdef DBG
                    printf("FO: %d\tTempo: %.2f\n", s.funObj, tempo_melhor);
                #endif
           }
           if(vet_populacao[filho+1].funObj > s.funObj)
           {
                memcpy(&s, &vet_populacao[filho+1], sizeof(vet_populacao[filho+1]));
                hF = clock();
                tempo_melhor = ((double)(hF - hI))/CLOCKS_PER_SEC;
                #ifdef DBG
                    printf("FO: %d\tTempo: %.2f\n", s.funObj, tempo_melhor);
                #endif
           }
           //------------------
           // Mutação
           if(rand()%100 < mut)
           {
               gerar_vizinha(vet_populacao[filho]);
               if(vet_populacao[filho].funObj > s.funObj)
               {
                    memcpy(&s, &vet_populacao[filho], sizeof(vet_populacao[filho]));
                    hF = clock();
                    tempo_melhor = ((double)(hF - hI))/CLOCKS_PER_SEC;
                    #ifdef DBG
                        printf("FO: %d\tTempo: %.2f\n", s.funObj, tempo_melhor);
                    #endif
               }
           }
           if(rand()%100 < mut)
           {
               gerar_vizinha(vet_populacao[filho+1]);
               if(vet_populacao[filho+1].funObj > s.funObj)
               {
                    memcpy(&s, &vet_populacao[filho+1], sizeof(vet_populacao[filho+1]));
                    hF = clock();
                    tempo_melhor = ((double)(hF - hI))/CLOCKS_PER_SEC;
                    #ifdef DBG
                        printf("FO: %d\tTempo: %.2f\n", s.funObj, tempo_melhor);
                    #endif
               }
           }
           filho += 2;
       }
       //------------------
       // Seleção de sobreviventes
       ordenar_populacao(pop + 2 * cro);
       hF = clock();
       tempo_total = ((double)(hF - hI))/CLOCKS_PER_SEC;
    }
}

void crossover(const int p1, const int p2, const int f1, const int f2)
{
    int p = 1 + rand()%(numObj-1);
    for(int i = 0; i < p; i++)
    {
        vet_populacao[f1].vetSol[i] = vet_populacao[p1].vetSol[i];
        vet_populacao[f2].vetSol[i] = vet_populacao[p2].vetSol[i];
    }
    for(int i = p; i < numObj; i++)
    {
        vet_populacao[f1].vetSol[i] = vet_populacao[p2].vetSol[i];
        vet_populacao[f2].vetSol[i] = vet_populacao[p1].vetSol[i];
    }
    calcFO(vet_populacao[f1]);
    calcFO(vet_populacao[f2]);
}

void ordenar_populacao(const int limite)
{
    int flag;
    Solucao aux;
    flag = 1;
    while(flag)
    {
        flag = 0;
        for(int i = 0; i < limite-1;i++)
        {
            if(vet_populacao[i].funObj < vet_populacao[i+1].funObj)
            {
                flag = 1;
                memcpy(&aux, &vet_populacao[i], sizeof(vet_populacao[i]));
                memcpy(&vet_populacao[i], &vet_populacao[i+1], sizeof(vet_populacao[i+1]));
                memcpy(&vet_populacao[i+1], &aux, sizeof(aux));
            }
        }
    }
}

//-------------------------------------------------
void vns(const double tempo_max, Solucao &s, double &tempo_melhor, double &tempo_total)
{
    int v;
    clock_t hI, hF;
    Solucao s_vizinha;
    printf("\n\n>>> EXECUTANDO O VNS...\n\n");
    hI = clock();
    heuConAle(s);
    calcFO(s);
    hF = clock();
    tempo_melhor = ((double)(hF - hI))/CLOCKS_PER_SEC;
    #ifdef DBG
        printf("FO: %d\tTempo: %.2f\n", s.funObj, tempo_melhor);
    #endif
    tempo_total = tempo_melhor;
    while(tempo_total < tempo_max)
    {
        v = 0;
        while(v < 2)
        {
            memcpy(&s_vizinha, &s, sizeof(s));
            if(v == 0)
                gerar_vizinha(s_vizinha);
            else
                gerar_vizinha2(s_vizinha);
            heuBLPM(s_vizinha);
            if(s_vizinha.funObj > s.funObj)
            {
                memcpy(&s, &s_vizinha, sizeof(s_vizinha));
                hF = clock();
                tempo_melhor = ((double)(hF - hI))/CLOCKS_PER_SEC;
                #ifdef DBG
                    printf("FO: %d\tTempo: %.2f\n", s.funObj, tempo_melhor);
                #endif
                v = 0;
            }
            else
                v++;
        }
        hF = clock();
        tempo_total = ((double)(hF - hI))/CLOCKS_PER_SEC;
    }
}

//-------------------------------------------------
void gerar_vizinha2(Solucao &s)
{
    int obj1, obj2, aux;
    obj1 = rand()%numObj;
    do
        obj2 = rand()%numObj;
    while(s.vetSol[obj2] == s.vetSol[obj1]);
    aux = s.vetSol[obj1];
    s.vetSol[obj1] = s.vetSol[obj2];
    s.vetSol[obj2] = aux;
    calcFO(s);
}

//-------------------------------------------------
void ils(const double tempo_max, Solucao &s, double &tempo_melhor, double &tempo_total)
{
    clock_t hI, hF;
    Solucao s_vizinha;
    printf("\n\n>>> EXECUTANDO O ILS...\n\n");
    hI = clock();
    heuConAle(s);
    calcFO(s);
    heuBLPM(s);
    hF = clock();
    tempo_melhor = ((double)(hF - hI))/CLOCKS_PER_SEC;
    #ifdef DBG
        printf("FO: %d\tTempo: %.2f\n", s.funObj, tempo_melhor);
    #endif
    tempo_total = tempo_melhor;
    while(tempo_total < tempo_max)
    {
        memcpy(&s_vizinha, &s, sizeof(s));
        gerar_vizinha(s_vizinha);
        heuBLPM(s_vizinha);
        if(s_vizinha.funObj > s.funObj)
        {
            memcpy(&s, &s_vizinha, sizeof(s_vizinha));
            hF = clock();
            tempo_melhor = ((double)(hF - hI))/CLOCKS_PER_SEC;
            #ifdef DBG
                printf("FO: %d\tTempo: %.2f\n", s.funObj, tempo_melhor);
            #endif
        }
        hF = clock();
        tempo_total = ((double)(hF - hI))/CLOCKS_PER_SEC;
    }
}

//-------------------------------------------------
void heuBLRA(Solucao &s, const int iteracoes)
{
    int obj, moc, mocOri, foOri, flag;
    int melFO = s.funObj;
    while(true)
    {
        flag = 1;
        for(int t = 0; t < iteracoes; t++)
        {
            foOri = s.funObj;
            obj = rand()%numObj;
            do
                moc = rand()%(numMoc + 1) - 1;
            while(moc == s.vetSol[obj]);
            mocOri = s.vetSol[obj];
            s.vetSol[obj] = moc;
            calcFO(s);
            if(s.funObj > melFO)
            {
                melFO = s.funObj;
                flag = 0;
            }
            else
            {
                s.vetSol[obj] = mocOri;
                s.funObj = foOri;
            }
        }
        if(flag)
            break;
    }
    calcFO(s);
}

//-------------------------------------------------
void heuBLMM(Solucao &s)
{
    int mocOri, foOri, melObj, melMoc, flag;
    int melFO = s.funObj;
    while(true)
    {
        flag = 0;
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
                        melObj = j;
                        melMoc = i;
                        flag = 1;
                    }
                }
            }
            s.vetSol[j] = mocOri;
            s.funObj = foOri;
        }
        if(flag)
        {
            s.vetSol[melObj] = melMoc;
            s.funObj = melFO;
        }
        else
            break;
    }
    calcFO(s);
}

//-------------------------------------------------
void heuBLPM(Solucao &s)
{
    int vetObjAux[MAX_OBJ]; // usado para evitar determinismo na ordem de teste dos objetos
    int mocOri, foOri, indice, aux;
    int melFO = s.funObj;
    for(int j = 0; j < numObj; j++)
        vetObjAux[j] = j;
    INICIO : ;
    foOri = s.funObj;
    for(int j = 0; j < numObj; j++)
    {
        indice = j + rand()%(numObj - j);
        mocOri = s.vetSol[vetObjAux[indice]];
        for(int i = -1; i < numMoc; i++)
        {
            if(i != s.vetSol[vetObjAux[indice]])
            {
                s.vetSol[vetObjAux[indice]] = i;
                calcFO(s);
                if(s.funObj > melFO)
                {
                    melFO = s.funObj;
                    goto INICIO;
                }
                else
                {
                    s.vetSol[vetObjAux[indice]] = mocOri;
                    s.funObj = foOri;
                }
            }
        }
        aux = vetObjAux[j];
        vetObjAux[j] = vetObjAux[indice];
        vetObjAux[indice] = aux;
    }
    calcFO(s);
}

//-------------------------------------------------
void heuConAleGul(Solucao &s, const int percentual)
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
void escreverSol(const Solucao &s, const bool flag)
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
    int flag, aux;
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




//======== ESTRUTURAS E METODOS AUXILIARES ========
const int PESO2 = 1000; // peso usado para objetos levados em mais de uma mochila

//-------------------------------------------------
void teste_buscaLocal()
{
    Solucao sol;
    clock_t h;
    double tempo;

    lerDados("pmm3.txt");
    ordenarObjetos();

    printf("\n\n>>> TESTE - BUSCA LOCAL - PMM3\n\n");

    heuConAle(sol);
    calcFO(sol);
    printf("Solucao inicial:");
    escreverSol(sol, 0);

    Solucao sol2;
    memcpy(&sol2, &sol, sizeof(sol));

    Solucao sol3;
    memcpy(&sol3, &sol, sizeof(sol));

    h = clock();
    heuBLPM(sol);
    h = clock() - h;
    tempo = (double)h/CLOCKS_PER_SEC;
    escreverSol(sol, 0);
    printf("BL PM...: %.5f seg.\n",tempo);

    h = clock();
    heuBLMM(sol2);
    h = clock() - h;
    tempo = (double)h/CLOCKS_PER_SEC;
    escreverSol(sol2, 0);
    printf("BL MM...: %.5f seg.\n",tempo);

    h = clock();
    heuBLRA(sol3, (numObj * (numMoc + 1)));
    h = clock() - h;
    tempo = (double)h/CLOCKS_PER_SEC;
    escreverSol(sol3, 0);
    printf("BL RA...: %.5f seg.\n",tempo);
}

//-------------------------------------------------
void teste_heuConstrutivas()
{
    Solucao sol;
    clock_t h;
    double tempo;
    const int repeticoes = 10000;

    lerDados("pmm3.txt");
    ordenarObjetos();

    printf("\n\n>>> TESTE - HEURISTICAS CONSTRUTIVAS - PMM3 - %d REPETICOES\n", repeticoes);

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
void teste_estruturas()
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
void escSolBin(SolucaoBIN &s, const bool flag)
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
