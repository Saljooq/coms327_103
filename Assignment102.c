#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <endian.h>
#include <string.h>
#include <stdint.h>

#define MAX_ROOMS 12
#define MIN_ROOMS 6

#define xlenMax 78
#define ylenMax 19


#define minRoomxlen 4
#define minRoomylen 3

#define maxStairs 3
#define minStairs 1

/*the room is a struct to save the relevant coordinates and leghts of a room*/
typedef struct room
{
	int xloc;
	int yloc;
	int xlen;
	int ylen;
}room;

int makes_sense(room rooms[], int numRooms);
int not_so_rand_roomsize_resizer(int numRooms);

char grid[xlenMax][ylenMax];
int hardness[xlenMax][ylenMax];

int difficulty[xlenMax][ylenMax];
int difficulty_nt[xlenMax][ylenMax];

int main(int argc, char* argv[])
{

	int i, j, x, y, k; //we declare most of the common variables we'll be using late
	//the grid below will be use to store all the characters for dungeon
	//char grid[xlenMax][ylenMax];
	//int hardness[xlenMax][ylenMax];
	int numRooms, numUpstairs, numDownstairs;
	uint8_t xPCpos, yPCpos;

	room *rooms;

	//first we populate the grid with spaces
	for (i = 0; i < xlenMax; i++)
	{
		for (j = 0; j < ylenMax; j++)
		{
			grid[i][j] = ' ';
		}
	}

	//load method goes ther
	for (i = 1; i < argc; i++)
	{
			if (!(strcmp(argv[i], "--load")))
			{
				j = 1;
				break;
			}

	}

	if (j==1)
	{
		//printf("load found\n");

		FILE *f;

		char *home = getenv("HOME");
		char *gamedir = ".rlg327";
		char *savefile = "dungeon";
		char *path = malloc(strlen(home) + strlen(gamedir) + strlen(savefile) + 2 + 1);
		sprintf(path, "%s/%s/%s", home, gamedir, savefile);

		if( !( f = fopen( path, "r"))) {printf("Failed to open file\n"); return 1;}
		free(path);


		uint8_t temp8;
		uint16_t temp16;
		uint32_t temp32;

		char filetype[12];
		fread(filetype, sizeof(char), 12, f);

		//dealing with version
		fread(&temp32, sizeof(temp32), 1, f);

		//dealing with filesize - will need to adjust endian if to be used
		fread(&temp32, sizeof(temp32), 1, f);

		//dealing xPCpos and yPCpos
		fread(&xPCpos, sizeof(uint8_t), 1, f);
		fread(&yPCpos, sizeof(uint8_t), 1, f);

		//now we populate the dungeon matrix
		for (j = 0; j < xlenMax + 2; j++)
		{
			fread(&temp8, sizeof(uint8_t), 1, f);
		}

		for (i = 0; i < ylenMax; i++)
		{

			fread(&temp8, sizeof(uint8_t), 1, f);

			for (j = 0; j < xlenMax; j++)
			{
				fread(&temp8, sizeof(uint8_t), 1, f);
				hardness[j][i] = temp8;
			}

			fread(&temp8, sizeof(uint8_t), 1, f);

		}

		for (j = 0; j < xlenMax + 2; j++)
		{
			fread(&temp8, sizeof(uint8_t), 1, f);
		}
		//end of hardness reading

		//number of Rooms are entered here
		fread(&temp16, sizeof(uint16_t), 1, f);
		numRooms = be16toh(temp16);

		//next we fill in the matrix rooms
		rooms = malloc(numRooms * sizeof(room));

		//here we write the coordinates of the room
		for (i = 0; i < numRooms; i++)
		{
			fread(&temp8, sizeof(uint8_t), 1, f);
			rooms[i].xloc = temp8 - 1;

			fread(&temp8, sizeof(uint8_t), 1, f);
			rooms[i].yloc = temp8 - 1;

			fread(&temp8, sizeof(uint8_t), 1, f);
			rooms[i].xlen = temp8;

			fread(&temp8, sizeof(uint8_t), 1, f);
			rooms[i].ylen = temp8;

		}

		//next we populate the room area with the downstairs
		for (x = 0; x < numRooms; x++)
		{

			for (i = rooms[x].xloc; i < (rooms[x].xloc + rooms[x].xlen); i++)
			{
				for (j = rooms[x].yloc; j < (rooms[x].yloc + rooms[x].ylen); j++)
				{
					grid[i][j] = '.';
				}
			}
		}

		//next we deal with upstairs
		fread(&temp16, sizeof(uint16_t), 1, f);
		numUpstairs = be16toh(temp16);


		for (i = 0; i < numUpstairs; i++)
		{

			fread(&temp8, sizeof(uint8_t), 1, f);
			x = temp8 - 1;

			fread(&temp8, sizeof(uint8_t), 1, f);
			y = temp8 - 1;
			grid[x][y] = '<';

		}

		//next we deal with downstairs
		fread(&temp16, sizeof(uint16_t), 1, f);
		numDownstairs = be16toh(temp16);


		for (i = 0; i < numDownstairs; i++)
		{

			fread(&temp8, sizeof(uint8_t), 1, f);
			x = temp8 - 1;

			fread(&temp8, sizeof(uint8_t), 1, f);
			y = temp8 - 1;
			grid[x][y] = '>';

		}
		//next we populate the corridors wherever hardness is zero and there is no room or stairs (or in this case wherever there is space in the grid)
		for (i = 0; i < ylenMax; i++)
		{
			for (j = 0; j < xlenMax; j++)
			{
				if (hardness[j][i] == 0)
				{
					if (grid[j][i] == ' ') grid[j][i] = '#';
				}
			}
		}

		fclose(f);
		//printf("load found - final - no error while loading\n");



	}
	//this is where we start processing the dungron if the load was not found
	else
	{


		//we will start out by creating a seed with time-0 to access some randomeness
		srand(time(0));

		//populating the hardness randomly
		for (i = 0; i < ylenMax; i++)
		{
			for (j = 0; j < xlenMax; j++)
			{
				hardness[j][i] = 1 + (rand() % 254);
			}
		}

		numRooms = MIN_ROOMS + (rand() % (MAX_ROOMS - MIN_ROOMS + 1));

		rooms = malloc(numRooms * sizeof(room));

		int resizer = not_so_rand_roomsize_resizer(numRooms);//we use this function to obtain a denominator to limit the size of the rooms

		//the if conditions used to obtain the max length of the room help avoid the floating point exception (core dump) later when we use it with modulus later
		int maxRoomxlen = xlenMax / resizer;
		if (maxRoomxlen <= minRoomxlen) maxRoomxlen = minRoomxlen + 1;


		int maxRoomylen = ylenMax / resizer;
		if (maxRoomylen <= minRoomylen) maxRoomylen = minRoomylen + 1;

		//printf("num Rooms = %d\n", numRooms); //uncomment to see num of rooms generated

		//this loop keeps going till random coordinates and lengths are obtained from random function that make sense
		while (1)
		{
			for (i = 0; i < numRooms; i++)
			{
				rooms[i].xloc = rand() % xlenMax;
				rooms[i].yloc = rand() % ylenMax;
				rooms[i].xlen = minRoomxlen + rand() % ((maxRoomxlen) - minRoomxlen);
				rooms[i].ylen = minRoomylen + rand() % ((maxRoomylen) - minRoomylen);
			}
			if (makes_sense(rooms, numRooms)) break;
		}


		//Next we populate the grid with '.' as per the randomised coordinates that made sense that we obtained earlier
		for (x = 0; x < numRooms; x++)
		{

			for (i = rooms[x].xloc; i < (rooms[x].xloc + rooms[x].xlen); i++)
			{
				for (j = rooms[x].yloc; j < (rooms[x].yloc + rooms[x].ylen); j++)
				{
					grid[i][j] = '.';
					hardness[i][j] = 0;
				}
			}
		}

		//next we carve out a path between adjacent rooms in which we use the former's x coordinate and latter's y-coordinates to create a mid-point
		for (int x = 0; x < numRooms - 1; x++)
		{
			int middlex = rooms[x].xloc;
			int middley = rooms[x + 1].yloc;
			int i;//i will save the direction of the path

			if (rooms[x].yloc > middley) i = 1;
			else i = -1;

			//first we go from from midpoint to former room
			for ( j = middley; j != rooms[x].yloc; j += i)
			{
				if (grid[middlex][j] != '.')
				{
					grid[middlex][j] = '#';
					hardness[middlex][j] = 0;
				}
			}

			//then we go from midpoint to latter room
			if (rooms[x + 1].xloc > middlex) i = 1;
			else i = -1;

			for ( j = middlex; j != rooms[x + 1].xloc; j += i)
			{
				if (grid[j][middley] != '.')
				{
					grid[j][middley] = '#';
					hardness[j][middley] = 0;
				}
			}

		}


		//here we randomise the upwards and downward staircases and insert them wherever the random coordinates and its horizontal neighbours are part of room
		for (i = 0; i < 2; i++)
		{
			//first iteration adds random number of '<' to the grid, second adds '<'
			char staircase;
			if (i == 0) staircase = '<';
			else staircase = '>';

			int numStairs = minStairs + rand() % ((maxStairs) - minStairs);
			if (i == 0)
			{
				staircase = '<';
				numUpstairs = numStairs;

			}
			else
			{
				staircase = '>';
				numDownstairs = numStairs;
			}

			for (j = 0; j < numStairs; j++)
			{
				//while loops below keeps going till a successfuk coordinate is found
				while (1)
				{
					x = 1 + (rand() % (xlenMax - 2));//this ensures that we're not on the left or the right edge because the condition below checks horizontal neighbours
					y = (rand() % (ylenMax));

					if (grid[x][y] == '.' && grid[x - 1][y] == '.' && grid[x + 1][y] == '.')
					{
						grid[x][y] = staircase;
						break;
					}
				}


			}
		}

		//this is where we try to position PC on the floor, making sure there's floor there
		xPCpos = 0;
		yPCpos = 0;
		for (i = 0; i < ylenMax; i++)
		{
			k = 0;
			for (j = 0; j < xlenMax; j++)
			{
				if (grid[j][i] == '.')
				{
					xPCpos = j+1;
					yPCpos = i+1;
					k=1;
					break;
				}

			}
			if (k) break;
		}

	}
	//this is where processing of the dungeon ends

	//below is where we print out the actual grid

	char temp = grid[xPCpos - 1][yPCpos - 1];//we save the char where we want to place the PC
	grid[xPCpos - 1][yPCpos - 1] = '@';

	for (i = 0; i < xlenMax; i++) {printf("-");}
	printf("\n");

	for (i = 0; i < ylenMax; i++)
	{
		printf("|");
		for (j = 0; j < xlenMax; j++)
		{
			printf("%c", grid[j][i]);
		}
		printf("|\n");
	}

	for (i = 0; i < xlenMax; i++) {printf("-");}
	printf("\n\n\n");

	grid[xPCpos - 1][yPCpos - 1] = temp; //restoring the grid by replacing the PC

	//Now we check to see if there's a save switch to update the /.rlg327/dungeon
	j = 0;
	for (i = 1; i < argc; i++)
	{
			if (!(strcmp(argv[i], "--save")))
			{
				j = 1;
				break;
			}

	}
	//processing for save tags beings here
	if (j)
	{
		//printf("save found\n");
		FILE *f;

		char *home = getenv("HOME");
		char *gamedir = ".rlg327";
		char *savefile = "dungeon";
		char *path = malloc(strlen(home) + strlen(gamedir) + strlen(savefile) + 2 + 1);
		sprintf(path, "%s/%s/%s", home, gamedir, savefile);

		if( !( f = fopen( path, "w"))) {printf("Failed to open file\n"); return 1;}
		free(path);

		char* marker = "RLG327-S2021";
		fwrite(marker, sizeof(char), 12, f);

		uint32_t version = 0;
		version = htobe32(version);
		fwrite(&version, sizeof(uint32_t), 1, f);

		//calculate the size of the file, meanwhile the size is taken to be zero
		uint32_t size = 1708 + (4 * numRooms) + (2 * (numUpstairs + numDownstairs));
		size = htobe32(size);
		fwrite(&size, sizeof(uint32_t), 1, f);

		//now we enter position of the PC
		fwrite(&xPCpos, sizeof(uint8_t), 1, f);
		fwrite(&yPCpos, sizeof(uint8_t), 1, f);


		//next we write the dungeon matrix - we will have to artificially populate the file with max hardness on border
		uint8_t temp8;

		for (j = 0; j < xlenMax + 2; j++)
		{
			temp8 = 255;
			fwrite(&temp8, sizeof(uint8_t), 1, f);
		}

		for (i = 0; i < ylenMax; i++)
		{
			temp8 = 255;
			fwrite(&temp8, sizeof(uint8_t), 1, f);

			for (j = 0; j < xlenMax; j++)
			{
				temp8 = hardness[j][i];
				fwrite(&temp8, sizeof(uint8_t), 1, f);
			}

			temp8 = 255;
			fwrite(&temp8, sizeof(uint8_t), 1, f);

		}

		for (j = 0; j < xlenMax + 2; j++)
		{
			temp8 = 255;
			fwrite(&temp8, sizeof(uint8_t), 1, f);
		}

		//number of Rooms are entered here
		uint16_t temp16 = numRooms;
		temp16 = htobe16(temp16);

		fwrite(&temp16, sizeof(uint16_t), 1, f);

		//mext we write the coordinates of the room
		for (i = 0; i < numRooms; i++)
		{
			temp8 = 1 + rooms[i].xloc;
			fwrite(&temp8, sizeof(uint8_t), 1, f);

			temp8 = 1 + rooms[i].yloc;
			fwrite(&temp8, sizeof(uint8_t), 1, f);

			temp8 = rooms[i].xlen;
			fwrite(&temp8, sizeof(uint8_t), 1, f);

			temp8 = rooms[i].ylen;
			fwrite(&temp8, sizeof(uint8_t), 1, f);

		}

		//here we process the number of upstairs
		temp16 = numUpstairs;
		temp16 = htobe16(temp16);
		fwrite(&temp16, sizeof(uint16_t), 1, f);

		//here we enter the coordinates of upstairs
		for (j = 0; j < ylenMax; j++)
		{
			for (k = 0; k < xlenMax; k++)
			{
				if (grid[k][j] == '<')
				{
					temp8 = k + 1;
					fwrite(&temp8, sizeof(uint8_t), 1, f);


					temp8 = j + 1;
					fwrite(&temp8, sizeof(uint8_t), 1, f);

				}
			}
		}

		//here we process the number of downstairs
		temp16 = numDownstairs;
		temp16 = htobe16(temp16);
		fwrite(&temp16, sizeof(uint16_t), 1, f);

		//here we enter the coordinates of downstairs
		for (j = 0; j < ylenMax; j++)
		{
			for (k = 0; k < xlenMax; k++)
			{
				if (grid[k][j] == '>')
				{
					temp8 = k + 1;
					fwrite(&temp8, sizeof(uint8_t), 1, f);

					temp8 = j + 1;
					fwrite(&temp8, sizeof(uint8_t), 1, f);

				}
			}
		}


		fclose(f);
	}
	//processing for save ends here


	free(rooms);


	return 0;
}
/*The first is the makes_sense function that takes the array of rooms and number of Rooms as the argument.
It tries to see if the top edge of one room coincides with area occupied with all the other rooms.
This it does in four ways - first it check if the y coordinates of the top edge are inside
the range+1(1 is added to keep a gap of 1) of the other rooms. If a room is coincides on
 the y-coordinates, then we check the x-coordinates. This we do in three ways, by checking the left corner,
 the right corner and the middle. If any of these indicate intersection then the program ends soonafter because
 it 'doesnt make sense' so the random function will pick some other coordinates. This function also makes sure
 the rooms are within the grid we have and don't exceed.*/
