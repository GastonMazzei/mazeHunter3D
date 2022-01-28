#include "Gardien.h"

bool Gardien::move (double dx, double dy){
	/*
	 * Move the gardien by increasing it's _x 
	 * position coordinate by dx and it's _y
	 * position coordinate by dy.
	 */


	// Build the variables that represent the evolved coordinates
	// (in grid-units, i.e. i,j in data[i][j])
	int X = (int) (_x + dx) / Environnement::scale;
	int Y = (int) (_y + dy) / Environnement::scale;
	
	// Protect the program by not allowing coordinates outside the range
	if ((X<0) || (Y<0) || (Y >= _l->height()) || (X >= _l->width())){
		return false;
	}

	// We round up values and if the guardian is over another case by rounding up he is blocking that new case instead
	// We need to make sure he actually moves before checking if the new position is safe
	if (X != ((int) _x/Environnement::scale) || Y != ((int) _y/Environnement::scale)) {
		// Check if the map is empty in the new position
		if (EMPTY == _l -> data (X,Y)){
			// Move their 'not walking on' bit ('1') inside the maze
			((Labyrinthe *) _l)->set_data ((int) X, (int) Y, '1');
			((Labyrinthe *) _l)->set_data (((int) _x/Environnement::scale), ((int) _y/Environnement::scale), '\0');
			// Evolve the Gardien's positions
			_x += dx;
			_y += dy;
			return true;
		}
	} else {
		// Evolve the Gardien's positions
		_x += dx;
		_y += dy;
		return true;
	}
	return false;
}

void Gardien::update_gardien_accuracy(){	
	/*
	 * Update the accuracy by defining it as the maximum possible accuracy
	 * modulated by the lifesigns as a 0-1 number yielded by the 
	 * quotient of the current life against the initial life
	 */
	accuracy = (lifesigns / GARDIEN_LIFE) * GARDIEN_ACCURACY;
}

void Gardien::update_gardien_firingFrequency(){	
	/*
	 * Updates the Gardien Firing Frequency:
	 * 	The firing_frequency is actually a period,
	 * 	so if we name it "T" the function performs:
	 *
	 * 	f(T) = INITIAL_PERIOD * max( { MAX_LIFESIGNS / lifesigns, 1} )
	 * 	
	 * 	note: the units of the period are calls to Gardien::update
	 */

	// Protection 1
	if (firing_frequency == 0) firing_frequency = 1;
	
	// Factor computation
	double factor =  (double) ((double) lifesigns / GARDIEN_LIFE);
        
	// Protection 2
	if (factor <= 1e-2) {
		return;
	}

	// Frequency update
        firing_frequency = (int) ((double) firing_frequency / factor);
        
	// Protection 3
	if (firing_frequency >=  MAX_ALLOWED_FIRING_FREQUENCY ) {
                firing_frequency = MAX_ALLOWED_FIRING_FREQUENCY;
        }
}



void Gardien::update(void){
	/*
	 * Function encoding the Gardien's behaviour.
	 * it is constantly called my OpenGL.
	 */

	// if DEAD: don't do anything
	if (dummy){
		return;
	}
		
	_counter += 1;

	// Update variables based on lifesigns and Chasseur's position
	update_gardien_accuracy();
	update_gardien_firingFrequency();
	update_chasseur_visibility();
	
	// If the chasseur is visible we are angry	
	if (isChasseurVisible){
		angry = true;
	} else {
		angry = false;
	}
	
	// if we are angry and the counter allows it, we fire
	if (angry) {
		if (_counter % firing_frequency == 0){
			fire(180);
		}
	}
	
	// MOVEMENT SECTION
	
	// Movement guard 1:
	// 	if we are closer 'dont_get_closer_than' 
	// 	we are already close enough to Chasseur so don't do anything :-)
	if (stayStill) {
		_angle = 360 -  (int) chasseurTheta; // update Gardien's orientation
		return;
	}
	
	// if we are angry, move towards the Chasseur
	if (angry){
		move_towards_chasseur();
	} else {
		move_randomly();
	}
}


void Gardien::move_towards_chasseur(){
	/*
 	 * Implementation of the function
	 * that makes the Gardien move towards
	 * the Chasseur character (controlled by user).
 	 */	

	// If it's not time to walk, stay still.
	if (_counter % walking_frequency != 0) return;
	

	// Compute the x (DX) and y (DY) elements of
	// the 2D vector that goes from Gardien to Chasseur
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

	// Update the Gardien character's orientation and move accordingly
	_angle = 360 - (int)  chasseurTheta;
	double dx = DX / (double) L;
	double dy = DY / (double) L;
	move(dx,dy);	
}


