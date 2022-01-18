#ifndef CHASSEUR_H
#define CHASSEUR_H

#include <stdio.h>
#include "Mover.h"
#include "Sound.h"
#include  <set>
#include "Labyrinthe.h"

class Labyrinthe;

class Chasseur : public Mover {
private:
	// accepte ou non un deplacement.
	bool move_aux (double dx, double dy);
	std::set<int> killed;
	bool hasLastBallExploded = false;
	int _counter=0;
	bool blinking_message=false;
	std::string current_message="Hello";
	std::string next_message = "Hi";
	int blinking_period = 60;
	int last_blinker = 250;
	bool gardienInfo = false;
	double lowest_lifesigns = GARDIEN_LIFE;
public:
	void add_info2message(std::string information,double lifesigns);
	double lifesigns = CHASSEUR_LIFE;
	// les sons.
	static Sound*	_hunter_fire;	// bruit de l'arme du chasseur.
	static Sound*	_hunter_hit;	// cri du chasseur touch�.
	static Sound*	_wall_hit;		// on a tap� un mur.

	// constructeur.
	Chasseur (Labyrinthe* l);
	// ne bouger que dans une case vide (on 'glisse' le long des obstacles)
	bool move (double dx, double dy) {
		return move_aux (dx, dy) || move_aux (dx, 0.0) || move_aux (0.0, dy);
	}
	// le chasseur ne pense pas!
	void update (void) {};
	// fait bouger la boule de feu (ceci est une exemple, � vous de traiter les collisions sp�cifiques...)
	bool process_fireball (float dx, float dy);
	// tire sur un ennemi.
	void fire (int angle_vertical);
	// clic droit.
	void right_click (bool shift, bool control);
};

#endif
