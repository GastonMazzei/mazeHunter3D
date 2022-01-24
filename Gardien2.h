
#include <random>
#include <ctime>
#include <cmath>

#ifndef GARDIEN_H
#define GARDIEN_H


#ifndef GARDIEN_LIFE
#define GARDIEN_LIFE 200
#endif

#ifndef GARDIEN_FIREBALL_DAMAGE
#define GARDIEN_FIREBALL_DAMAGE 20
#endif

#ifndef GARDIEN_ACCURACY
#define GARDIEN_ACCURACY 1.0
#endif

#include "Mover.h"

class Labyrinthe;

class Gardien : public Mover {
public:
	Gardien (Labyrinthe* l, const char* modele) : Mover (120, 80, l, modele)
	{std::srand(time(0));}
	
	bool exists = true;
	void update (void);
	bool move (double dx, double dy);
	void fire (int angle_vertical);
	bool process_fireball (float dx, float dy);
private:
	// ***CLASS ATTRIBUTES***
	// binary movement behaviour: angry or not?
	bool angry = false;
	// is the chasseur visible?
	bool isChasseurVisible = false;
	// a real-valued variable indicates the lifesigns of Gardien
	double lifesigns = (double) GARDIEN_LIFE;
	// a 0-1 number is the probability of the Gardien missing it's target
	double accuracy =  GARDIEN_ACCURACY;
	double default_change_threshold = 0.98;
	double current_change_threshold = 0.98;
	int x_moving_trend = 1;
	int y_moving_trend = -1;
	// ***CLASS FUNCTIONS***
	// Is the chasseur visible? update vars accordingly.
	void update_chasseur_visibility(void);
	// The accuracy depends on the gardien's lifesigns.
	void update_gardien_accuracy(void);
	// Wrappers for diffeVrent movements
	void move_towards_chasseur();
	void move_randomly();
};

#endif