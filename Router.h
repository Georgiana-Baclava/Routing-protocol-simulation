
#ifndef _ROUTER_H_
#define _ROUTER_H_

#include "api.h"
#include "sim.h"
#include "utils.h"

#define NMAX 200

using namespace std;

//clasa pentru un router
class Router{

	

public:

	
	int tpl[NMAX][NMAX];//matricea in care retin topologia
	int tabela[NMAX];//tabela de routare
	vector<int> vers;//vectorul de versiuni

	Router(){
		int i, j;

		//initializare matrice topologie
		for(i = 0; i < NMAX; i++){
			for(j = 0; j < NMAX; j++){
				tpl[i][j] = -1;
			}
		}

		//initializare vector versiuni
		for(i = 0; i < NMAX; i++)
			vers.push_back(-1);

		//initializare tabela routare
		for(i = 0; i < NMAX; i++){
			tabela[i] = -1;
		}
	}
};

#endif
