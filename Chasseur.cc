#include "Chasseur.h"
#include "Gardien.h"
#include  <iostream>
#include "Labyrinthe.h"
/*
 *	Tente un deplacement.
 */

bool Chasseur::move_aux (double dx, double dy)
{
	_counter++;
	//message(current_message.c_str());
	if (blinking_message){
		if (_counter == last_blinker){
			current_message = " ";
			next_message = " ";
			message(current_message.c_str());
			blinking_message = false;
		} else if (_counter % blinking_period == 0){
			std::string temp = current_message;
			if (gardienInfo){
				current_message = ("Current lifesigns are: " + 
					std::to_string((int) lifesigns) + 
					"/" + std::to_string((int) CHASSEUR_LIFE ));
			
			} else {
				current_message = next_message;	
			}
			next_message = current_message;
		}
	} else if (_counter % 5 == 0) {
		message(("Current lifesigns are: " + 
			std::to_string((int) lifesigns) + 
			"/" + std::to_string((int) CHASSEUR_LIFE )).c_str());
	}
	if (EMPTY == _l -> data ((int)((_x + dx) / Environnement::scale),
							 (int)((_y + dy) / Environnement::scale)))
	{
		_x += dx;
		_y += dy;
		return true;
	} else if (BOUNTY_TAG == _l -> data ((int)((_x + dx) / Environnement::scale),
							 (int)((_y + dy) / Environnement::scale)))
	
	{
		std::cout << "CONGRATULATIONS! YOU HAVE WON ;-)" << std::endl;
		exit(0);
	}

	return false;
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

/*
 *	Fait bouger la boule de feu (ceci est une exemple, à vous de traiter les collisions spécifiques...)
 */

bool Chasseur::process_fireball (float dx, float dy)
{
	double Ox = -dy / (std::sqrt(dx*dx+dy*dy));
	double Oy = dx / (std::sqrt(dx*dx+dy*dy));
	if ((((int) (_fb->get_x()+dx + Ox))/Environnement::scale >= _l->height()) ||
		(((int) (_fb->get_y() + dy + Oy))/Environnement::scale >= _l->width())){
		std::cout << "dx and dy are " << dx << " " << dy << std::endl;
		std::cout << "Ox and Oy are " << Ox << " " << Oy << std::endl;
		std::cout << "Fireball X and Y are " << _fb->get_x() << " " << _fb->get_y() << std::endl;
		std::cout << "It didnt pass through" << std::endl;
		return false;
	}

	// calculer la distance entre le chasseur et le lieu de l'explosion.
	//
	float	x = (_x - _fb -> get_x ()) / Environnement::scale;
	float	y = (_y - _fb -> get_y ()) / Environnement::scale;
	float	dist2 = x*x + y*y;	
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
		if (!gardienInfo) message ("Woooshh ..... %d", (int) dist2);
		return true;
	}
	else {

	// faire exploser la boule de feu avec un bruit fonction de la distance.
		float	dmax2 = (_l -> width ())*(_l -> width ()) + (_l -> height ())*(_l -> height ());
		_wall_hit -> play (1. - dist2/dmax2);
		current_message = "Boom...";
		message (current_message.c_str());
		gardienInfo = false;
		lowest_lifesigns = GARDIEN_LIFE;
		hasLastBallExploded = true;


	std::cout << "Process the lifesigns of your enemy: there are " << _l->_nguards <<  std::endl;
	for (int i=1; i<_l->_nguards; i++){
		//std::cout << "Checking if its dummy" << std::endl;
		bool dummy = ((Gardien *) _l->_guards[i])->dummy;
		//std::cout << "Checking if it has already been killed" << std::endl;
		if ( !(killed.find(i) != killed.end()) && !(dummy)){
			//std::cout << "Processing guard  n " << i << std::endl;
			((Gardien *) _l->_guards[i])->process_fireball_external(0,0);
			Gardien *p_temp = (Gardien*) _l->_guards[i];
			//std::cout << "Computing lifesigns: " << p_temp->get_lifesigns() << std::endl;
			if (p_temp->get_lifesigns() <= 0){
				std::cout << "Deleting object :-)" << std::endl;
				
				// To kill a Gardien, we call the 
				// Labyrinth's method for it ;-)
				((Labyrinthe *) _l)->destroyGardienByIndex(i);
				killed.insert(i);
				blinking_message = true;
				next_message = "KILLED";
				message("KILLED");
				_counter = last_blinker/3 ;
			}
		}
	}



		return false;
	}
	
	
	
	
}

void Chasseur::add_info2message(std::string information, double lifesigns){
	//if (lifesigns < lowest_lifesigns){
		lowest_lifesigns = lifesigns;
		current_message =  information;
		next_message = information;
		blinking_message = true;
		_counter = -25;
		gardienInfo = true;
		message(current_message.c_str());
	//}
}

/*
 *	Tire sur un ennemi.
 */
void Chasseur::fire (int angle_vertical)
{

	std::cout << " Requested angle_vertical is: " << angle_vertical << std::endl;
	hasLastBallExploded = false;
	message ("Woooshh...");
	_hunter_fire -> play ();
	_fb -> init (/* position initiale de la boule */ _x, _y, 10.,
				 /* angles de visée */ angle_vertical, _angle);
	
	//for (int *p = _l->_guards[1]; p <  + sizeof(ia)/sizeof(*ia); ++p) {
    	//	printf("Char is: %d\n", *p);
	//}
	//
	//
}

/*
 *	Clic droit: par défaut fait tomber le premier gardien.
 *
 *	Inutile dans le vrai jeu, mais c'est juste pour montrer
 *	une utilisation des fonctions « tomber » et « rester_au_sol »
 */

void Chasseur::right_click (bool shift, bool control)
{
	if (shift)
		_l -> _guards [1] -> rester_au_sol ();
	else
		_l -> _guards [1] -> tomber ();
}
