//============================================================================
// Name        : sudokuSolver.cpp
// Author      : sapuppogit
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <omp.h>
#include "sudoku.h"
#include "readSudokuTxt.h"

using namespace std;

int mains() {
	int size = numSudokuFiles();
	sudoku s[size];
	openFile(s);

	#pragma omp parallel for schedule (static)
		for (int i=0; i<size; i++) {
			printf("Thread %d computes %s\n",  omp_get_thread_num(), s[i].file.c_str());
			solveSudoku(&s[i]);
			if(isComplete(s[i])) cout << s[i].file << " Completed" << endl;
			else cout << s[i].file << " Not completed" << endl;
		}

	return 0;
}

