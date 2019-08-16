/*
 * readSudokuTxt.h
 *
 *  Created on: Aug 2, 2019
 *      Author: samuel
 */

#include <iostream>
#include <fstream>
#include <string>
#include <dirent.h>

using namespace std;

int numSudokuFiles() {
	DIR *dir;
	struct dirent *ent;
	int i=0;
	if ((dir = opendir ("sudokuTxt")) != NULL)
		while ((ent = readdir (dir)) != NULL) i++;
	return i-2;
}

void openFile(sudoku *s){

	struct dirent **namelist;
	int t=0, n;


	n = scandir("sudokuTxt", &namelist, 0, alphasort);
	if (n < 0)
		perror("scandir");
	else {
		for (int i = 0; i < n; i++) {
			string str1 = namelist[i]->d_name;
			if (str1 != "." && str1 != "..") {
				fstream myfile("sudokuTxt/" + str1, std::ios_base::in);

				if (myfile.is_open()) {

					s[t] = createSudoku();
					s[t].file = str1;
					int x, y, k;

					while (myfile >> x >> y >> k)
						insVal(&s[t], x, y, k);

					myfile.close();

					t++;
				}
				else cout << "Unable to open file";
			}
			free(namelist[i]);
		}
	}
	free(namelist);



















/*

	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir ("sudokuTxt")) != NULL) {
		int i=0;

		// print all the files and directories within directory
		while ((ent = readdir (dir)) != NULL) {
			string str1 = ent->d_name;
			if (str1 != "." && str1 != "..") {
				fstream myfile("sudokuTxt/" + str1, std::ios_base::in);

				if (myfile.is_open()) {

					s[i] = createSudoku();
					s[i].file = str1;
					int x, y, k;

					while (myfile >> x >> y >> k)
						insVal(&s[i], x, y, k);

					myfile.close();

					i++;
				}
				else cout << "Unable to open file";
			}
		}
		closedir (dir);
	} else {
	  // could not open directory
	  perror ("");
	}

	*/

}
