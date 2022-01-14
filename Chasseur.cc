#include "Chasseur.h"
#include "Gardien.h"
#include  <iostream>
#include "Labyrinthe.h"
/*
 *	Tente un deplacement.
 */

bool Chasseur::move_aux (double dx, double dy)
{
	if (EMPTY == _l -> data ((int)((_x + dx) / Environnement::scale),
							 (int)((_y + dy) / Environnement::scale)))
	{
		_x += dx;
		_y += dy;
		return true;
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
 *	Fait bouger la boule de feu (ceci est une exemple, � vous de traiter les collisions sp�cifiques...)
 */

bool Chasseur::process_fireball (float dx, float dy)
{
	// calculer la distance entre le chasseur et le lieu de l'explosion.
	float	x = (_x - _fb -> get_x ()) / Environnement::scale;
	float	y = (_y - _fb -> get_y ()) / Environnement::scale;
	float	dist2 = x*x + y*y;
	// on bouge que dans le vide!
	if (EMPTY == _l -> data ((int)((_fb -> get_x () + dx) / Environnement::scale),
							 (int)((_fb -> get_y () + dy) / Environnement::scale)))
	{
		message ("Woooshh ..... %d", (int) dist2);
		// il y a la place.
		return true;
	}
	// collision...
	// calculer la distance maximum en ligne droite.
	float	dmax2 = (_l -> width ())*(_l -> width ()) + (_l -> height ())*(_l -> height ());
	// faire exploser la boule de feu avec un bruit fonction de la distance.
	_wall_hit -> play (1. - dist2/dmax2);
	message ("Booom...");
	
	
	std::cout << "Process the lifesigns of your enemy: there are " << _l->_nguards <<  std::endl;
	for (int i=1; i<_l->_nguards; i++){
		std::cout << "Checking if its dummy" << std::endl;
		bool dummy = ((Gardien *) _l->_guards[i])->dummy;
		std::cout << "Checking if it has already been killed" << std::endl;
		if ( !(killed.find(i) != killed.end()) && !(dummy)){
			std::cout << "Processing guard  n " << i << std::endl;
			_l->_guards[i]->process_fireball(0,0);
			Gardien *p_temp = (Gardien*) _l->_guards[i];
			std::cout << "Computing lifesigns: " << p_temp->get_lifesigns() << std::endl;
			if (p_temp->get_lifesigns() <= 0){
				std::cout << "Deleting object :-)" << std::endl;
				
				// We don't just delete, we call the 
				// Labyrinth's method for it ;-)
				//delete _l->_guards[i];
				std::cout << "About to destroy Gardien by index" << std::endl;
				((Labyrinthe *) _l)->destroyGardienByIndex(i);
				killed.insert(i);
				std::cout << "Have finished destroying Gardien by index" << std::endl;
			}
		}
	}
	
	return false;
}


/*
 *	Tire sur un ennemi.
 */
void Chasseur::fire (int angle_vertical)
{
	message ("Woooshh...");
	_hunter_fire -> play ();
	_fb -> init (/* position initiale de la boule */ _x, _y, 10.,
				 /* angles de vis�e */ angle_vertical, _angle);
	
	//for (int *p = _l->_guards[1]; p <  + sizeof(ia)/sizeof(*ia); ++p) {
    	//	printf("Char is: %d\n", *p);
	//}
	//
	//
}

/*
 *	Clic droit: par d�faut fait tomber le premier gardien.
 *
 *	Inutile dans le vrai jeu, mais c'est juste pour montrer
 *	une utilisation des fonctions � tomber � et � rester_au_sol �
 */

void Chasseur::right_click (bool shift, bool control)
{
	if (shift)
		_l -> _guards [1] -> rester_au_sol ();
	else
		_l -> _guards [1] -> tomber ();
}
