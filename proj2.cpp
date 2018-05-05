#include<stdlib.h>
#include<stdio.h>
#include<math.h>
#include<limits.h>
#include <list>
#include<iostream>

using namespace std;

typedef struct{
    //nome do vertice de destino
    int dest;
    // capacidade da aresta
    int capacity;
} Edge;

typedef struct{
    // capacidade da aresta
    int capacity;
    // fluxo da aresta
    int flux;
    //
}VirtualEdge;

typedef struct{
    //tipo de pixel 0 se for de primeiro plano e 1 se for de cenario
    int pixel;
    //pixel plano de fundo
    int c;
    //pixel primeiro plano
    int p;
    //array de vizinhos (cima,baixo,esquerda,direita)
    Edge *neig;
    //arestas que foi posta la para o algoritmo funcionar
    VirtualEdge source;
    VirtualEdge target;
}Vertex;

//Grafo dado como input
Vertex* G;
//numero de linhas dado como input
int m;
//numero de colunas dado como input
int n;

int flowMax = 0;

/*aloca a memoria para os 2 grafos necessarios 
  para a implementacao do algoritmo de dinic
*/
void initGraphs(){
    G = (Vertex*) malloc(sizeof(Vertex)*m*n);
    for(int i =0;i<m*n;i++)
    	G[i].neig = (Edge*) malloc(sizeof(Edge)*4);
}

void freeGraphs(){
	for(int i =0;i<m*n;i++)
    	free(G[i].neig);
    free(G);
}

void readGraph(){ //so esta a funcionar para um grafo agora TODO
    int i,j,k = 0;
    //le a primeira matriz
    for(i = 0;i<m;i++)
        for(j=0;j<n;j++){
            scanf("%d",&G[k].p);
            G[k].source.capacity = G[k].p;

            G[k].neig[1].dest = k+n; // estou a somar uma coluna inteira para ter o elemento a baixo
            G[k].neig[0].dest = k-n; // estou a subtrair uma coluna inteira para ter o elemento a cima

            if(i==0){ //caso da primeira linha
                G[k].neig[0].capacity = 0; 
                G[k].neig[0].dest = -1;
                
            }if(i == m-1){
                G[k].neig[1].capacity = 0; 
                G[k].neig[1].dest = -1;  
            }
            k++;        
        }
    //le a segunda matriz
    int fluxo;
    k = 0;
    for(i = 0;i<m;i++)
        for(j=0;j<n;j++){
            scanf("%d",&G[k].c);

            G[k].target.capacity = G[k].c;
            if(G[k].c < G[k].p){
                fluxo = G[k].c;
            }else{
                fluxo = G[k].p;
            }    

            G[k].neig[2].dest = k-1; // estou a subtrair uma coluna inteira para ter o elemento a cima
            G[k].neig[3].dest = k+1; // estou a somar uma coluna inteira para ter o elemento a baixo

            if(j==0){ //caso da primeira coluna
                G[k].neig[2].capacity = 0; //mudar para infinito TODO
                G[k].neig[2].dest = -1;
            }if(j == n-1){
                G[k].neig[3].capacity = 0; //mudar para infinito TODO
                G[k].neig[3].dest = -1;
            }
	
	        flowMax += fluxo;
	        G[k].target.flux = fluxo;
	        G[k].source.flux = fluxo;
	        k++;
    }
    //le os pesos dos arcos horizontais
    k=0;
    for(i=0;i<m;i++,k++)
        for(j=0;j<n-1;j++){
            scanf("%d",&G[k].neig[3].capacity);
            G[k+1].neig[2].capacity = G[k].neig[3].capacity;
            k++;
        }
    //le os pesos dos arcos verticais
    k=0;
    for(i=0;i<m-1;i++)
        for(j=0;j<n;j++){
            scanf("%d",&G[k].neig[1].capacity);
            G[k+n].neig[0].capacity = G[k].neig[1].capacity;
            k++;   
    }
}

int* pi;
list<int> *augPath;

