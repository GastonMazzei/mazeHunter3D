#include "Labyrinthe.h"
#include "Chasseur.h"
#include "Gardien.h"

Sound*	Chasseur::_hunter_fire;	// bruit de l'arme du chasseur.
Sound*	Chasseur::_hunter_hit;	// cri du chasseur touché.
Sound*	Chasseur::_wall_hit;	// on a tapé un mur.

Environnement* Environnement::init (char* filename)
{
	return new Labyrinthe (filename);
}


bool Labyrinthe::is_wall (char c) {
	return (c != ' ' && c != '\n' && c != 'x' && c != 'T' && c != 'G' && c != 'C');
}


void Labyrinthe::find_measurements (std::ifstream lab_file, char *line) {
	//trying to find the max length and height of the maze
	int maxl = 0;
	int maxh = 0;
	lab_file.getline(line, 2048);
	
	//loop until end of file
	while (!lab_file.eof()) {
		//if getline failed and it has not reached eof
		if (lab_file.fail()) {
			std::cout << "reading the file failed, either the line is too long (<2048) or the construction of sentry failed" << std::endl;
			return;
		}
		
		int i = 0;
		for (; i < 2048 && line[i] != '#' && line[i] != '\n'; i++);
		i--;
		for (; i > 0 && line[i] == ' '; i--);
		maxl = (i > maxl)? i:maxl;
		
		//not a commentary or useless line then increase width
		if (i != 0) maxh+=1;
		lab_file.getline(line, 2048);
	}
	
	self.set_width (maxl);
	self.set_height (maxh);
}


Labyrinthe::Labyrinthe (char* filename)
{
	std::cout << filename << std::endl;
	std::ifstream lab_file;
	lab_file.open(filename, std::ifstream::in);
	
	if(!lab_file.is_open()) {
		// need to crash with error message
		std::cout << "filename for maze layout not found or not opennable" << std::endl;
		return;
	}
	
	char *line;
	if ((line = (char*) malloc(2048)) == NULL) {
		std::cout << "malloc crash" << std::endl;
		return;
	}
	
	self.find_measurements(lab_file, line);
	
	/*
	//we malloc an array to contain the maze only
	char **tab = (char**) malloc(maxh * sizeof(char*));
	for (i = 0; i < maxh; i++) {
		tab[i] = (char*) malloc(maxl * sizeof(char));
	}*/
	
	for (int i = 0; i < 2048 && line[i] != '#' && line[i] != '\n'; i++) {
		if (line[i] == '+') {
			int j = i+1;
			for (; j < 2048 && Labyrinthe::is_wall(line[j]); j++);
			if (i < j-1) {
				///walls[x] = { i, h1, j-1, h2, 0 },
			}
			
			
		}
	}
	
	
	
	// les 4 murs.
	static Wall walls [] = {
		{ 0, 0, LAB_WIDTH-10, 0, 0 },
		{ LAB_WIDTH-1, 0, LAB_WIDTH-1, LAB_HEIGHT-1, 0 },
		{ LAB_WIDTH-1, LAB_HEIGHT-1, 0, LAB_HEIGHT-1, 0 },
		{ 0, LAB_HEIGHT-1, 0, 0, 0 },
	};
	// une affiche.
	//  (attention: pour des raisons de rapport d'aspect, les affiches doivent faire 2 de long)
	static Wall affiche [] = {
		{ 4, 0, 6, 0, 0 },		// première affiche.
		{ 8, 0, 10, 0, 0 },		// autre affiche.
	};
	// 3 caisses.
	static Box	caisses [] = {
		{ 70, 12, 0 },
		{ 10, 5, 0 },
		{ 65, 22, 0 },
	};

/* DEB - NOUVEAU */
	// 2 marques au sol.
	static Box	marques [] = {
		{ 50, 14, 0 },
		{ 20, 15, 0 },
	};
/* FIN - NOUVEAU */

	// juste un mur autour et les 3 caisses et le trésor dedans.
	for (int i = 0; i < LAB_WIDTH; ++i)
		for (int j = 0; j < LAB_HEIGHT; ++j) {
			if (i == 0 || i == LAB_WIDTH-1 || j == 0 || j == LAB_HEIGHT-1)
				_data [i][j] = 1;
			else
				_data [i][j] = EMPTY;
		}
	// indiquer qu'on ne marche pas sur une caisse.
	_data [caisses [0]._x][caisses [0]._y] = 1;
	_data [caisses [1]._x][caisses [1]._y] = 1;
	_data [caisses [2]._x][caisses [2]._y] = 1;
	// les 4 murs.
	_nwall = 4;
	_walls = walls;
	// deux affiches.
	_npicts = 2;
	_picts = affiche;
	// la deuxième à une texture différente.
	char	tmp [128];
	sprintf (tmp, "%s/%s", texture_dir, "voiture.jpg");
	_picts [1]._ntex = wall_texture (tmp);
	// 3 caisses.
	_nboxes = 3;
	_boxes = caisses;

/* DEB - NOUVEAU */
	// mettre une autre texture à la deuxième caisse.
	sprintf (tmp, "%s/%s", texture_dir, "boite.jpg");
	caisses [1]._ntex = wall_texture (tmp);

	// mettre les marques au sol.
	sprintf (tmp, "%s/%s", texture_dir, "p1.gif");
	marques [0]._ntex = wall_texture (tmp);
	sprintf (tmp, "%s/%s", texture_dir, "p3.gif");
	marques [1]._ntex = wall_texture (tmp);
	_nmarks = 2;
	_marks = marques;
/* FIN - NOUVEAU */

	// le trésor.
	_treasor._x = 10;
	_treasor._y = 10;
	_data [_treasor._x][_treasor._y] = 1;	// indiquer qu'on ne marche pas sur le trésor.
	// le chasseur et les 4 gardiens.
	_nguards = 5;
	_guards = new Mover* [_nguards];
	_guards [0] = new Chasseur (this);
	_guards [1] = new Gardien (this, "Lezard");
	_guards [2] = new Gardien (this, "Blade"); _guards [2] -> _x = 90.; _guards [2] -> _y = 70.;
	_guards [3] = new Gardien (this, "Serpent"); _guards [3] -> _x = 60.; _guards [3] -> _y = 10.;
	_guards [4] = new Gardien (this, "Samourai"); _guards [4] -> _x = 130.; _guards [4] -> _y = 100.;
	// indiquer qu'on ne marche pas sur les gardiens.
	_data [(int)(_guards [1] -> _x / scale)][(int)(_guards [1] -> _y / scale)] = 1;
	_data [(int)(_guards [2] -> _x / scale)][(int)(_guards [2] -> _y / scale)] = 1;
	_data [(int)(_guards [3] -> _x / scale)][(int)(_guards [3] -> _y / scale)] = 1;
	_data [(int)(_guards [4] -> _x / scale)][(int)(_guards [4] -> _y / scale)] = 1;
}

