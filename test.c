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


int getNeighbour(int x, int y, neighbourhood* n);
int push(node_heap* nh, int x, int y);
int pop(node_heap* nh, int* x, int* y);
int print_difficulty();
int djik (int x, int y, int ifdigger);//takes the PC position (x,y and ifdigger)

int main(int argc, char* argv[])
{
	srand(time(0));
	printf("Hello World!!\n");

	int i,j,k, x, y;
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

	for (i = 0; i < ylenMax; i++)
	{
		for (j = 0; j < xlenMax; j++)
		{
			hardness[j][i] = 1 + (rand() % 254);
		}
	}

	for (i = 0; i < ylenMax; i++){
		for(j = 0; j < xlenMax; j++) grid[i][j] = '.';
	}

	//placing the PC at 20,10

	//first we initialize a node_heap
	node_heap newH;
	newH.size = 0;
	newH.tail = NULL;
	newH.head = NULL;

	push (&newH, 20, 10);
	difficulty[20][10] = 0;

	while(newH.size > 0)
	{
		pop(&newH, &i, &j);
		neighbourhood currN;
		getNeighbour(i, j, &currN);
		int init_diff = difficulty[i][j];
		int diff_curr_block;
		if (grid[i][j]== ' ') diff_curr_block = 1 + (hardness[i][j]/85);
		else diff_curr_block = 1;

		for (k = 0; k < currN.size; k++){
			x = currN.store[k][0];
			y = currN.store[k][1];

			if (grid[x][y] != ' ')
			{
				//check to see if it is already on the processed stack
				if (difficulty[x][y] > (init_diff + diff_curr_block))
				{
					difficulty[x][y] = init_diff + 1;
					push(&newH, x, y);
				}
			}

		}
		//add the processed i,j on the processed stack

	}

	print_difficulty();

	printf("\n\n");
	j = 43;
	i = j%10;
	printf("%c\n", (char)('0' + i));










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

	return 0;

}
int pop(node_heap* nh, int* x, int* y)
{
	if (nh->size == 1)
	{
		*x = nh->tail->xcoor;
		*y = nh->tail->ycoor;
		free(nh->tail);
	}
	else
	{
		node* temp;
		*x = nh->tail->xcoor;
		*y = nh->tail->ycoor;
		temp = nh->tail;
		nh->tail = nh->tail->next;
		nh->tail->prev = NULL;
		free(temp);

	}
	nh->size -= 1;
	return 0;

}
int print_difficulty(){
	int i, j;
	for (i = 0; i < ylenMax; i++)
	{
		for (j = 0; j < xlenMax; j++)
		{
			if (difficulty[j][i] == INT_MAX) printf(" ");
			else printf("%c", (char) ('0' + (difficulty[j][i]%10)));
		}
		printf("\n");
	}
	printf("\n\n");
	for (i = 0; i < ylenMax; i++)
	{
		for (j = 0; j < xlenMax; j++)
		{
			//if (difficulty[j][i] == INT_MAX) printf(" ");
			//else printf("%c", (char) ('0' + (difficulty[j][i]%10)));
			printf("%02x", hardness[j][i]);
		}
		printf("\n");
	}


	return 0;
}
