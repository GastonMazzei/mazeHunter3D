#include "Gardien.h"
#include "Chasseur.h"
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

	// This is the x-slope that goes from Gardien to Chasseur :-)
	double DX = (_l->_guards[0]->_x - _x);
	double DY = (_l->_guards[0]->_y - _y);
	// Define the discretization required for the step
	int L;
	if (DX > DY){
		L = (int) DX;
	} else {
		L = (int) DY;
	}
	if (L==0) return;
	// if the distance to Chasseur is smaller or equal than MIN_DIST
	// then we stand still
	double MIN_DIST = 0;
	if (std::sqrt(DX * DX + DY * DY) > MIN_DIST){
		double dx = DX / (double) L;
		double dy = DY / (double) L;
		move(dx,dy);	
	}
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
	isChasseurVisible = true;
	// This is the x-slope that goes from Gardien to Chasseur :-)
	double DX = (_l->_guards[0]->_x - _x);
	double DY = (_l->_guards[0]->_y - _y);
	// Define the discretization required for the step
	int L;
	if (DX > DY){
		L = (int) DX;
	} else {
		L = (int) DY;
	}
	double dx = DX / (double) L;
	double dy = DY / (double) L;
	int ix_x, ix_y;
	// Is the Chasseur visible?
	for (int i=1; i<=L; i++) {
		ix_x = (int) (_x + i * dx);
		ix_y = (int) (_y + i * dy);
		if (EMPTY != _l->data(ix_x, ix_y)){
			isChasseurVisible = false;
			return;
		}
	}
	// DEBUGGING: while labyrinths are not correctly formed, we 
	// add the further restriction of guards and chasseurs being
	// at an L2 distance smaller or equal than THRESHOLD to be visible
	double THRESHOLD = 500;
	if (std::sqrt(DX * DX + DY * DY) >= THRESHOLD){
		isChasseurVisible = false;
	}

}



// Actively modify
bool Gardien::process_fireball (float dx, float dy)
{
	std::cout << "Ouch! I've received a Fireball and the code of my reaction is not written haha :-)" << std::endl;

	// Decrease life according to the distance :-)
	// CODE GOES  HERE ;;; TODO
	//
	//
	// Move.h -> Gardien, Chasseur
	//
	Chasseur * pG = (Chasseur*) _l->_guards[0];	
	//double ACC = pG->accuracy; todo: write accuracy
	double ACC = 1;
	if (( ((double) (std::rand() % 1000)) / 1000.) <= ACC){
		double distance_from_fireball = 1;
		lifesigns -= GARDIEN_FIREBALL_DAMAGE * distance_from_fireball;  	
	}
	
	// If life terminates extinguish myself :-)	
	if (lifesigns <= 0){
		std::cout << "Terminating guard 4" << std::endl;
		delete(_l->_guards[4]);
		// Terminate Myself Here
		
	}

		// INFO TO BUILD THE DISTANCE FROM FIREBALL :-)
	
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



