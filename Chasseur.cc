#include "Chasseur.h"
#include "Gardien.h"
#include  <iostream>
#include "Labyrinthe.h"
/*
 *	Tente un deplacement.
 */

bool Chasseur::move_aux (double dx, double dy){


	/*
	 * This function tries to Move the Chasseur to the required place,
	 * but before that it piggybacks a counter that is used to display
	 * on screen the lifesigns and potentially other 'blinking messages'
	 * such as the currently-under-attack Gardien's lifesigns.
	 */
	_counter++;
	
	// 
	if (blinking_message){
		// There is a blinking  mode activated
		if (_counter == last_blinker){
			// The count has been exceeded, terminate blinking
			current_message = " ";
			next_message = " ";
			message(current_message.c_str());
			blinking_message = false;
		} else if (_counter % blinking_period == 0){
			// The time to blink is now
			std::string temp = current_message;
			if (gardienInfo){
				current_message = ("Current lifesigns are: " + 
					std::to_string((int) lifesigns) + 
					"/" + std::to_string((int) CHASSEUR_LIFE ));
			
			} else {
				current_message = next_message;	
			}
			//swtich messages
			next_message = current_message;
		}
	} else if (_counter % 5 == 0) {
		// No blinking pattern on: show lifesigns
		message(("Current lifesigns are: " + 
			std::to_string((int) lifesigns) + 
			"/" + std::to_string((int) CHASSEUR_LIFE )).c_str());
	}

	int X = (int) (_x + dx) / Environnement::scale;
	int Y = (int) (_y + dy) / Environnement::scale;
	
	// We need to make sure he actually moves before checking if the new position is safe
	if (X != ((int) _x/Environnement::scale) || Y != ((int) _y/Environnement::scale)) {
		if (EMPTY == _l -> data (X, Y)) {
			// Move their 'not walking on' bit ('1') inside the maze
			((Labyrinthe *) _l)->set_data ((int) X, (int) Y, '1');
			((Labyrinthe *) _l)->set_data (((int) _x/Environnement::scale), ((int) _y/Environnement::scale), '\0');
			// If the move is allowed: do it, return true
			_x += dx;
			_y += dy;
			return true;
		} else if (BOUNTY_TAG == _l -> data (X, Y)) {
		
			// If the move is not allowed, but that is because the treasure is ahead
			// EXIT CONDITION: you have won, you found the treasure :-)
			std::cout << "CONGRATULATIONS! YOU HAVE WON ;-)" << std::endl;
			exit(0);
		} else return false;	// Move not possible
	} else {
		_x += dx;
		_y += dy;
		return true;
	}
}

/*
 *	Constructeur.
 */

Chasseur::Chasseur (Labyrinthe* l) : Mover (100, 80, l, 0)
{
	// initialise les sons.
	_hunter_fire = new Sound ("sons/hunter_fire.wav");
	_hunter_hit = new Sound ("sons/hunter_hit.wav");
	if (_wall_hit == 0)
		_wall_hit = new Sound ("sons/hit_wall.wav");
}


bool Chasseur::process_fireball (float dx, float dy)
{
	// Compute the normal vector to the Fireball's trajectory
	double Ox = -dy / (std::sqrt(dx*dx+dy*dy));
	double Oy = dx / (std::sqrt(dx*dx+dy*dy));
	if ((((int) (_fb->get_x()+dx + Ox))/Environnement::scale >= _l->width()) ||
		(((int) (_fb->get_y() + dy + Oy))/Environnement::scale >= _l->height())){
		// If the fireball is travelling too close to the walls it should explode
		return false;
	}


	// Check if the fireball can travel with width 3 units of map (i.e. i,j in data[i][j])
	if ((EMPTY == _l -> data (
				(int)((_fb -> get_x () + dx) / Environnement::scale),
				(int)((_fb -> get_y () + dy) / Environnement::scale))
		) &&  
		(EMPTY == _l -> data (
				(int)((_fb -> get_x () + dx + Ox) / Environnement::scale),
				 (int)((_fb -> get_y () + dy + Oy) / Environnement::scale))
		 ) &&
		(EMPTY == _l -> data (
				(int)((_fb -> get_x () + dx - Ox) / Environnement::scale),
				 (int)((_fb -> get_y () + dy - Oy) / Environnement::scale))
		 )
		)
	{
		// If there is nothing relevant being displayed, print "Woooosh..."
		if (!gardienInfo) message ("Woooshh .....");
		return true;
	}
	else {
		// The fireball can't advance. It  should explode!
		float	dmax2 = (_l -> height ())*(_l -> height ()) + (_l -> width ())*(_l -> width ());
		float	x = (_x - _fb -> get_x ()) / Environnement::scale;
		float	y = (_y - _fb -> get_y ()) / Environnement::scale;
		float	dist2 = x*x + y*y;

		_wall_hit -> play (1. - dist2/dmax2);
		current_message = "Boom...";
		message (current_message.c_str());

		// Stop giving priority to Gardien statistics
		gardienInfo = false;
		lowest_lifesigns = GARDIEN_LIFE;

	// Check for every Gardien if they should be affected by the explosion
	for (int i=1; i<_l->_nguards; i++){
		
		// Are they alive?
		bool dummy = ((Gardien *) _l->_guards[i])->get_life();
		
		// Have they already been killed?
		if ( !(killed.find(i) != killed.end()) && !(dummy)){
			
			// Decrease their life
			((Gardien *) _l->_guards[i])->process_fireball_external(0,0);
			Gardien *p_temp = (Gardien*) _l->_guards[i];
			
			// If they should be dead, effectively kill them
			if (p_temp->get_lifesigns() <= 0){
				// To kill a Gardien, we call the 
				// Labyrinth's method for it ;-)
				((Labyrinthe *) _l)->destroyGardienByIndex(i);
				
				// Add to the set of indexes of killed Gardiens
				killed.insert(i);

				// Set up a blinking message saying that you have killed them!
				blinking_message = true;
				next_message = "KILLED";
				message("KILLED");
				_counter = last_blinker/3 ;
			}
		}
	}
		// 'false' terminates the fireball evolution routine
		return false;
	}
}


void Chasseur::add_info2message(std::string information, double lifesigns){
	/*
	 * Function that gets called and 
	 * produces the printing of 
	 * 'information' in the screen.
	 */
	lowest_lifesigns = lifesigns;
	current_message =  information;
	next_message = information;
	blinking_message = true;
	_counter = -25;
	gardienInfo = true;
	message(current_message.c_str());
}


void Chasseur::fire (int angle_vertical){
	/* 
	 * Fire at the enemy
	 */
	message ("Woooshh...");
	_hunter_fire -> play ();
	_fb -> init ( _x, _y, 10., angle_vertical, _angle);
	
}