void Gardien::move_randomly(){
	/*
 	 * Implementation of the function
	 * that makes the Gardien move randomly
 	 */	

	// If it's not time to move, return
	if (_counter % walking_frequency != 0) return;
	
	// Pick a random number between 0 and 1 for x and y, with resolution 1e-6.
	// if it's higher  than the probability of changing direction, invert the
	// respective dimension.
	double xrand = ((double) (std::rand() % 100000)) / 100000.;
	double yrand = ((double) (std::rand() % 100000)) / 100000.;
	if (xrand > probability_change_direction){
		if (x_moving_trend == 1) {
			x_moving_trend = -1;
		} else {
			x_moving_trend = 1;
		}
	}
	if (yrand > probability_change_direction){
		if (y_moving_trend == 1) {
			y_moving_trend = -1;
		} else {
			y_moving_trend = 1;
		}
	}
	
	// Try to move in the computed direction
	bool response = move((double)  x_moving_trend, (double) y_moving_trend);

	// If  it wasn't possible, change direction (180 degrees)
	if (!response){
		x_moving_trend *= -1;
		y_moving_trend *= -1;
	}
}

int Gardien::check_availability(int ix_x, int ix_y){
	/*
	 * Returns 0 or 1, depending on if the point (ix_x, ix_y) can be
	 * transversed by a ray of light or not.
	 */

	// If the point is inside the grid...
	if ((ix_x>=0) && (ix_y>=0) && (ix_x<_l->width()) && (ix_y<_l->height())){
		// If it is not empty
		if (EMPTY != _l->data(ix_x, ix_y)){
			// Chasseur is not visible, return 1
			stayStill = false;
			isChasseurVisible = false;
			return 1;
		}
	} else {
		return 1;
	}
	return 0; // it is O.K., the ray of light can pass.
}

void Gardien::update_chasseur_visibility(void){
	
	// The script tries to negate that the Chasseur is visible
	isChasseurVisible = true;
	
	// This is the x-slope that goes from Gardien to Chasseur :-)
	int xef = (int) (_x / Environnement::scale);
	int yef = (int) (_y / Environnement::scale);
	int xch = (int) (_l->_guards[0]->_x / Environnement::scale);
	int ych = (int) (_l->_guards[0]->_y / Environnement::scale);
	int DX = xch - xef;
	int DY = ych - yef;
	double sgx=1,sgy=1;
	if (DX < 0) {
		DX*=-1;
		sgx = -1;
	}
	if (DY < 0) {
		DY*=-1;
		sgy = -1;
	}
	int L, type;
	double dx,dy;
	if (DX > DY){
		type = 0;
		L = DX;
		dx = 1.0;
		dy = (double) DY / L;
	} else {
		type = 1;
		L = DY;
		dy = 1.0;
		dx = (double) DX / L;
	}
	
	int call_result = 0;
	for (int i=1; i<L; i++) {
		int ix_x1 = (int) ((double) xef + sgx * i * dx);
		int ix_y1 = (int) ((double) yef + sgy * i * dy);
		int ix_x2 = (int) ((double) xef + sgx * (i+1) * dx);
		int ix_y2 = (int) ((double) yef + sgy * (i+1) * dy);
		int ix_x3 = (int) ((double) xef + sgx * (i-1) * dx);
		int ix_y3 = (int) ((double) yef + sgy * (i-1) * dy);
		
		call_result = 0;
		call_result += check_availability(ix_x1, ix_y1);
		if (type == 0){
			call_result += check_availability(ix_x1, ix_y2);
			call_result += check_availability(ix_x1, ix_y3);
		} else {
			call_result += check_availability(ix_x2, ix_y1);
			call_result += check_availability(ix_x3, ix_y1);
		}
		if (call_result >= 1){
			stayStill = false;
			isChasseurVisible = false;
			return;
		}
	}

	// Being here means that Chasseur is indeed visible.
	// Let's compute its angle.
	// theta = arctan(y/x)
	chasseurTheta = 180 / 3.14   * std::atan((_l->_guards[0]->_x - _x)/(_l->_guards[0]->_y - _y)); 
	if ((sgy==-1) && (sgy=sgx)){
		chasseurTheta += 180;
	}
	
	
	// PLUGINS for Gardien behaviour	
	if (isChasseurVisible){

		double distance = std::sqrt(DX * DX + DY * DY);		
		
		// Proximity plugin: 
		// 			Gardiens don't approach Chasseur
		// 			more than a certain distance while
		// 			attacking. 
		if (distance <= dont_get_closer_than){
			stayStill = true;
		} else {
			stayStill = false;
		}
		// Eyesight plugin:
		//
		// 			Gardiens do not have 'optimal' 
		// 			sight, so anything further
		// 			from the eyesight threshold
		// 			is not detected.
		if (distance >= eyesight_reaches_here) {
			isChasseurVisible = false;
		}
	}

}