/*transforma o resultado da BFS num caminho como deve ser*/
void correct_path(){
    augPath = new list<int>();
    int i = m*n+1;
    printf("BFS result\n");
    while(pi[i] != -1){
    	printf("%d\n",pi[i]);
    	augPath->push_front(i);
    	i = pi[i];
    }
}

void BFS(){

    list<int> augPath;
    bool *visited;
    visited = (bool*) malloc(sizeof(bool)*(m*n+2));
    
    int i,s;
    //inicializa tudo a false
    for(i = 0;i<m*n+2;i++){
        visited[i] = false;
        pi[i] = -1;
    }

    list<int> queue;
    //"visita" a source "" porque o vertice source nao chega a existir
    visited[m*n] = true;
    queue.push_back(m*n);
    while(!queue.empty()){

        s = queue.front();
        queue.pop_front();
        /*se o vertice que estamos a falar for a source entao 
        pode ir para todos os outros de ]0,m*n[*/
        if(s == m*n){
            for(i = 0;i<m*n;i++){
                if((!visited[i]) && G[i].source.capacity > G[i].source.flux){
                    visited[i] = true;
                    pi[i] = s;
                    queue.push_back(i);
                }                
            }
        }
        //como o target nao e bem um vertice e nao tem vizinhos nao e preciso fazer isto
        else{
            //para o target
            if((!visited[m*n+1]) && G[s].target.capacity > G[s].target.flux){
                pi[m*n+1] = s;
                break;
            }

            //para todos os vertices que nao sejam o target
            for(i=0;i<4;i++){
                if(G[s].neig[i].dest != -1 && (!visited[G[s].neig[i].dest]) && G[s].neig[i].capacity != 0){
                    visited[G[s].neig[i].dest] = true;
                    pi[G[s].neig[i].dest] = s;
                    queue.push_back(G[s].neig[i].dest);
                }
            }
        }
    }
    free(visited);
    correct_path();
}

void augmentFlow(){
	/*encontra o minimo*/
	int i,min;
	list<int>::iterator curr;
	list<int>::iterator next;
	curr = augPath->begin();
	next = ++augPath->begin();

	min = G[*curr].source.capacity - G[*curr].source.flux;

	for(; *next != augPath->back(); curr++, next++){
		for (i=0; i<4; i++){
			if(*next == G[*curr].neig[i].dest){
				if(G[*curr].neig[i].capacity <min)
					min = G[*curr].neig[i].capacity;
			}
		}		
	}
	int fluxo = G[*curr].target.capacity - G[*curr].target.flux;
	if(fluxo < min)
		min = fluxo;

	/*aumenta o fluxo*/
    curr = augPath->begin();
    next = ++augPath->begin();

    for(;*next != augPath->back(); ++curr,++next){
    	if(curr == augPath->begin())
    		G[*curr].source.flux += min;

        for(i=0;i<4;i++){
            if(G[*curr].neig[i].dest == *next){
                G[*curr].neig[i].capacity -=min;
            }
            if(G[*next].neig[i].dest == *curr){
            	G[*next].neig[i].capacity +=min;
            }
        }
    }
    G[*curr].target.flux += min;
    flowMax += min;	    
}

void edmondsKarp(){	
	/*fluxo de aumento que se deve acrecentar em cada iteracao*/
	while(true){
		BFS();

		if(augPath->back() != m*n+1)
			break;
		else{
			augmentFlow();
		}
	}
}

void pixelType(){
    for(int i = 0;i<m*n;i++){
    	if(pi[i] == -1)
    		G[i].pixel = 0; // e de primeiro plano
    	else{
    		G[i].pixel = 1; // e de cenario
    	}
    }
}

void printOutput(){

    pixelType();

	printf("%d\n\n",flowMax);
	for(int i = 0;i<m*n;i++){
        if(G[i].pixel==0)
			printf("P ");
		else
			printf("C ");

        if((i+1)%n == 0 || n == 1)
        	printf("\n");
    }
}

int main(){
    scanf("%d %d",&m, &n);
    initGraphs();

    pi = (int*) malloc(sizeof(int)*(m*n+2));

    readGraph();

    edmondsKarp();

	printOutput();

    freeGraphs();   

    free(pi); 

    return 0;
}
