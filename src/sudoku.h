/*
 * sudoku.h
 *
 *  Created on: Aug 1, 2019
 *      Author: samuel
 */

using namespace std;

const int N=9;

struct sudoku {
	int pos[N][N][N];
	int sud[N][N];
	string file;
};

void subdivideMatrix(sudoku s);

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
	for (int i=0; i<N; i++)
		if (i!=x && s->pos[y][i][k] == 1) return false;
	return true;
}

// Check if (x,y) is the only possible position for value k in the column x
bool checkInCol(sudoku *s, int x, int y, int k) {
	for (int i=0; i<N; i++)
		if (i!=y && s->pos[i][x][k] == 1) return false;
	return true;
}

void insInRow(sudoku *s, int y, int k) {
	for (int x=0; x<N; x++)
		s->pos[y][x][k] = 0;
}

void insInCol(sudoku *s, int x, int k) {
	for (int y=0; y<N; y++)
		s->pos[y][x][k] = 0;
}

void insInBlk(sudoku *s, int x, int y, int k) {
	int n = y/3;
	int m = x/3;
	for (int j=n*3; j<n*3+3; j++)
		for (int i=m*3; i<m*3+3; i++)
			s->pos[j][i][k] = 0;
}

void insInCell(sudoku *s, int x, int y) {
	for (int k=0; k<N; k++)
		s->pos[y][x][k] = 0;
}

void insInSudoku(sudoku *s, int x, int y, int k) {
	s->sud[y][x] = k;
}

void insVal(sudoku *s, int x, int y, int k) {
	insInRow(s, y, k-1);
	insInCol(s, x, k-1);
	insInBlk(s, x, y, k-1);
	insInSudoku(s, x, y, k);
	insInCell(s, x, y);
}

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

int getUniqueVal(sudoku s, int x, int y) {
	for (int k=0; k<N; k++)
		if (s.pos[y][x][k] == 1 && (checkInRow(&s, x, y, k) || checkInCol(&s, x, y, k))) return k+1;
	return -1;
}

bool isComplete(sudoku s) {
	for (int i=0; i<N; i++)
		for (int j=0; j<N; j++)
			if (s.sud[i][j] == 0) return false;
	return true;
}

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

bool solveSudoku(sudoku *s) {
	int i=0;
	do {
		i=0;
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
				cout << "value " << k+1 << " added in position [" << x << ", " << y << "]" << endl;
				if (solveSudoku(&tempSud)) {
					*s=tempSud;
					return true;
				}
				else {
					cout << "value " << k+1 << " wrong" <<endl;
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

