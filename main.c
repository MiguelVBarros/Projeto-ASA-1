#include <stdio.h>
#include <stdlib.h>

#define MIN(a,b) ((a) < (b) ? a : b)
#define TRUE 1
#define FALSE 0
#define INFINITY -1

/*estruturas*/

typedef struct listNode *link;
typedef struct vertex vertice;
typedef struct edge Edge;
typedef struct stacknode *linkStacknode;

/*lista simplesmente ligada*/
struct listNode {
	struct vertex* v;
	struct listNode *next;
};


/*vertice de um grafo*/
struct vertex{
	int id;
	char inStack;
	int d, low;
	int* idMinSCC;
	/*lista dos vertices adjacentes ao vertice*/
	struct listNode* arcos_lista;
};


/*arco do grafo*/
struct edge{
	int origem;
	int destino;
};

/*grafo (representado por uma lista de adjacencias)*/
typedef struct graph {
	struct vertex* vertices;
	int numberOfVertexes;
	int numberOfArcs;
	int number_of_components;
	int visited;
	link stack;
}graph;



/*adiciona um elemento a uma lista na primeira posicao*/
link add2list(link head, struct vertex *e) {
	link newNode = malloc(sizeof (link));
	newNode->v = e;
	newNode->next = head;
	return newNode;
}

/*faz push para uma list que e um stack*/
void push (link *head, struct vertex* v) {

	*head = add2list(*head, v);
	v->inStack = TRUE;	
}

/*faz pop  de um stack (retira o primeiro elemento da lista)*/
struct vertex* pop (link *head) {
	link aux = *head;
	struct vertex* value = aux->v;
	value->inStack = FALSE;
	*head = (*head)->next;
	return value;
}


/*compara duas edge (definidas pela origem e pelo destino da edge)
se tem origens diferentes entao a menor e a que tem a menor origem
se tem origens iguais entao a menor e a que tem a maior origem*/
int compare (const void * a, const void * b){
  Edge *edgeA = (Edge *)a;
  Edge *edgeB = (Edge *)b;

  if (edgeA->origem == edgeB->origem) return edgeA->destino - edgeB->destino;
  
  else return edgeA->origem - edgeB->origem;
}


/*construtor do vertice*/
vertice createVertex(int id) {
	vertice new;
	new.id = id;
	new.inStack = FALSE;
	new.d = INFINITY;
	new.low = INFINITY;
	new.idMinSCC = NULL;
	new.arcos_lista = NULL;
	return new;
}

/*construtor do grafo*/
graph* initGraph (int size) {
	int i;
	graph* newGraph = malloc (sizeof (graph));
	newGraph->visited = 1;
	newGraph->vertices = (struct vertex*) malloc (sizeof(struct vertex) * size);
	newGraph->numberOfVertexes = size;
	newGraph->numberOfArcs = 0;
	newGraph->stack = NULL;
	for (i = 0; i < size; i++ )
		newGraph->vertices[i] = createVertex (i);
	return newGraph;
}

/*encontra arcos de ligacao entre SCCs e faz print delas*/
void findConnextions(graph* g) {
	int arrayBound = 0; /*conta o numero de arcos de ligacao entre SCCs*/
	int i;

	/*array que guarda todas os arcos de ligacao entre SCCs 
	(que no maximo sao todos os arcos do grafo)*/
	Edge array[g->numberOfArcs];

	link l;

	for (i = 0; i < g->numberOfVertexes; i++) 
		for (l = g->vertices[i].arcos_lista; l != NULL; l = l->next)
			if(*g->vertices[i].idMinSCC != *l->v->idMinSCC) {
				/*encontrou um arco de ligacao entre SCCs e vai po-lo no array*/
				Edge aux; 
				aux.origem = *g->vertices[i].idMinSCC;
				aux.destino = *l->v->idMinSCC;
				array[arrayBound] = aux;
				arrayBound++;
			}

	/*se nao encontrou arcos de ligacao*/
	if (arrayBound == 0){
		printf("%d\n%d\n", g->number_of_components, 0);
		return;
	}

	/*ordena o array, usando o criterio da funcao compare*/
	qsort(array, arrayBound, sizeof(Edge), compare);


	int conextions = arrayBound; /*conextions e o numero de ligacaoes entre SCCs (nao conta elementos repetidos)*/


	for (i = 1; i < arrayBound; i++)

		if(array[i].origem == array[i-1].origem && array[i].destino == array[i-1].destino)
			/*encontrou dois arcos iguais (como o array esta ordenado estao necessariamente em posicoes seguidas)*/
			conextions--;


	printf("%d\n%d\n%d %d\n", g->number_of_components, conextions, array[0].origem + 1, array[0].destino + 1);


	for (i = 1; i < arrayBound; i++)

		if(array[i].origem != array[i-1].origem || array[i].destino != array[i-1].destino)
			/*print dos arcos (elimina arcos repetidos como acima)*/
			printf("%d %d\n",array[i].origem+1, array[i].destino + 1 );
}


/*algoritmo tarjan para encontrar componentes fortemente ligadas*/
void tarjan_Visit (graph *g, int idVertice) {
	/*define a profundidade do vertice*/
	vertice* v = &g->vertices[idVertice];
	v->low = g->visited;
	v->d = g->visited;
	g->visited ++;
	push(&g->stack, v);
	link aux;

	/*considera todos os vertices sucessores ao vertice atual*/
	for (aux = v->arcos_lista; aux != NULL; aux = aux->next) {
		vertice* u = aux->v;
		if (u->d == INFINITY || u->inStack == 1) {
			if (u->d == INFINITY)
				/*sucessor ainda nao foi visitado*/
				tarjan_Visit(g, u->id);
			/*se chega aqui e porque o sucessor esta no stack 
			e portanto na SCC atual*/

			/*atualiza o valor de low do vertice atual para o valor minimo dos seus sucessores*/
			v->low =MIN(v->low, u->low);
		}
	}
	if (v->d == v->low) {
		/*v e a raiz de uma SCC*/
		vertice* w = NULL;
		int* indice_menor = malloc(sizeof(int));
		*indice_menor = INFINITY;
		g->number_of_components++;
		do {
			/*fazemos pop ate encontrarmos a raiz de uma SCC*/
			w = pop(&g->stack);
			w->idMinSCC = indice_menor;
			if (*indice_menor == INFINITY || w->id < *indice_menor)
				*indice_menor = w->id ;
		} while (w->id != v->id );
	}
}

/*encontra SCCs e ligacoes entre elas*/
void scc_Tarjan (graph *g) {
	int i;
	for (i = 0 ; i < g->numberOfVertexes; i++)
		if (g->vertices[i].d == INFINITY)
			tarjan_Visit(g, i);


	}

	
int main () {
	int n;
	scanf("%d", &n);

	graph* g = initGraph (n);
	int m;

	scanf("%d", &m);
	
	int u, v, i;
	for ( i = 0; i < m; i++) {
		/*adiciona os arcos escritos no stdin no grafo*/
		scanf ("%d %d", &u, &v);
		g->numberOfArcs++;
		g->vertices[u - 1].arcos_lista = add2list(g->vertices[u - 1].arcos_lista, &g->vertices[v - 1]);
	}

	scc_Tarjan(g);
	findConnextions(g);


	return 0;
}
