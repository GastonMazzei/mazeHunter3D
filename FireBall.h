#ifndef FIREBALL_H
#define FIREBALL_H

#ifndef M_PI
#define M_PI 3.1415926
#endif

class Mover;

enum FBstat {
	FB_NONE, FB_MOVE, FB_EXPLODE,
};

class FireBall {
private:
	float	_x, _y, _z;		// position de la boule.
	float	_size;			// sa taille actuelle.
	float	_orig_size;		// sa taille à l'origine.
	int		_angle;			// angle (elle tourne sur elle-même).
	int		_angle2;		// angle (elle tourne sur elle-même sur un autre axe).
	unsigned int	_fire_texture;	// sa texture.
	int		_move_angle;	// angle de déplacement.
	int		_azimuth;		// le même en hauteur.
	Mover*	_owner;			// le proprio.

	static unsigned int	_fire_list;		// sa display list.
	FBstat	_state;		// état de la boule.
	void explode ();

public:
	FireBall (float size, unsigned int tex, Mover*);
	void display ();
	void move_step ();
	// initialise un tir.
	void init (float x, float y, float z, int angle_vertical, int angle_horizontal);
	// décide de la taille initiale de la boule.
	void set_orig_size (float size) { _orig_size = size; }
	// récupère la position actuelle de la boule.
	float get_x () { return _x; }
	float get_y () { return _y; }
};

#endif
