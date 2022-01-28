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

// Chasseur's life
#ifndef CHASSEUR_LIFE
#define CHASSEUR_LIFE 800
#endif

// Gardien's life
#ifndef GARDIEN_LIFE
#define GARDIEN_LIFE 50
#endif

// Gardien's accuracy max value
#ifndef GARDIEN_ACCURACY
#define GARDIEN_ACCURACY 1.0
#endif

// Gardien's initial firing period (in calls-to-Gardien::update() units)
#ifndef GARDIEN_FIRING_FREQUENCY
#define GARDIEN_FIRING_FREQUENCY 50
#endif

// Gardien's max allowed firing period (in calls-to-Gardien::update() units)
#ifndef MAX_ALLOWED_FIRING_FREQUENCY
#define MAX_ALLOWED_FIRING_FREQUENCY 130
#endif

// Gardien walking period (in calls-to-Gardien::update() units)
#ifndef GARDIEN_WALKING_FREQUENCY
#define GARDIEN_WALKING_FREQUENCY 2
#endif

// Max Fireball Damage (if it explodes besides you)
#ifndef FIREBALL_DAMAGE
#define FIREBALL_DAMAGE 23
#endif

// Probability of Gardien changing it's direction (normalized to 1)
#ifndef GARDIEN_PROBABILITY_CHANGE_DIRECTION
#define GARDIEN_PROBABILITY_CHANGE_DIRECTION 0.03
#endif
	
// Minimum allowed distance when Gardiens chase the Chasseur
// (in grid units, i,j in d[i][j])
#ifndef DONT_GET_CLOSER_THAN
#define DONT_GET_CLOSER_THAN 3;
#endif

// Max distance at which the Gardiens can detect the Chasseur 
// (in grid units, i,j in d[i][j])
#ifndef EYESIGHT_REACHES_HERE
#define EYESIGHT_REACHES_HERE 50;
#endif

// Max distance the fireball explosion can affect you
// (in grid units, i,j in d[i][j])
#ifndef MAX_RADIUS_FIREBALL
#define MAX_RADIUS_FIREBALL 6
#endif

// A tag defined for the bounty, will be made 'char'
#ifndef BOUNTY_TAG
#define BOUNTY_TAG 'T'
#endif


typedef struct {
	char key;
	char *path;
} textur;


class Labyrinthe : public Environnement {
private:
	char	**_data;
	int 	_width = -1;
	int 	_height = -1;
	textur	**_text;
	int 	_nbtext = -1;

	void find_measurements (std::ifstream *lab_file, char *line); //90-60-90 or nothing
	void fill_temp_tab (char **tab, char *line, int indice);
	void walls_create ();
	void objects_create ();
	void print ();

public:
	void destroyGardienByIndex(int i);	
	Labyrinthe (char*);
	//width and height are reversed because the teacher uses them in the reverse order as we do
	//ik it's bad design but it's better than to have array reversed imo  ======>> more in the summary
	int width () { return _height; };	// retourne la largeur du labyrinthe.
	int height () { return _width; };	// retourne la longueur du labyrinthe.
	
	bool is_lab (char c);
	
	char **data() { return _data; };
	char data (int i, int j) {
		if (i < 0 || j < 0 || i >= _height || j >= _width) {
			static char atila = '1';
			return atila;	//atila is a 'hun' and we need to return a 1 therefore it works ! :)
		}
		return _data [i][j];
	};
	void set_data (int i, int j, char value);
};

#endif
