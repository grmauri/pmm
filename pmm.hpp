#ifndef PMM_HPP_INCLUDED
#define PMM_HPP_INCLUDED

#define MAX_OBJ 500
#define MAX_MOC 50

#include <string>

// Estruturas de dados
typedef struct tSolucao
{
    int vetPes[MAX_MOC];
    int vetSol[MAX_OBJ];
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


void heuBLPM(Solucao &s);
//---
void heuConAleGul(Solucao &s, const int &percentual);
void heuConGul(Solucao &s);
void heuConAle(Solucao &s);
//---
void calcFO(Solucao &s);
void escreverSol(const Solucao &s, const bool &flag);
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
    int vetPes[MAX_MOC];          // vetor com o peso das mochilas
    int matSol[MAX_MOC][MAX_OBJ]; // matriz binaria moc x obj
    int funObj;
}SolucaoBIN;

void heuConAleBIN(SolucaoBIN &s);
void calFOBIN(SolucaoBIN &s);
void escSolBin(SolucaoBIN &s, const bool &flag);

void teste_Estruturas();
void teste_memset();
void teste_heuConstrutivas();

#endif // PMM_HPP_INCLUDED
