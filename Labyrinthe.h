#ifndef LABYRINTHE_H
#define LABYRINTHE_H

#include <ios>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cassert>
#include "Environnement.h"

#define	LAB_WIDTH	258
#define	LAB_HEIGHT	258

class Labyrinthe : public Environnement {
private:
	//char	_data [LAB_WIDTH][LAB_HEIGHT];
	char	**_data;
	int 	_width = -1;
	int 	_height = -1;

	char **find_measurements (std::ifstream *lab_file, char *line); //90-60-90 or nothing
	void fill_temp_tab (char **tab, char *line, int indice);
	void walls_create (char **tab);
	void boxes_create(char **tab);
public:
	void destroyGardienByIndex(int i);	
	Labyrinthe (char*);
	int width () { return _width; };	// retourne la largeur du labyrinthe.
	//void set_width (int w) { assert(w > 0); _width = w; };
	int height () { return _height; };	// retourne la longueur du labyrinthe.
	//void set_height (int h) { assert(h > 0); _height = h; };
	
	bool is_wall (char c);
	
	char data (int i, int j)
	{
		return _data [i][j];
	};	// retourne la case (i, j).
};

#endif
