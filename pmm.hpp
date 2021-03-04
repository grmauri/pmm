#ifndef PMM_HPP_INCLUDED
#define PMM_HPP_INCLUDED

#define MAX_OBJ 500
#define MAX_MOC 50

#include <string>

typedef struct tSolucao
{
    int vetPesMoc[MAX_MOC];
    int vetIdMocObj[MAX_OBJ];
    int funObj;
}Solucao;

int numObj;
int numMoc;
int vetValObj[MAX_OBJ];
int vetPesObj[MAX_OBJ];
int vetCapMoc[MAX_MOC];


void heuConAle(Solucao &s);
void calcFO(Solucao &s);
void escreverSolucao(Solucao &s, const bool flag);

void lerDados(std::string arq);
void testarDados(char *arq);


// TODO
typedef struct tSolucaoBIN
{
    // ???
    int funObj;
}SolucaoBIN;

void heuConAleBIN(SolucaoBIN &s);
void calcFOBIN(SolucaoBIN &s);
void escreverSolucaoBIN(SolucaoBIN &s, const bool flag);
//---

void teste_alocacao();






#endif // PMM_HPP_INCLUDED
