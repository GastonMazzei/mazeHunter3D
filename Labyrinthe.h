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
	void objects_create(char **tab);
	void print();

public:
	Labyrinthe (char*);
	//width and height now take a parameter because the teacher uses them in the reverse order as we do
	//height = width and width = height and is not natural therefore we can keep using this->width()
	//and get instead height and the teacher code would still work
	//ik it's bad design but it's better than to have array reversed imo
	int width (int flag = 1) { return (flag == 1)? _width:_height; };	// retourne la largeur du labyrinthe.
	//void set_width (int w) { assert(w > 0); _width = w; };
	int height (int flag = 1) { return (flag == 1)? _height:_width; };	// retourne la longueur du labyrinthe.
	//void set_height (int h) { assert(h > 0); _height = h; };
	
	bool is_wall (char c);
	
	char data (int i, int j)
	{
		return _data [i][j];
	};	// retourne la case (i, j).
};

#endif
