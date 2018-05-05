#include<stdlib.h>
#include<stdio.h>
#include<math.h>
/**/
typedef struct node *link;
typedef struct vertice{
    int id;
    link head; 
    int d;
    int low;
    int in_stack;
    int scc_id;
} Vertice;

struct node{
    int v;
    link next;
};

typedef struct{
    int orig,dest;
}ponte;

Vertice **graph;
ponte *bridges;

int visited = 0;
link Stack=NULL;
int scc_counter = 0;
int m;
int  bridge_counter = 0;


Vertice* new_Vertice(int id,int i){
    Vertice *v ;
    v = malloc(sizeof(Vertice));
    v->id = id;
    v->in_stack = 0;
    v->d=-1;
    v->head=NULL;
    v->scc_id=0;  
    return v;
}

void free_Vertice(Vertice* vertice){
    free(vertice);
}

link InsertBegin(Vertice* adj, link head){
    link x = malloc(sizeof(struct node));
    x->v =adj->id;
    x->next = head;
    return x;
}

void add_arch(Vertice *curr,Vertice *adj){
    curr->head = InsertBegin(adj,curr->head);
}

void Pop(){
    graph[Stack->v-1]->in_stack = 0;
    link tmp=Stack;
    Stack=Stack->next;
    free(tmp);
}

void freeList(link h){
    link tmp;
    while(h!=NULL){
        tmp=h;
        h=h->next;
        free(tmp);
    }
}

void freeGraph(){
    int i;
    for (i = 0;i< m;i++){
        freeList(graph[i]->head);
        if(graph[i] != NULL)
            free_Vertice(graph[i]);
    }
    free(graph);
}

int min(int a,int b){
    if(a<b)
        return a;  
    return b;
}

void tarjan_visit(Vertice *v){
    v->d=v->low=visited;
    visited++;
    Stack = InsertBegin(v,Stack);
    v->in_stack = 1;
    link l;
    for(l=v->head;l!=NULL;l=l->next){ 
        if(graph[l->v-1]->d==-1 || graph[l->v-1]->in_stack){
            if(graph[l->v-1]->d==-1 )
                tarjan_visit(graph[l->v-1]); 
        v->low=min(v->low,graph[l->v-1]->low);
        }
    }
    if (v->d==v->low){
        scc_counter++;
        int i;
        i=0;
        int min;
        while(Stack->v!=v->id){
            graph[Stack->v-1]->scc_id = -1;
            Pop();
        }
    
        graph[Stack->v-1]->scc_id =-1;
        Pop();

        i = 0;
        min = -1;
        for(;i<m;i++){
            if(graph[i]->scc_id == -1)
                if(min ==-1 ||min>graph[i]->id)
                    min= graph[i]->id;
        }
        i = 0;
        for(;i<m;i++)
            if(graph[i]->scc_id == -1)
                graph[i]->scc_id = min;
    }    
}

void scc_Tarjan(){
    int i;
    for(i=0;i<m;i++)
        if(graph[i]->d == -1)
           tarjan_visit(graph[i]); 
}

int comparator(const void* a, const void* b){
    ponte *a1 = (ponte*) a;
    ponte *b1 = (ponte*) b;

    if(a1->orig == 0) return 1;
    if(b1->orig == 0) return -1;
    return (a1->orig == b1->orig) ? a1->dest-b1->dest : a1->orig -b1->orig;
}

int main(){
    int n;
    scanf("%d",&m);
    scanf("%d",&n);

    graph = (Vertice**) malloc(sizeof(Vertice*)*m);

    int i;
    for( i= 0;i<m;i++) 
        graph[i] = NULL;

    int orig,dest,j = 0;  
    for(i=0;i<m;i++)
        graph[i] = new_Vertice(i+1,i);
    
    while(j++<n){    
        scanf("%d",&orig);    
        scanf("%d",&dest);
        add_arch(graph[orig-1],graph[dest-1]);
    }    

    scc_Tarjan();
    printf("%d\n",scc_counter);

    if(scc_counter == 1){
        printf("0\n");
    }else{
        bridges = (ponte*) malloc(sizeof(ponte)*n);
        for(i=0;i<n;i++){
            bridges[i].orig = 0;
            bridges[i].dest = 0;
        }

        link head;
        for(j=0;j<m;j++)
            for(head = graph[j]->head; head != NULL; head = head->next)
                if(graph[j]->scc_id != graph[head->v-1]->scc_id){
                    bridges[bridge_counter].orig = graph[j]->scc_id;
                    bridges[bridge_counter++].dest = graph[head->v-1]->scc_id;
                }        

        qsort(bridges,bridge_counter+1,sizeof(ponte),comparator);

        for(i=0,j=1;j<bridge_counter;j++)
            if(!(bridges[i].orig == bridges[j].orig && bridges[i].dest == bridges[j].dest))
                bridges[++i]= bridges[j]; 

        printf("%d\n", ++i); 

        for(j=0;j<i;j++)
            printf("%d %d\n",bridges[j].orig,bridges[j].dest); 
    }
    free(bridges);
    freeGraph();
    return 0;
}