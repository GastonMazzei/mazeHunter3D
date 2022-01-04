#ifndef LABYRINTHE_H
#define LABYRINTHE_H

#include <ios>
#include <iostream>
#include <fstream>
#include <string>
#include "Environnement.h"

#define	LAB_WIDTH	80
#define	LAB_HEIGHT	25

class Labyrinthe : public Environnement {
private:
	char	_data [LAB_WIDTH][LAB_HEIGHT];
	int 	_width = -1;
	int 	_height = -1;

	void find_measurements (std::ifstream lab_file, char *line); //90-60-90 or nothing


public:
	Labyrinthe (char*);
	int width () { return _width; }	// retourne la largeur du labyrinthe.
	int set_width (int w) { assert(w > 0); _width = w; }
	int height () { return _height; }	// retourne la longueur du labyrinthe.
	int set_height (int h) { assert(h > 0); _height = w; }
	
	bool is_wall (char c);
	
	char data (int i, int j)
	{
		return _data [i][j];
	}	// retourne la case (i, j).
};

#endif
