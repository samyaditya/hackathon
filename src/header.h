#ifndef _HEADER_H_
#define _HEADER_H_

#include <stdint.h>

struct AdjListNode
{
    int dest;
    struct AdjListNode* next;
};
 
struct AdjList
{
    struct AdjListNode *head;
};
 
struct Graph
{
    int V;
    struct AdjList* array;
};
 
struct AdjListNode* newAdjListNode(int dest);
struct Graph* createGraph(int size);
void addEdge(struct Graph* graph, int src, int dest);
void printGraph(struct Graph* graph);

#endif
