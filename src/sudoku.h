/*
 * sudoku.h
 *
 *  Created on: Aug 1, 2019
 *      Author: samuel
 */

using namespace std;

const int N=9;

struct sudoku {
	// For each position it contains
	// 1 if a number can be entered, 0 otherwise
	int pos[N][N][N];
	// For each position contains the value
	int sud[N][N];
	string file;
};

void subdivideMatrix(sudoku s);

// For each txt file creates a sudoku and initialize pos and sud
sudoku createSudoku() {
	sudoku s;
	for (int y=0; y<N; y++)
		for (int x=0; x<N; x++) {
			for (int k=0; k<N; k++)
				s.pos[y][x][k] = 1;
			s.sud[y][x] = 0;
		}
	return s;
}

// Check if (x,y) is the only possible position for value k in the row y
bool checkInRow(sudoku *s, int x, int y, int k) {
	bool isChecked = true;
	// all cells in the row are checked in parallel
	#pragma omp parallel
	for (int i=0; i<N; i++)
		if (i!=x && s->pos[y][i][k] == 1) isChecked = false;
	return isChecked;
}

// Check if (x,y) is the only possible position for value k in the column x
bool checkInCol(sudoku *s, int x, int y, int k) {
	bool isChecked = true;
	// all cells in the column are checked in parallel
	#pragma omp parallel
	for (int i=0; i<N; i++)
		if (i!=y && s->pos[i][x][k] == 1) isChecked = false;
	return isChecked;
}

// Sets the whole row to 0 for the value k
// No other k value can be added to this row
void insInRow(sudoku *s, int y, int k) {
	for (int x=0; x<N; x++)
		s->pos[y][x][k] = 0;
}

// Sets the whole column to 0 for the value k
// No other k value can be added to this column
void insInCol(sudoku *s, int x, int k) {
	for (int y=0; y<N; y++)
		s->pos[y][x][k] = 0;
}

// Sets the whole block 3x3 to 0 for the value k
// No other k value can be added to this block
void insInBlk(sudoku *s, int x, int y, int k) {
	int n = y/3;
	int m = x/3;
	for (int j=n*3; j<n*3+3; j++)
		for (int i=m*3; i<m*3+3; i++)
			s->pos[j][i][k] = 0;
}

// Sets the whole row to 0 for the value k
// No other k value can be added to this row
void insInCell(sudoku *s, int x, int y) {
	for (int k=0; k<N; k++)
		s->pos[y][x][k] = 0;
}

// Sets all values ​​of a cell to 0
// No other value can be added to this cell
void insInSudoku(sudoku *s, int x, int y, int k) {
	s->sud[y][x] = k;
}

void insVal(sudoku *s, int x, int y, int k) {
	#pragma omp parallel sections
		{
		#pragma omp section
		  insInRow(s, y, k-1);
		#pragma omp section
		  insInCol(s, x, k-1);
		#pragma omp section
		  insInBlk(s, x, y, k-1);
		#pragma omp section
		  insInSudoku(s, x, y, k);
		#pragma omp section
		  insInCell(s, x, y);
	}
}

// Check if for the cell (x,y) there is a value that is the only one possible
// Return -1 if there isn't, the value k if it is the only possible one
int getUniqueNum(sudoku s, int x, int y) {
	int i=-1;
	for (int k=0; k<N; k++) {
		if (s.pos[y][x][k] == 1) {
			if (i==-1) i=k;
			else return -1;
		}
	}
	if (i==-1) return -1;
	else return i+1;
}

// Check for the cell (x,y) if there is a value that is the only one possibile
// in that row or in that column.
// Returns -1 if there is not a unique possible value for that row and column
// the value k otherwise
int getUniqueVal(sudoku s, int x, int y) {
	for (int k=0; k<N; k++) {
		bool answer1, answer2;
		#pragma omp parallel sections
		{
			#pragma omp section
				answer1 = checkInRow(&s, x, y, k);
			#pragma omp section
				answer2 = checkInCol(&s, x, y, k);
		}
		if (s.pos[y][x][k] == 1 && (answer1 || answer2)) return k+1;
	}
	return -1;
}

// Check if for each cell there is a value greater than 0
bool isComplete(sudoku s) {
	for (int i=0; i<N; i++)
		for (int j=0; j<N; j++)
			if (s.sud[i][j] == 0) return false;
	return true;
}

// Returns true, so it is wrong, if there is a cell with number 0,
// where you can enter a value k
bool isWrong(sudoku s) {
	bool temp;
	for (int i=0; i<N; i++)
		for (int j=0; j<N; j++) {
			if (s.sud[i][j] == 0) {
				temp = false;
				for (int k=0; k<N; k++ ) {
					if (s.pos[i][j][k] == 1) temp = true;
				}
				if (!temp) return true;
			}
		}
	return false;
}

// x and y assume the position of the first cell or of the
// first cell in which it is possible to enter a maximum of 2 values

void lastTwoValues(sudoku s, int *x, int *y) {
	int count;
	for (int i=0; i<N; i++)
		for (int j=0; j<N; j++)
			if (s.sud[i][j] == 0) {
				if (*x==-1 && *y==-1) {
					*x=j; *y=i;
				}
				count = 0;
				for (int k=0; k<N || count==3; k++) {
					if (s.pos[i][j][k] == 1) count++;
				}
				if (count == 2) {
					*x=j; *y=i;
					return;
				}
			}
}

void printSudoku(sudoku s) {
	cout << "sudoku " << s.file << endl;
	for (int i=0; i<N; i++) {
		if (i==3 || i==6) cout << " ------+-------+------" << endl;
		for (int j=0; j<N; j++) {
			cout << " " << s.sud[i][j];
			if (j==2 || j==5) cout << " |";
		}
		cout << endl;
	}
	cout << endl;
}

// For each cell it checks if there is a value k unique in the position (x,y)
// if it exists, it inserts k into that cell
// otherwise it checks if there is a value k unique in that row or column
// if it exists, it inserts k into that cell.

// For each value inserted a counter i is increased.
// If at the end of the sudoku i is higher than 0, the the process is repeated
// otherwise it checks if the sudoku is concluded and then the process terminated
// or is not concluded and no other steps are possible.

// If no other steps are possibile check if the sudoku is wrong or not.
// If it is not than is created then a temporary scheme is created in which,
// in the cell where there are maximum 2 possible values, one of the two values ​​is entered
// and the process continues until checked that sudoku is wrong and the other value is chosen.

// The process is repeated until all the options have been taken and the sudoku is complete or wrong.
bool solveSudoku(sudoku *s) {
	int i=0;
	do {
		i=0;
		#pragma omp parallel for

		for (int y=0; y<N; y++)
			for (int x=0; x<N; x++)
				if (s->sud[y][x] == 0) {
					int k = getUniqueNum(*s, x, y);
					if (k != -1) {
						insVal(s, x, y, k);
						i++;
					}
					else {
						k = getUniqueVal(*s, x, y);
						if (k != -1) {
							insVal(s, x, y, k);
							i++;
						}
					}
				}

	} while (i>0);
	if (isComplete(*s)) {
		return true;
	}
	else if (!isWrong(*s)) {
		sudoku tempSud = *s;

		int x = -1, y = -1;
		lastTwoValues(*s, &x, &y);
		for (int k=0; k<N; k++) {
			if (s->pos[y][x][k] == 1) {
				insVal(&tempSud, x, y, k+1);
				if (solveSudoku(&tempSud)) {
					*s=tempSud;
					return true;
				}
			}
			tempSud = *s;
		}
	}
	else {
		return false;
	}
	return false;
}

