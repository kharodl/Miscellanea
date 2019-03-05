#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

/* structure for passing data to threads */
typedef struct {
	int row;
	int column;
	int type; // (type / 9) 0 = rows, 1 = columns, 2 = box
	int **grid;
} parameters;

void * solveCheck(void * params) {
	parameters *data = (parameters *) params;		// Dereference parameters
	int found[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 }; 	// Initialize found set
	int row = data->row, column = data->column;		// Used for box type traversing
	int currentVal;									// Used as temp for duplicate check
	for (int i = 0; i < 9; i++) {
		if (data->type / 9 == 0) { 					// Traverse as row type
			currentVal = data->grid[data->row][i];
		} else if (data->type / 9 == 1) { 			// Traverse as column type
			currentVal = data->grid[i][data->column];
		} else { 									// Traverse as box type
			currentVal = data->grid[row][column];
			column++;
			if (column > data->column + 2) {
				column = data->column;
				row++;
			}
		}
		if (currentVal < 1 || currentVal > 9 || found[currentVal - 1] == 1)
			pthread_exit((void *) -1); 	// Invalid or duplicate number, end thread with -1 code
		else
			found[currentVal - 1] = 1;	// Note that currentVal exists in set
	}
	pthread_exit((void *) 0); 			// No invalid or duplicate number found, end thread with 0 code
}

int main(void) {
	printf("\nCS149 Sudoku from Lovejit Kharod\n");

	int grids[2][9][9] = {{{ 4, 1, 9, 3, 7, 6, 5, 8, 2 },
						   { 3, 6, 7, 8, 5, 2, 1, 4, 9 },
						   { 2, 8, 5, 1, 9, 4, 3, 7, 6 },
						   { 6, 4, 1, 9, 3, 7, 8, 2, 5 },
						   { 9, 3, 2, 4, 8, 5, 6, 1, 7 },
						   { 5, 7, 8, 2, 6, 1, 4, 9, 3 },
						   { 8, 5, 6, 7, 4, 9, 2, 3, 1 },
						   { 1, 9, 4, 5, 2, 3, 7, 6, 8 },
						   { 7 ,2, 3, 6, 1, 8, 9, 5, 4 }},
						  {{ 7, 3, 9, 8, 4, 6, 1, 2, 5 },
						   { 4, 1, 2, 9, 7, 5, 8, 6, 3 },
						   { 8, 0, 5, 2, 3, 1, 9, 4, 7 },
						   { 5, 4, 7, 6, 9, 3, 2, 1, 8 },
						   { 3, 9, 1, 7, 8, 2, 4, 5, 6 },
						   { 2, 8, 6, 5, 1, 4, 3, 7, 9 },
						   { 9, 2, 8, 4, 6, 7, 5, 3, 1 },
						   { 1, 7, 4, 3, 5, 9, 1, 8, 2 },
						   { 6, 5, 3, 1, 2, 8, 7, 9, 4 }}};

	int **grid = malloc(9 * sizeof(int *));
	pthread_t thread[27];
	parameters **data = malloc(27 * sizeof(parameters *));

	for (int i = 0; i < 18; i++) { 		// Allocate and define type, row, column for row and column type
		data[i] = malloc(sizeof(parameters));
		data[i]->type = i;
		data[i]->row = data[i]->column = i % 9;
	}

	int rowTemp = 0, columnTemp = 0;
	for (int i = 18; i < 27; i++) { 	// Allocate and define type, row, column for box type threads
		data[i] = malloc(sizeof(parameters));
		data[i]->type = i;
		data[i]->row = rowTemp;
		data[i]->column = columnTemp;
		columnTemp += 3;
		if (columnTemp > 6) {
			columnTemp = 0;
			rowTemp += 3;
		}
	}

	for (int gridChoice = 0; gridChoice < 2; gridChoice++) { // Set and print current grid
		printf("\nChecking puzzle %d...\n\n", gridChoice);
		for (int i = 0; i < 9; i++) {
			grid[i] = grids[gridChoice][i];
			for (int j = 0; j < 9; j++)
				printf("%d ", grid[i][j]);
			printf("\n");
		}

		for (int i = 0; i < 27; i++) { 	// Set grid pointer for each set of parameters and create threads
			data[i]->grid = grid;
			pthread_create(&thread[i], NULL, solveCheck, data[i]);
		}

		void **solvedTemp = malloc(sizeof(void *));
		int solved = 1, hint = -1;
		for (int i = 0; i < 27; i++) { 	// Join threads back together while checking return value for invalid sets
			pthread_join(thread[i], solvedTemp);
			if (*solvedTemp != (void *) 0) {
				solved = 0;
				hint = i;
			}
		}
		free(solvedTemp);

		char hintType[] = "unknown";
		switch (hint / 9) { 			// After type / 9, 0 = rows, 1 = columns, 2 = box
		case 0:
			strcpy(hintType, "row");
			break;
		case 1:
			strcpy(hintType, "column");
			break;
		case 2:
			strcpy(hintType, "box");
			break;
		}
		if (!solved) // Print whether solved or not with hint if not solved
			printf("\nNot solved, try again... hint: take a look at %s #%d\n",
					hintType, hint % 9 + 1);
		else
			printf("\nSolved! Good job!\n");
	}

	for (int i = 0; i < 27; i++) 		// Free miscellaneous pointers
		free(data[i]);
	free(data);
	free(grid);
}
