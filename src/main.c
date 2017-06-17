#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>

typedef struct _Node {
    char name[32];
    uint8_t numConnectsTo;
    char connectsTo[8][32];
    time_t timeStamp;
} Node;

Node thisNode;
pthread_t thId;

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

            if (!strcmp(token, "NodeName")) {
                strcpy(thisNode.name, token_val);
            }
            else if (!strcmp(token, "ConnectsTo")) {
                tokens = str_split(token_val, ',');
                if (tokens)
                {
                    int i;
                    for (i = 0; *(tokens + i); i++)
                    {
                        strcpy(thisNode.connectsTo[i], *(tokens + i));
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
    printf("NodeName: %s \n", node->name);
    printf("ConnectsTo: ");
    for (i = 0; i < node->numConnectsTo; i++)
        printf ("%s ", node->connectsTo[i]);
    printf("\n");
}

int insertNode(Node *node)
{
    // TODO: Fetch the graph and insert new Node. Also update timeStamp.
    return 0;
}

void* listenerThread (void *args)
{
    char choice = 0;
    while (1) {
        printf("(a) Find Shortest Path \t (b) Find Mimimum Spanning Tree \nEnter a choice: ");
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
            default:
                printf("Invalid Choice: %c\n", choice);
        }
    }
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("Usage: %s <input-file>\n", argv[0]);
        return -1;
    }
    memset(&thisNode, 0, sizeof(Node));

    if (parseFile(argv[1])) {
        return -1;
    }
    printNode(&thisNode);

    insertNode(&thisNode);

    pthread_create(&thId, NULL, listenerThread, NULL);
    pthread_join(thId, NULL);

    printf("OK\n");
}
