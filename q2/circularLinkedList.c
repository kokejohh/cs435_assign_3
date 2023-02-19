#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct hnode {
	int client;
	int length;
	char *line;
	struct hnode* next;
} hnode;

hnode *head = NULL, *tail = NULL;

void addAtFront(int client, int length, char *line)
{
	hnode *tmp = (hnode*)malloc(sizeof(hnode));

	tmp->client = client;
	tmp->length = length;
	tmp->line = strdup(line);
	if (tail == NULL)
	{
		tmp->next = tmp;
		head = tail = tmp;
	}
	else
	{
		tmp->next = tail->next;
		head = tail->next = tmp;
	}
}

void addAtLast(int client, int length, char *line)
{
	hnode *tmp = (hnode *)malloc(sizeof(hnode));
	
	tmp->client = client;
	tmp->length = length;
	tmp->line = strdup(line);
	if (tail == NULL)
	{
		tmp->next = tmp;
		head = tail = tmp;
	}
	else
	{
		head = tmp->next = tail->next;
		tail->next = tmp;
		tail = tmp;
	}
}

void deleteFirst()
{
	if (tail == NULL) printf("\nList is empty.\n");
	else
	{
		hnode *tmp = tail->next;
		head = tail->next = tmp->next;
		free(tmp);
	}
}

void deleteLast()
{
	if (tail == NULL) printf("\nList is empty\n");
	else
	{
		hnode *tmp = tail;
		while (tmp->next != tmp)
			tmp = tmp->next;
		head = tmp->next = tail->next;
		tail = tmp->next;
		free(tmp);	
	}
}

void viewList()
{
	if (tail == NULL) printf("\nList i empty\n");
	else
	{
		printf("History message :\n");
		hnode *tmp = tail->next;
		do
		{
			printf("cli-%03d(%d) says: %s", tmp->client, tmp->length, tmp->line);
			tmp = tmp->next;
		} while (tmp != tail->next);
	}
}

void test()
{
	printf("test\n");
}