double Gardien::get_lifesigns(){
	/*
	 * Simple function that returns
	 * Gardien's private attribute
	 * called "lifesigns"
	 */
	return lifesigns;
}

bool Gardien::process_fireball (float dx, float dy) {
	/*
	 * Function to process our own fireball against Chasseur.
	 */

		// if it's dead, don't do anything
	if (dummy) return false;
		
	// Can the fireball keep transversing space?
	if (EMPTY == _l -> data ((int)((_fb -> get_x () + dx) / Environnement::scale),
							 (int)((_fb -> get_y () + dy) / Environnement::scale)))
	{
		// Yes
		return true;
	} else {
		// No. Make it explode and compute how much it hurts the Chasseur.
	
		// Compute distance between fireball and Chasseur
		float	x = (_fb->get_x() - _l->_guards[0]->_x) / Environnement::scale;
		float	y = (_fb->get_y() - _l->_guards[0]->_y) / Environnement::scale;
		float	radius = std::sqrt(x * x + y * y) ;
		if (radius < 1) radius = 1;
		float factor = 1 / radius;

		// If the distance is smaller than the max allowed affection radius, decrease
		// the Chasseur's lifesigns.
		if (radius < MAX_RADIUS_FIREBALL){
			((Chasseur *) _l->_guards[0])->lifesigns -= FIREBALL_DAMAGE * factor;
			double lf = ((Chasseur *) _l->_guards[0])->lifesigns;
			
			// EXIT CONDITION: if the Chasseur's lifesigns have reached zero then exit
			if (lf < 0) {
				exit(0);
				std::cout << "YOU HAVE LOST!" << std::endl;
			}

		}
	}
	// make the fireball explode	
	return false;
}


bool Gardien::process_fireball_external (float dx, float dy) {
	/*
	 * Function called by Chasseur to make the Gardien be affected by the fireball
	 */

	// if Chasseur is dead (dummy==true), return
	if (dummy) return false;


	// Compute distance between Chasseur's fireball and current Gardien
	float	x = (_x - _l->_guards[0]->_fb -> get_x ()) / Environnement::scale;
	float	y = (_y - _l->_guards[0]->_fb -> get_y ()) / Environnement::scale;
	float	radius = std::sqrt(x * x + y * y) ;
	if (radius == 0) radius = 1;

	/* Compute the hitting factor, given by 
	*
	*        { 1       if    1 >= x
	* f(x) = { 1/x     if    1 <= x <= MAX_RADIUS
	*        { 0       if         x >= MAX_RADIUS
	*
	* which has a qualitative drawing like this:
	*
	*        |
	*damage 1|__
	*factor  |  \
	*        |   \
	*       0|    \____
	*        0  1 max_radius
	*/

	float factor = 1/radius;

	if (radius <= MAX_RADIUS_FIREBALL) {

		// Decrease lifesigns
		this->lifesigns -= FIREBALL_DAMAGE * factor;

		// Build a message to be displayed on sreen and send it
		std::string message_to_communicate = ">Ouch! I've been hit (Gardien lifesgins: " + 
							std::to_string((int) this->lifesigns) + "/" + 
							std::to_string(GARDIEN_LIFE) +")";
		((Chasseur *)_l->_guards[0])->add_info2message(message_to_communicate,
								this->lifesigns);
		return true;
	}
		
	return false;
}



void Gardien::fire (int angle_vertical) {
	/*
 	* Attack the hunter.
 	*/
	
	// Get a probability between 0.01 and 1.00
	double randN = ((double) (std::rand() % 100)) / 100.;
	
	// ANGLE is the azimuth. It starts unperturbed
	// being the azimuth that makes the Gardien face
	// the chasseur, i.e. 'chasseurTheta'
	double ANGLE = chasseurTheta;
	
	// if the accuracy is smaller than the 
	// picked number, add a perturbation 
	// both to the azimuth and to the altitude
	if (randN > accuracy){
		ANGLE += (std::rand() % 90 - 45) * (1 - accuracy / GARDIEN_ACCURACY) ;
		angle_vertical += (std::rand() % 40 - 20)  * (1 - accuracy/GARDIEN_ACCURACY);
	}

	// Throw the fireball
	_fb -> init (_x, _y, 10., angle_vertical, ANGLE);
}



