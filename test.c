#include <stdio.h>
#include <stdlib.h>
#include <endian.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <limits.h>

#define xlenMax 78
#define ylenMax 19

char grid[xlenMax][ylenMax];
int hardness[xlenMax][ylenMax];

int difficulty[xlenMax][ylenMax];

typedef struct node {
	int xcoor;
	int ycoor;
	struct node* prev;
	struct node* next;
} node;

typedef struct node_heap {
	node* head;
	node* tail;
	int size;
} node_heap;

typedef struct neighbourhood{
int store[8][2];
int size;
}neighbourhood;


int main(int argc, char* argv[])
{
	srand(time(0));
	printf("Hello World!!\n");

	int i,j,k;
/*
	FILE *f;

	for (i = 1; i < argc; i++)
	{
			if (!(strcmp(argv[i], "--save")))
			{
				j = 1;
				break;
			}

	}

	if (j==1)
	{
		printf("save found\n");
	}
	else
	{
		printf("save not found\n");
	}

	int hardness[xlenMax][ylenMax];

	//populating the hardness randomly
	for (i = 0; i < ylenMax; i++)
	{
		for (j = 0; j < xlenMax; j++)
		{
			hardness[j][i] = 1 + (rand() % 254);
			printf("%d ", hardness[j][i]);
		}
		printf("\n");
	}

	fopen("binary_file", "w");

	if( !( f = fopen( "binary_file", "w"))) {fprintf(stderr, "Failed to open file"); return 1;}

	char* marker = "RLG327-S2021";
	fwrite(marker, sizeof(char), 12, f);

	uint32_t version = 1;
	version = htobe32(version);
	fwrite(&version, sizeof(version), 1, f);



	u_int8_t xPCpos = 0;
	u_int8_t yPCpos = 0;
	for (i = 0; i < ylenMax; i++)
	{
		k = 0;
		for (j = 0; j < xlenMax; j++)
		{
			if (hardness[j][i] < 10)//if (grid[j][i] == '.')
			{
				xPCpos = j+1;
				yPCpos = i+1;
				k=1;
				break;
			}

		}
		if (k) break;
	}

	fwrite(&xPCpos, sizeof(u_int8_t), 1, f);
	fwrite(&yPCpos, sizeof(u_int8_t), 1, f);

	for (i = 0; i < ylenMax; i++)
	{
		for (j = 0; j < xlenMax; j++)
		{
			int8_t write_value = (hardness[i][j]);
			fwrite(&write_value,sizeof(int8_t),1,f);
		}
	}

	fclose(f);
	//FILE *f;
	//if( !( f = fopen( "boo", "r"))) {printf("Failed to open file\n"); return 1;}
	//fclose(f);

	char *home = getenv("HOME");
	char *gamedir = ".rlg327";
	char *savefile = "dungeon";
	char *path = malloc(strlen(home) + strlen(gamedir) + strlen(savefile) + 2 + 1);
	sprintf(path, "%s/%s/%s", home, gamedir, savefile);

	printf("%s\n", path);
	free(path);

	i = 0;
	j = 5;
	if (!(j=i)) printf("successful!\n");
*/

	i = INT_MAX;
	printf("%d\n", i);

	for (i = 0; i < ylenMax; i++){
		for(j = 0; j < xlenMax; j++) difficulty[j][i] = INT_MAX;
	}

	for (i = 0; i < ylenMax; i++){
		for(j = 0; j < xlenMax; j++) grid[i][j] = '.';
	}

	//placing the PC at 2020










	return 0;

}
int getNeighbour(int x, int y, neighbourhood* n){
	int i,j;
	int size = 0;
	int store[8][2]; //= n->store;
	//we start from the left and the move in the clockwise fashion
	if (x > 0) {
		store[size][0] = x - 1;
		store[size][1] = y;
		size++;
	}
	if ( (x > 0) && (y > 0) ){
		store[size][0] = x - 1;
		store[size][1] = y - 1;
		size++;
	}
	if (y > 0) {
		store[size][0] = x;
		store[size][1] = y - 1;
		size++;
	}
	if ( (x < (xlenMax - 1)) && (y > 0) ){
		store[size][0] = x + 1;
		store[size][1] = y - 1;
		size++;
	}
	if (x < (xlenMax - 1)) {
		store[size][0] = x + 1;
		store[size][1] = y;
		size++;
	}
	if ( (x < (xlenMax - 1)) && (y < (ylenMax - 1)) ){
		store[size][0] = x + 1;
		store[size][1] = y + 1;
		size++;
	}
	if (y < (ylenMax - 1)) {
		store[size][0] = x;
		store[size][1] = y + 1;
		size++;
	}
	if ( (x > 0) && (y < (ylenMax - 1)) ){
		store[size][0] = x - 1;
		store[size][1] = y + 1;
		size++;
	}

//finally getting the copy in neighbourhood
	for (i = 0; i < 2; i++)
	{
		for (j = 0; j < size; j++){
			n->store[j][i] = store[j][i];
		}
	}
	n->size = size;

	return 0;
}
int push(node_heap* nh, int x, int y)
{
	node *newNode = malloc(sizeof(node));
	newNode->xcoor = x;
	newNode->ycoor = y;
	newNode->next = NULL;

	if (!(nh->size)){
		newNode->prev = NULL;
		nh->tail = newNode;
		nh->head = newNode;
		nh->size++;
	}
	else{
		nh->head->next = newNode;
		newNode->prev = (nh->head);
		nh->head = newNode;
		nh->size++;
	}

}
