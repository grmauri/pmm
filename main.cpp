#include <iostream>

#include "pmm.hpp"

using namespace std;

int main()
{
    lerDados("pmm1.txt");
    testarDados("");

    return 0;
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