int makes_sense(room rooms[], int numRooms)
{

	int checker = 1;//this essentially marks whether the program makes any sense, 0 indicates it doesnn't

	for (int i = 0; i < numRooms; i++)
	{
		for (int j = 0; j < numRooms; j++)
		{
			if (i != j)
			{
				//first it check if the y coordinates of the top edge are inside the range+1(1 is added to keep a gap of 1) of the other rooms. If a room is coincides on the y-coordinates, then we check the x-coordinates
				if(
				rooms[i].yloc >= rooms[j].yloc &&
				rooms[i].yloc <= (rooms[j].yloc + rooms[j].ylen + 1)
				)
				{
					//If a room is coincides on the y-coordinates, then we check the x-coordinates by checking the left corner, the right corner and the middle.
					if(
					(rooms[i].xloc >= rooms[j].xloc &&//this checks left corner
					rooms[i].xloc <= (rooms[j].xloc + rooms[j].xlen + 1)
					) || (
					rooms[i].xloc + rooms[i].xlen >= rooms[j].xloc &&//this checks right corner
					rooms[i].xloc + rooms[i].xlen <= (rooms[j].xloc + rooms[j].xlen + 1)
					) || (
					rooms[i].xloc < rooms[j].xloc &&//this checks middle
					rooms[i].xloc + rooms[i].xlen > (rooms[j].xloc + rooms[j].xlen + 1)
					)
					) checker = 0;
				}


			}

			if (checker == 0) break;//this helps end the program soon if the coordinates don't make sense

		}

		if (//this condition just makes sure all the coordinates will map the rooms in the available grid
			(rooms[i].xloc + rooms[i].xlen > xlenMax - 1) ||
			(rooms[i].yloc + rooms[i].ylen > ylenMax - 1)
			) checker = 0;

		if (checker == 0) break;//this, along with the break above, ensures we swiftly end the program soon if the coordinates don't make sense
	}

	return checker;
}

/*The second function simply creates a number that we use as a denominator for calculating max_size of the rooms.
The reason for this function was because the program took too long to find coordinates that made sense of number
of rooms greater than 8. This restricts the random function a little bit more - hence the not_so_random part of the name.*/
int not_so_rand_roomsize_resizer(int numRooms)
{
	int roomSizer = (numRooms/2) - 1;

	return roomSizer;
}
