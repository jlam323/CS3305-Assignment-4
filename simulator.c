/*****************************
CS3305 Assignment 4
Jonathan Lam
250754703
******************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NUM_ADDRESS 100		//changeable max number of memory addresses in the trace


typedef struct pageInfo
{
	int frameNumber;		//position in the table
	int lastused;			//counter for LRU
	int frequency;			//counter for number of times the frame was used
	char *address;			//memory address from the trace
}pageInfoEntry;



int main(int argc, char *argv[])
{
	char line[50];					//used for reading from the trace file
	char *filename;					//name of the file
	FILE *file;						//used to read the file

	char *addresses[NUM_ADDRESS];	//number of memory addresses
	int numaddr = -1;				//number of memory addresses in the trace

	int pageTableSize;				//number of frames, set by the user
	char *algorithm;				//LFU or LRU

	int count;						//used to scan through the trace array
	int count2;						//used to scan through the frame array
	int pagefaults = 0;				//output of the program
	int found;				    	//boolean to represent if the page was found in the table
	int largest;					//used for LRU
	int least;						//used for LFU
	int temp;
	int newframe;                   //records the most recently added frame

									//check number of arguments
	if (argc != 4)
	{
		printf("Missing arguments\n");
		exit(EXIT_FAILURE);
	}
	else
	{
		pageTableSize = atoi(argv[1]);
		filename = argv[2];
		algorithm = argv[3];

		//DEBUG
		//printf("%d\n", pageTableSize);
		//printf("%s\n", filename);
		//printf("%s\n", algorithm);
	}

	//declare size of the page table
	pageInfoEntry *pageTable;
	pageTable = (pageInfoEntry*)malloc(sizeof(pageInfoEntry)*pageTableSize);


	//---------------------------------
	//READ FROM THE FILE
	//---------------------------------

	//create the FILE
	file = fopen(filename, "r");

	//handle a non-existing file or missing file name
	if (!file)
	{
		printf("Can't find the file.\n");
		exit(EXIT_FAILURE);
	}
	//read the file line by line
	else
	{
		while (fgets(line, 50, file) != NULL)
		{
			//remove '\n' character
			strtok(line, "\n");
			//allocate space
			addresses[numaddr + 1] = malloc(strlen(line) + 1);
			//record the address and increase the pointer
			strcpy(addresses[numaddr + 1], line);
			numaddr++;
		}
	}

	//close the text file
	fclose(file);


	/*******************
	*  Initialize table
	********************/
	for (count = 0; count < pageTableSize; count++)
	{
		pageTable[count].frameNumber = 0;
		pageTable[count].lastused = 0;
		pageTable[count].frequency = 0;
		pageTable[count].address = malloc(6);
		strcpy(pageTable[count].address, "");
	}



	/*******************
	*  LRU Algorithm
	*******************/
	if (strcmp(algorithm, "LRU") == 0)
	{
		//go through the address list
		for (count = 0; count <= numaddr; count++)
		{
			found = 0;

			//lookup if the table has a frame with the page loaded
			for (count2 = 0; count2 < pageTableSize; count2++)
			{
				//check if the memory address matches the one being looked up
				if (strcmp(pageTable[count2].address, addresses[count]) == 0)
				{
					printf("Page %s found at frame #%d.\n", addresses[count], count2 + 1);
					//record that the page has been found
					found = 1;
					newframe = count2;
					pageTable[count2].lastused = 0;
					//exit loop
					count2 = pageTableSize;
				}
			}

			//check if the page was not found
			if (found == 0)
			{
				//record the pagefault
				pagefaults++;
				//reset
				largest = -1;

				//find the page that we haven't used in the longest
				for (count2 = 0; count2 < pageTableSize; count2++)
				{
					//if NULL frame, then just put it in there
					if (pageTable[count2].frameNumber == 0)
					{
						//put the page's information into the frame
						pageTable[count2].frameNumber = count2 + 1;
						strcpy(pageTable[count2].address, addresses[count]);
						pageTable[count2].lastused = 0;
						printf("Page fault: %s not found. Placed in empty frame #%d.\n", addresses[count], count2 + 1);

						newframe = count2;

						//exit loop
						count2 = pageTableSize;
						found = 1;
					}
					//if the address still isn't moved onto a NULL frame
					else if (found == 0)
					{
						//check if this frame was used the longest from now
						if (pageTable[count2].lastused > largest)
						{
							largest = pageTable[count2].lastused;
							temp = count2;
						}
					}
				}

				//replace the last used frame from now
				if (found == 0)
				{
					printf("Page fault: %s was not found. Replacing the page in frame #%d.\n", addresses[count], temp + 1);
					strcpy(pageTable[temp].address, addresses[count]);
					pageTable[temp].lastused = 0;
					pageTable[temp].frameNumber = temp + 1;
					found = 0;	//reset
					newframe = temp;
				}
			}
			//if the page has been found earlier
			else
			{
				//reset the found confirmation
				found = 0;
			}

			//scan through the list and add increase the frame table 'last used'
			for (count2 = 0; count2 < pageTableSize; count2++)
			{
				//add 1 for all non-NULL frames except for the one we just added in
				if ((count2 != newframe) && (pageTable[count2].frameNumber != 0))
					pageTable[count2].lastused++;
			}
		}
	}

	/*******************
	*  LFU Algorithm
	*******************/
	else if (strcmp(algorithm, "LFU") == 0)
	{
		//go through the address list
		for (count = 0; count <= numaddr; count++)
		{
			found = 0;

			//lookup if the table has a frame with the page loaded
			for (count2 = 0; count2 < pageTableSize; count2++)
			{
				//check if the memory address matches the one being looked up
				if (strcmp(pageTable[count2].address, addresses[count]) == 0)
				{
					printf("Page %s found at frame #%d.\n", addresses[count], count2 + 1);
					//record that the page has been found
					found = 1;
					pageTable[count2].frequency++;
					//exit loop
					count2 = pageTableSize;
				}
			}

			//check if the page was not found
			if (found == 0)
			{
				//record the pagefault
				pagefaults++;
				//reset
				least = 99;

				//find the last frequently used page
				for (count2 = 0; count2 < pageTableSize; count2++)
				{
					//if NULL frame, then just put it in there
					if (pageTable[count2].frameNumber == 0)
					{
						//put the page's information into the frame
						pageTable[count2].frameNumber = count2 + 1;
						strcpy(pageTable[count2].address, addresses[count]);
						printf("Page fault: %s not found. Placed in empty frame #%d.\n", addresses[count], count2 + 1);
						pageTable[count2].frequency = 1;
						//exit loop
						count2 = pageTableSize;
						found = 1;
					}
					//if the address still isn't moved onto a NULL frame
					else if (found == 0)
					{
						//check if this frame was used the longest from now
						if (pageTable[count2].frequency < least)
						{
							least = pageTable[count2].frequency;
							temp = count2;
						}
					}
				}

				//replace the least frequently used frame
				if (found == 0)
				{
					printf("Page fault: %s was not found. Replacing the page in frame #%d.\n", addresses[count], temp + 1);
					strcpy(pageTable[temp].address, addresses[count]);
					pageTable[count2].frequency = 1;
					pageTable[temp].frameNumber = temp + 1;
					found = 0;	//reset
				}
			}
			//if the page has been found earlier
			else
			{
				//reset the found confirmation
				found = 0;
			}

		}
	}
	else
	{
		printf("Algorithm name not recognized.\n");
		exit(EXIT_FAILURE);
	}


	printf("\nPage Table\n");
	for (count = 0; count < pageTableSize; count++)
	{
		printf("Frame %d: %s\n", count + 1, pageTable[count].address);
	}


	printf("For algorithm %s on the trace, there were %d number of page faults.\n", algorithm, pagefaults);

}