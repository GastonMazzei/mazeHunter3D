#include "Gardien.h"
#include  <iostream>

/*
 *	Move
 */

bool Gardien::move (double dx, double dy)
{
	double X = (int) (_x + dx) / Environnement::scale;
	double Y = (int) (_y + dy) / Environnement::scale;
	if ((X<0) || (Y<0) || (X>=_l->height()) || (Y>=_l->width())){
		return false;
	}
	if (EMPTY == _l -> data (
				// 1st argument
				(int) (   X 
				        ),
				 // 2nd argument
			 	 (int)(    Y 
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
	isChasseurVisible = false;
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
	bool response = move((double)  x_moving_trend, (double) y_moving_trend);	
	if (!response){
		x_moving_trend *= -1;
		y_moving_trend *= -1;
	}
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
		if ((ix_x>=0) && (ix_y>=0) && (ix_x<_l->height()) && (ix_y<_l->width())){
			if (EMPTY != _l->data(ix_x, ix_y)){
				isChasseurVisible = false;
				return;
			}
		}
	}
	// Gardiens do not have 'optimal' sight, so anything farther
	// from a THRESHOLD L2 distance from them is not detected ;-)
	double THRESHOLD = 50;
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
	//			 /* angles de vis�e */ angle_vertical, _angle);
	}
}



