#include "Gardien.h"
#include  <iostream>

/*
 *	Move
 */

bool Gardien::move (double dx, double dy)
{
	if (EMPTY == _l -> data (
				// 1st argument
				(int) (   (_x + dx) /
				       	   Environnement::scale
				        ),
				 // 2nd argument
			 	 (int)(    (_y + dy) /
					   Environnement::scale
				       )
				 ))
	{
		_x += dx;
		_y += dy;
		return true;
	}
	return false;
}

void Gardien::update_gardien_accuracy(){
	accuracy = accuracy * 0.99999;
}

void Gardien::update(void){	
	// Update accuracy according to life
	update_gardien_accuracy();

	// Check if the Hunter is visible, and
	// change the anger status accordingly
	update_chasseur_visibility();
	if (isChasseurVisible){
		angry = true;
	} else {
		angry = false;
	}

	// if we are angry, move towards the Chasseur
	if (angry){
		move_towards_chasseur();
	} else {
		move_randomly();
	}
}


void Gardien::move_towards_chasseur(){

	//
	//
	
}

void Gardien::move_randomly(){
	double xrand = ((double) (std::rand() % 100000)) / 100000.;
	double yrand = ((double) (std::rand() % 100000)) / 100000.;
	if (xrand > current_change_threshold){
		if (x_moving_trend == 1) {
			x_moving_trend = -1;
		} else {
			x_moving_trend = 1;
		}
	}
	if (yrand > current_change_threshold){
		if (y_moving_trend == 1) {
			y_moving_trend = -1;
		} else {
			y_moving_trend = 1;
		}
	}
	move((double)  x_moving_trend, (double) y_moving_trend);	
}

void Gardien::update_chasseur_visibility(void){
	// TODO
	// Apply some criteria: is the chasseur visible?
	isChasseurVisible = false;
}



// Actively modify
bool Gardien::process_fireball (float dx, float dy)
{
	std::cout << "Ouch! I've received a Fireball and the code of my reaction is not written haha :-)" << std::endl;

	// Decrease life according to the distance :-)
	// CODE GOES  HERE ;;; TODO
	
	// If life terminates extinguish myself :-)	


	bool TEST = false;
	if (TEST)
	{
		// calculer la distance entre le chasseur et le lieu de l'explosion.
		//float	x = (_x - _fb -> get_x ()) / Environnement::scale;
		//float	y = (_y - _fb -> get_y ()) / Environnement::scale;
		//float	dist2 = x*x + y*y;
		// on bouge que dans le vide!
		//if (EMPTY == _l -> data ((int)((_fb -> get_x () + dx) / Environnement::scale),
		//					 (int)((_fb -> get_y () + dy) / Environnement::scale)))
		//{
		//	message ("Woooshh ..... %d", (int) dist2);
		//	// il y a la place.
		//	return true;
		//}
		// collision...
		// calculer la distance maximum en ligne droite.
		//float	dmax2 = (_l -> width ())*(_l -> width ()) + (_l -> height ())*(_l -> height ());
		// faire exploser la boule de feu avec un bruit fonction de la distance.
		//_wall_hit -> play (1. - dist2/dmax2);
		//message ("Booom...");
	}
	return false;
}




/*
 *	Attack the hunter.
 */

void Gardien::fire (int angle_vertical)
{
	bool TEST = false;
	if (TEST)
	{
	//	message ("Woooshh...");
	//	_hunter_fire -> play ();
	//	_fb -> init (/* position initiale de la boule */ _x, _y, 10.,
	//			 /* angles de visée */ angle_vertical, _angle);
	}
}



