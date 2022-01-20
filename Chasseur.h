#ifndef CHASSEUR_H
#define CHASSEUR_H

#include <stdio.h>
#include  <set>

#include "Mover.h"
#include "Sound.h"
#include "Labyrinthe.h"


/*
 * Class for the Hunter: the main character, the only one
 * the user controls with the mouse and keyboard :-)
 */
class Chasseur : public Mover {
private:
	// axuiliary function 
	bool move_aux (double dx, double dy);
	
	// Set with the index of killed Chasseurs
	std::set<int> killed;

	// Configuration relevant to the blinking messages that
	// display Chasseur and Gardien's life.
	bool blinking_message=false;
	bool gardienInfo = false;
	int _counter=0;
	int blinking_period = 60;
	int last_blinker = 250;
	double lowest_lifesigns = GARDIEN_LIFE;
	std::string current_message="Hello";
	std::string next_message = "Hi";
	
public:
	
	// Lifesigns of the Chasseur
	double lifesigns = CHASSEUR_LIFE;

	// Sounds
	static Sound*	_hunter_fire;	// bruit de l'arme du chasseur.
	static Sound*	_hunter_hit;	// cri du chasseur touché.
	static Sound*	_wall_hit;		// on a tapé un mur.

	// constructeur.
	Chasseur (Labyrinthe* l);
	// ne bouger que dans une case vide (on 'glisse' le long des obstacles)
	bool move (double dx, double dy) {
		return move_aux (dx, dy) || move_aux (dx, 0.0) || move_aux (0.0, dy);
	}
	// le chasseur ne pense pas!
	void update (void) {};
	// fait bouger la boule de feu (ceci est une exemple, à vous de traiter les collisions spécifiques...)
	bool process_fireball (float dx, float dy);
	// tire sur un ennemi.
	void fire (int angle_vertical);
	
	// Call this function to start a blinking pattern on the screen,
	// 'information' will be displayed.
	void add_info2message(std::string information,double lifesigns);
};

#endif
