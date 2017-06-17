#include <stdio.h>
#include <stdlib.h>
#include "header.h"

struct AdjListNode* newAdjListNode(int dest)
{
    struct AdjListNode* newNode =
            (struct AdjListNode*) malloc(sizeof(struct AdjListNode));
    newNode->dest = dest;
    newNode->next = NULL;
    return newNode;
}
 
struct Graph* createGraph(int V)
{
    struct Graph* graph = (struct Graph*) malloc(sizeof(struct Graph));
    graph->V = V;
 
    graph->array = (struct AdjList*) malloc(V * sizeof(struct AdjList));
 
    int i;
    for (i = 0; i < V; ++i)
        graph->array[i].head = NULL;
 
    return graph;
}
 
void addEdge(struct Graph* graph, int src, int dest)
{
    struct AdjListNode* newNode = newAdjListNode(dest);
    newNode->next = graph->array[src].head;
    graph->array[src].head = newNode;
 
#if UNDIRECTIONAL
    newNode = newAdjListNode(src);
    newNode->next = graph->array[dest].head;
    graph->array[dest].head = newNode;
#endif
}
 
 
void printGraph(struct Graph* graph)
{
    int i;
    printf("\n =============== Graph ===============\n");
    printf("\n Alive Nodes           Adjecent Nodes\n");
    for (i = 0; i < graph->V; ++i)
    {
        struct AdjListNode* temp = graph->array[i].head;
        if (temp)
            printf("\n      %d                   ", i);
        while (temp)
        {
            printf("%d ", temp->dest);
            temp = temp->next;
        }
        printf("\n");
    }
    printf("\n =============== END ===============\n");
}
 
