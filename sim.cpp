/******************************************************************************/
/* Tema 2 Protocoale de Comunicatie (Aprilie 2015)                            */
/******************************************************************************/
#include "api.h"
#include "sim.h"
#include "utils.h"
#include "Router.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdio.h>

using namespace std;

struct intrare_t{
	int cost;
	int vecin;
};

struct msg_t{
	int src;
	int vers;
	int no_nb;
	struct intrare_t date[0];
};

struct mesaj{
	int src;
	int dst;
	int time_mess;
	int tag;
	string message;
};

struct event{
	int r1;
	int r2;
	int type_event;
	int cost;
	int time_stamp;
};


int N;

vector<Router> routers;
vector<mesaj> vmess;
vector<event> events;





void init_sim(int argc, char **argv) {

	int no_events, time_units;
	//instantiem si deschidem un fisier din care citim topologia
	ifstream in(argv[1]);

	//se citeste nr de routere din retea
	in >> N;
	
	int i;
	
	for (i = 0; i < N; i++){
		Router r = Router();
		routers.push_back(r);
	}



	int r1, r2, cost;

	//construim topologia pe baza fisierului de intrare
	while(in >> r1 >> cost >> r2){

		routers[r1].tpl[r1][r2] = routers[r1].tpl[r2][r1] = cost;
		routers[r2].tpl[r1][r2] = routers[r2].tpl[r2][r1] = cost;
	}



	for(i = 0; i < N; i++){
		vector<intrare_t> vecini;//vectorul cu vecinii si costurile routerului i
		int k = 0, j;

			for(j = 0; j < N; j++){
				if(i != j && routers[i].tpl[i][j] != DRUMAX){//pentru fiecare vecin creem o intrare
					intrare_t intrare;
					intrare.vecin = j;
					intrare.cost = routers[i].tpl[i][j];
					vecini.push_back(intrare);
				}
			}

			
			int no_nb = vecini.size();//numarul de vecini ai routerului i
			msg_t *msg = (msg_t *)calloc(1, sizeof(msg_t) + no_nb * sizeof(intrare_t));//mesajul ce trebuie trimis

			if(!msg){
				printf("Alocare esuata!\n" );
				return;
			}
			

			msg->src = i;
			msg->no_nb = no_nb;
			for(k = 0; k < no_nb; k++){
				msg->date[k].vecin = vecini[k].vecin;
				msg->date[k].cost = vecini[k].cost;
			}

			

			for(j = 0; j < no_nb; j++){
				endpoint[i].send_msg(&endpoint[vecini[j].vecin], (char *)msg, sizeof(msg_t) + no_nb * sizeof(intrare_t), NULL);
			}
	}
	in.close();


	//atasam lui in un alt fisier de intrare, messages.in
	in.open(argv[2], ifstream::in);

	int no_messages = 0;

	in >> no_messages;

	i = 0; string line;
	while(no_messages > 0){
		mesaj m;
		in >> m.src >> m.dst >> m.time_mess >> m.tag;
		getline(in, m.message);
		(m.message).append("\n");
		
		vmess.push_back(m);
		i++;
		no_messages--;
	}

	in.close();




	//atasam lui in un alt fisier de intrare, events.in
	in.open(argv[3], ifstream::in);

	in >> time_units;
	in >> no_events;

	i = 0;
	while(no_events > 0){
		event e;
		in >> e.r1 >> e.r2 >> e.type_event >> e.cost >> e.time_stamp;
		events.push_back(e);
		i++;
		no_events--;
	}
}

void clean_sim() {
 
}

void trigger_events() {

	int i, no_events = events.size();
	
	vector<int> routs;

	for(i = 0; i < no_events; i++){
		if(events[i].time_stamp == get_time()){
			routs.push_back(events[i].r1);
			routs.push_back(events[i].r2);

			if(events[i].type_event == EVENT_TYPE_ADD){
				//refacem topologia routerelor in cauza
				routers[events[i].r1].tpl[events[i].r1][events[i].r2] = routers[events[i].r1].tpl[events[i].r2][events[i].r1] = events[i].cost;
				routers[events[i].r2].tpl[events[i].r2][events[i].r1] = routers[events[i].r2].tpl[events[i].r1][events[i].r2] = events[i].cost;
			}

			if(events[i].type_event == EVENT_TYPE_REMOVE){
				//refacem topologia routerelor in cauza
				routers[events[i].r1].tpl[events[i].r1][events[i].r2] = routers[events[i].r1].tpl[events[i].r2][events[i].r1] = DRUMAX;
				routers[events[i].r2].tpl[events[i].r2][events[i].r1] = routers[events[i].r2].tpl[events[i].r1][events[i].r2] = DRUMAX;
			}
		}
	}

	int size = routs.size();
	int r1;

	for(i = 0; i < size; i++){

		vector<intrare_t> vecini;//vectorul cu vecinii si costurile routerului i
		int k = 0, j;
		r1 = routs[i];
		
		for(j = 0; j < N; j++){
			if(r1 != j && routers[r1].tpl[r1][j] != DRUMAX){//pentru fiecare vecin creem o intrare
				intrare_t intrare;
				intrare.vecin = j;
				intrare.cost = routers[r1].tpl[r1][j];
				vecini.push_back(intrare);
				
			}
		}

				
		int no_nb = vecini.size();//numarul de vecini ai routerului r
		msg_t *msg = (msg_t *)calloc(1, sizeof(msg_t) + no_nb * sizeof(intrare_t));//mesajul ce trebuie trimis

		if(!msg){
			printf("Alocare esuata!\n" );
			return;
		}
				

		msg->src = r1;
		msg->vers = get_time();
		msg->no_nb = no_nb;

		for(k = 0; k < no_nb; k++){
			msg->date[k].vecin = vecini[k].vecin;
			msg->date[k].cost = vecini[k].cost;
		}
		for(j = 0; j < no_nb; j++){
			endpoint[r1].send_msg(&endpoint[vecini[j].vecin], (char *)msg, sizeof(msg_t) + no_nb * sizeof(intrare_t), NULL);
		}
	}
}






