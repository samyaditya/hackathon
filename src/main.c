#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "header.h"

typedef struct _Node {
    int vertex;
    uint8_t numConnectsTo;
    int connectsTo[16];
    time_t timeStamp;
} Node;

typedef struct _NodeList {
    uint8_t numNodes;
    Node node[16];
} NodeList;

Node thisNode;
NodeList gNodeList;
pthread_t thId;
sem_t semvar;
pthread_mutex_t nodeListLock;

char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    count += last_comma < (a_str + strlen(a_str) - 1);

    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        *(result + idx) = 0;
    }

    return result;
}

int parseFile(char *file)
{
    FILE *readPtr = NULL;
    char readLine[200] = "";
    char *status = NULL;
    char *token = NULL, **tokens;
    char *token_val = NULL;

    if ((readPtr = fopen(file, "r")) == NULL) {
        printf("File %s not found\n", file);
        return -1;
    }
    memset(readLine, 0, sizeof(readLine));

    while ((status = fgets(readLine, sizeof(readLine), readPtr)) != NULL) {
        if (readLine[0] != '#' && readLine[0] != ';' && readLine[0] != ' ') {
            token = strtok(readLine, "=");
            token_val = strtok(NULL, "\r\n");
            if (!token || !token_val)
                continue;

            if (!strcmp(token, "Node")) {
                thisNode.vertex = atoi(token_val);
            }
            else if (!strcmp(token, "ConnectsTo")) {
                tokens = str_split(token_val, ',');
                if (tokens)
                {
                    int i;
                    for (i = 0; *(tokens + i); i++)
                    {
                        thisNode.connectsTo[i] = atoi(*(tokens + i));
                        thisNode.numConnectsTo++;
                        free(*(tokens + i));
                    }
                    free(tokens);
                }
            }
        }
    }

    return 0;
}

void printNode(Node *node)
{
    int i;
    printf("Node: %d \n", node->vertex);
    printf("ConnectsTo: ");
    for (i = 0; i < node->numConnectsTo; i++)
        printf ("%d ", node->connectsTo[i]);
    printf("\n");
}

int insertNode(Node *node)
{
    // Update timeStamp and Insert Node into gNodeList.
    node->timeStamp = time(NULL);
    int i;
    for (i = 0; i < gNodeList.numNodes; i++) {
        if (gNodeList.node[i].vertex == node->vertex) {
            memcpy(&gNodeList.node[i], node, sizeof(Node));
            break;
        }
    }
    if (i == gNodeList.numNodes) {
        memcpy(&gNodeList.node[i], node, sizeof(Node));
        gNodeList.numNodes++;
    }
    return 0;
}

void validateList()
{
    int i, j;
    for (i = 0; i < gNodeList.numNodes; i++) {
        if (time(NULL) > (gNodeList.node[i].timeStamp + 5)) {
            // Node has expired. That mean process which created it is dead.
            for (j = i; j < gNodeList.numNodes - 1; j++) {
                gNodeList.node[j] = gNodeList.node[j + 1];
            }
            gNodeList.numNodes--;
            i--;
        }
    }
}

void printList()
{
    int i, j;
    printf("\n =============== Graph ===============\n");
    for (i = 0; i < gNodeList.numNodes; i++) {
        printf(" Node %d is connecting to ",gNodeList.node[i].vertex);
        for (j = 0; j < gNodeList.node[i].numConnectsTo; j++) {
             printf(" %d", gNodeList.node[i].connectsTo[j]);
             if((j+1) < gNodeList.node[i].numConnectsTo)
                 printf(", ");
        }
        printf("\n");
    }
    printf("\n ================ END ================\n");
}

struct Graph* createGraphFromNodes(void)
{
    int i, j;
    struct Graph *graph = createGraph(gNodeList.numNodes);

    for (i = 0; i < gNodeList.numNodes; i++) {
        for (j = 0; j < gNodeList.node[i].numConnectsTo; j++) {
            addEdge(graph, gNodeList.node[i].vertex, gNodeList.node[i].connectsTo[j]);
        }
    }
    return graph;
}

void* listenerThread (void *args)
{
    char choice = 0;
    int i = 0;
    while (1) {
        printf("(a) Find Shortest Path \t (b) Find Mimimum Spanning Tree \t (c) Show graph\nEnter a choice: ");
        scanf(" %c", &choice);

        switch(choice) {
            case 'a':
                printf("Calling a\n");
                // TODO
                break;
            case 'b':
                printf("Calling b\n");
                // TODO
                break;
            case 'c':
                pthread_mutex_lock(&nodeListLock);
                validateList();
                struct Graph *graph = createGraphFromNodes();
                //printGraph(graph);
                printList();
                pthread_mutex_unlock(&nodeListLock);
                break;
            default:
                printf("Invalid Choice: %c\n", choice);
        }
    }
}

int main(int argc, char **argv)
{
    FILE *fp = NULL;
    if (argc != 2) {
        printf("Usage: %s <input-file>\n", argv[0]);
        return -1;
    }
    memset(&thisNode, 0, sizeof(Node));

    if (parseFile(argv[1])) {
        return -1;
    }

    pthread_create(&thId, NULL, listenerThread, NULL);

    struct sembuf sb = {0,-1,0};
    key_t key=ftok(".",'a');
    if(key == -1 ) {
        printf("\n\n Initialization Falied of shared memory \n\n");
        return 1;
    }
    int semid=shmget(key, 1024,IPC_CREAT|0744);
    if(semid == -1 ) {
        printf("\n\n Error captured while share memory allocation\n\n");
        return 1;
    }
    //uint8_t *data=(uint8_t *)shmat(semid,(void *)0,0);

    while(1) {
        memset(&gNodeList, 0, sizeof(gNodeList));
        // fetch gNodeList (lock bin file and binary read)
        sb.sem_op = -1; //Lock
        semop(semid,(struct sembuf *)&sb,1);
        pthread_mutex_lock(&nodeListLock);
        fp = fopen("/tmp/sembin", "r");
        if (fp == NULL)
            perror("Fail to open bin file");
        else {
            fread(&gNodeList, sizeof(gNodeList), 1, fp);
            fclose(fp);
        }
        insertNode(&thisNode);  // into gNodeList
        fp = fopen("/tmp/sembin", "w");
        // Pushback updated gNodeList (binary write and unlock bin file)
        fwrite(&gNodeList, sizeof(gNodeList), 1, fp);
        fclose(fp);
        pthread_mutex_unlock(&nodeListLock);
        sb.sem_op = 1;//Unlock
        semop(semid,(struct sembuf *)&sb,1);
        sleep(2);
    }

    pthread_join(thId, NULL);

    printf("OK\n");
}
