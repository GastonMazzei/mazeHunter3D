
#include "Labyrinthe.h"
#include <random>
#include <ctime>
#include <cmath>

#ifndef GARDIEN_H
#define GARDIEN_H


#include "Chasseur.h"
#include "Mover.h"

class Labyrinthe;

/*
 * Class for generic Alien Guardians
 */
class Gardien : public Mover {
public:
	Gardien (Labyrinthe* l, const char* modele) : Mover (120, 80, l, modele)
	// Constructor set the random seed to the time at which the script is run
	{std::srand(time(0));}

	// Sets the killed status to "false" or "true"
	void give_life(){dummy=false;}
	void remove_life(){dummy=true;}

	// public access to the lifesigns value
	double get_lifesigns();
	
	// update() is constantly called by the system over all Gardiens
	void update (void);

	// Behaviour functions
	bool move (double dx, double dy);
	void fire (int angle_vertical);
	bool process_fireball (float dx, float dy);

	// Separate function used to process fireballs thrown by Chasseur
	bool process_fireball_external (float dx, float dy);

private:
	// ***CLASS ATTRIBUTES***

	// 1- Macro-defined attributes
	int firing_frequency = GARDIEN_FIRING_FREQUENCY; 
       	int walking_frequency = GARDIEN_WALKING_FREQUENCY;
	int eyesight_reaches_here = EYESIGHT_REACHES_HERE;
	int  dont_get_closer_than = DONT_GET_CLOSER_THAN;
	double lifesigns = GARDIEN_LIFE;
	double accuracy = GARDIEN_ACCURACY;
	double probability_change_direction = (double) 1 - (double) GARDIEN_PROBABILITY_CHANGE_DIRECTION; 
	
	// 2- Self-explained attributes
	bool isChasseurVisible = false;
	bool stayStill = false;
	int _counter = 0;
	double chasseur_distance = 0;
	double chasseurTheta = 0; // Angle to Chasseur :-)

	// 3- Attributes that may require explanation
	//
	bool dummy = true; // boolean indicates if the Gardien has been killed (true)	
	//
	bool angry = false; // movement has a binary behaviour: angry or not?
	//
	int x_moving_trend = 1; // if Gardien is not angry, it moves randomly in 
	int y_moving_trend = -1; // the direction (x_moving_trend, y_moving_trend) ϵ [(1,1), (1,-1), (-1,1), (-1,-1)]
	//

	// ***CLASS FUNCTIONS***
	//
	//  can a ray pass through this point? if not, return 1
	int check_availability(int ix_x, int ix_y);  
	//
	// Is the chasseur visible? update variables accordingly.
	void update_chasseur_visibility(void);
	//
	// ACCURACY and FIRING_FREQUENCY depend on the
	// Gardien's lifesigns
	//
	void update_gardien_accuracy(void);
	void update_gardien_firingFrequency(void);
	//
	// Helpers: the two type of movements (angry or not)
	void move_towards_chasseur();
	void move_randomly();

};

#endif