void process_messages() {

	int i, j, hop, no_messages = vmess.size();
	for(i = 0; i < no_messages; i++){
		if( vmess[i].time_mess == get_time()){
			
			hop = routers[vmess[i].src].tabela[vmess[i].dst];
			
			endpoint[vmess[i].src].route_message(&endpoint[hop], vmess[i].dst, vmess[i].tag, (char *)(vmess[i].message.c_str()),NULL);
			
		}
	}




	int msg_size;
	char message[1024];
	int src, dst, tag;
	bool valid;
	
	for(i = 0; i < N; i++){
	
		while(1){

			msg_size = endpoint[i].recv_protocol_message(message);
			if(msg_size == -1)
				break;
			if(((msg_t *)message)->vers <= routers[i].vers[((msg_t *)message)->src]){
				continue;
			}
			//isi updateza versiunea
			routers[i].vers[((msg_t *)message)->src] = ((msg_t *)message)->vers;
			

			for(int k = 0; k < ((msg_t *)message)->no_nb; k++){
				routers[i].tpl[((msg_t *)message)->src][((msg_t *)message)->date[k].vecin] = ((msg_t *)message)->date[k].cost;
				routers[i].tpl[((msg_t *)message)->date[k].vecin][((msg_t *)message)->src] = ((msg_t *)message)->date[k].cost;
			}

			//altfel trimit mai departe vecinilor mei direct conectati
			for(j = 0; j < N; j++){
				if(i != j  && j != ((msg_t *)message)->src && routers[i].tpl[i][j] != DRUMAX){
					endpoint[i].send_msg(&endpoint[j], message, sizeof(message), NULL);
				}
			}
		}

		while(1){
			valid = endpoint[i].recv_message(&src, &dst, &tag, message);
			if(valid == false)
				break;

			if(i == dst)
				continue;

			hop = routers[i].tabela[dst];

			cout <<"i= " << i << "hopul din process_messages = " << hop;
			endpoint[i].route_message(&endpoint[hop], dst, tag, message, NULL);
		}
	}
	
}


//algoritmul Floyd Warshall calculeaza drumul minim dintre 2 routere si determina next hopurile
void Floyd_Warshall(int r, vector< vector<int> > dist, vector< vector<int> > next){
	for(int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				for(int k = 0; k < N; k++){
					if(i == j || i == k || j == k)
						continue;
					if(dist[i][j] > dist[i][k] + dist[k][j]) { //daca distanta de la routerul i la j este mai mare decat cea prin routerul k
						dist[j][i] = dist[i][j] = dist[i][k] + dist[k][j]; //updatam distanta
						next[i][j] = next[i][k]; //updatam next hopul
						next[j][i] = next[j][k];
					} 
					if(dist[i][j] == dist[i][k] + dist[k][j] && next[i][j] > next[i][k]) { //daca distantele sunt egale se alege ca next hop routerul cu id minim
						next[i][j] = next[i][k]; 
						next[j][i] = next[k][i];
					}
				}
			}
		}
		

		for(int j = 0; j < N; j++){
			routers[r].tabela[j] = next[r][j];
		} 
}


void update_routing_table() {

	for(int r = 0; r < N; r++){

		vector< vector<int> > dist;//matricea costurilor
		vector< vector<int> > next;//matricea hopurilor

		//initial matricea costurilor va avea costurile din topologie
		//iar matricea hopurilor va avea vecinii
		for(int k = 0; k < N; k++) { 
			for (int i = 0; i < N; i++) { 
				vector<int> d; 
				vector<int> h;

				for (int j = 0; j < N; j++) { 
					if (routers[k].tpl[i][j] == DRUMAX || i == j) { 
						d.push_back(50000); 
						h.push_back(-1); 
					} 
					else { 
						d.push_back(routers[k].tpl[i][j]); 
						h.push_back(j); 
					} 
				}

				dist.push_back(d); 
				next.push_back(h); 
			}
		}

		//aplicam algoritmul lui Floyd Warshall pentru routerul r
		Floyd_Warshall(r, dist, next);

	}
}
