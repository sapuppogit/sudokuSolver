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
const int nThreads = 4;

int main() {
	double wtime = omp_get_wtime();
	int size = numSudokuFiles();
	sudoku s[size];
	openFile(s);

	#pragma omp parallel for schedule(dynamic,1) shared(s) num_threads(nThreads)
	for (int i=0; i<size; i++) {
		printf("Thread %d computes %s\n",  omp_get_thread_num(), s[i].file.c_str());
		solveSudoku(&s[i]);
		if (s[i].file == "101.txt" || s[i].file == "102.txt" || s[i].file == "103.txt") printSudoku(s[i]);
		if(isComplete(s[i])) cout << s[i].file << " Completed" << endl;
		else cout << s[i].file << " Not completed" << endl;
	}
	wtime = omp_get_wtime() - wtime;
	printf( "Execution time: %f\n", wtime );
	return 0;
}

