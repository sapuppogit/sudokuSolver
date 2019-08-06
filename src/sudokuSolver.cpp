//============================================================================
// Name        : sudokuSolver.cpp
// Author      : sapuppogit
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "sudoku.h"
#include "readSudokuTxt.h"

using namespace std;

int main() {
	int size = numSudokuFiles();
	sudoku s[size];
	openFile(s);
	for (int i=0; i<size; i++) {
//		printSudoku(s[i]);
		solveSudoku(&s[i]);
		cout << endl;
//		printSudoku(s[i]);
		if(isComplete(s[i])) cout << i+1 << " Completed" << endl;
		else cout << i+1 << "Not completed" << endl;
		cout << endl << endl;
	}

	return 0;
}

