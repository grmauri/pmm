#ifndef PMM_HPP_INCLUDED
#define PMM_HPP_INCLUDED

#define MAX_OBJ 500
#define MAX_MOC 50

#include <string>

// Estruturas de dados
typedef struct tSolucao
{
    int vetPesMoc[MAX_MOC];
    int vetIdMocObj[MAX_OBJ];
    int funObj;
}Solucao;

// Dados de entrada
int numObj;
int numMoc;
int vetValObj[MAX_OBJ];
int vetPesObj[MAX_OBJ];
int vetCapMoc[MAX_MOC];
int vetIndObjOrd[MAX_OBJ];

// Metodos
void simulated_annealing(const double alfa, const int sa_max, const double temp_ini, const double temp_con,
                         const double tempo_max, Solucao &s, double &tempo_melhor, double &tempo_total);
void gerar_vizinha(Solucao &s);
//---
void busca_tabu(const int tam_lista, const double tempo_max, Solucao &s, double &tempo_melhor, double &tempo_total);
int procurar_lista(int **lista, const int qtd, const int o, const int m);
void remover_lista(int **lista, int &qtd, const int pos);
void inserir_lista(int **lista, int &qtd, const int tam, const int o, const int m);
//---
void grasp(const int lrc, const double tempo_max, Solucao &s, double &tempo_melhor, double &tempo_total);
//---
Solucao vet_populacao[100000];
void algoritmo_genetico(const int pop, const int cro, const double mut, const double eli,
                        const double tempo_max, Solucao &s, double &tempo_melhor, double &tempo_total);
void crossover(const int p1, const int p2, const int f1, const int f2);
void ordenar_populacao(const int limite);
//---
void vns(const double tempo_max, Solucao &s, double &tempo_melhor, double &tempo_total);
void gerar_vizinha2(Solucao &s);
//---
void ils(const double tempo_max, Solucao &s, double &tempo_melhor, double &tempo_total);
//---
void heuBLRA(Solucao &s, const int iteracoes);
void heuBLMM(Solucao &s);
void heuBLPM(Solucao &s);
//---
void heuConAleGul(Solucao &s, const int percentual);
void heuConGul(Solucao &s);
void heuConAle(Solucao &s);
//---
void calcFO(Solucao &s);
void escreverSolucao(Solucao &s, const bool flag);
//---
void ordenarObjetos();
void lerDados(std::string arq);
void testarDados(char *arq);
//=================================================


//======== ESTRUTURAS E METODOS AUXILIARES ========
// Usados apenas para comparacao de desempenho

// Solucao representada como uma matriz binaria (variaveis do modelo matematico: x11, x12, ... , xmn)
typedef struct tSolucaoBIN
{
    int vetQtdMocObj[MAX_OBJ];    // vetor com a quantidade de mochilas que cada objeto foi colocado
    int vetPesMoc[MAX_MOC];       // vetor com o peso das mochilas
    int matSol[MAX_MOC][MAX_OBJ]; // matriz binaria moc x obj
    int funObj;
}SolucaoBIN;

void heuConAleBIN(SolucaoBIN &s);
void calcFOBIN(SolucaoBIN &s);
void escreverSolucaoBIN(SolucaoBIN &s, const bool flag);
//---
void testar_alocacao();
void testar_estruturas();
void testar_heuConstrutivas();
void testar_buscaLocal();

#endif // PMM_HPP_INCLUDED