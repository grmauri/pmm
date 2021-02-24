#ifndef PMM_HPP_INCLUDED
#define PMM_HPP_INCLUDED

#define MAX_OBJ 500
#define MAX_MOC 50

#include <string>

int numObj;
int numMoc;
int vetValObj[MAX_OBJ];
int vetPesObj[MAX_OBJ];
int vetCapMoc[MAX_MOC];

void lerDados(std::string arq);
void testarDados(char *arq);






#endif // PMM_HPP_INCLUDED
