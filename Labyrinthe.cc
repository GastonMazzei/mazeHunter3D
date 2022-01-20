#include "Labyrinthe.h"
#include "Chasseur.h"
#include "Gardien.h"

Sound*	Chasseur::_hunter_fire;	// bruit de l'arme du chasseur.
Sound*	Chasseur::_hunter_hit;	// cri du chasseur touché.
Sound*	Chasseur::_wall_hit;	// on a tapé un mur.

Environnement* Environnement::init (char* filename)
{
	return new Labyrinthe (filename);
}


bool Labyrinthe::is_wall (char c) {
	return (c != ' ' && c != '\n' && c != 'x' && c != 'T' && c != 'G' && c != 'C');
}

void Labyrinthe::print() {
	std::cout << std::endl;
	int height = this->height(), width = this->width();
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (this->_data[i][j] == EMPTY)
				std::cout << " ";
			else
				std::cout << this->_data[i][j];
		}
		std::cout << std::endl;
	}
}

char **Labyrinthe::find_measurements (std::ifstream *lab_file, char *line) {
	//trying to find the max length and height of the maze
	int maxw = 0;
	int maxh = 0;
	lab_file->getline(line, 2048);
	
	//we malloc an array to contain the maze only
	char **tab = (char**) malloc(258 * sizeof(char*));
	memset(tab, '\0', 258);
	
	//loop until end of file
	bool plus_flag = false;
	while (!lab_file->eof()) {
		//if getline failed and it has not reached eof
		if (lab_file->fail()) {
			std::cout << "reading the file failed, either the line is too long (<2048) or the construction of sentry failed" << std::endl;
			return nullptr;
		}
		int i = 0;
		//we try to extract the layout from the rest of the file
		//commentary inside the layout is currently not handled
		for (; i < 2048 && line[i] != '#' && line[i] != '\n' && line[i] != '\0'; i++) {
			if (line[i] == '+') plus_flag = true;
		}
		for (; i > 0 && !this->is_wall(line[i]); i--);
		maxw = (i > maxw)? i:maxw;
		if (plus_flag == true) {
			this->fill_temp_tab(tab, line, i);
			//not a commentary or useless line then increase width
			if (i != 0) maxh+=1;
		}
		lab_file->getline(line, 2048);
	}
	lab_file->seekg(std::ios::beg);
	
	int nb_boxes = 0, nb_guards = 0, nb_treasures = 0, nb_posters = 0;
	for (int i = 0; i < maxh; i++) {
		for (int j = 0; j < maxw; j++) {
			if (tab[i][j] == 'x') nb_boxes++;
			else if (tab[i][j] == 'G') nb_guards++;
			else if (tab[i][j] == 'T') nb_treasures++;	//pour vérifier la validité du layout
			else if (tab[i][j] > 96 && tab[i][j] < 123) nb_posters++;
		}
	}
	this->_height = maxh;
	this->_width = maxw;
	this->_nboxes = nb_boxes;
	this->_nguards = nb_guards+1;	//nb guards + the hunter
	this->_npicts = nb_posters;
	return tab;
}


void Labyrinthe::fill_temp_tab (char **tab, char *line, int indice) {
	static int j = 0;
	if (indice != 0 && j < 258) {
		tab[j] = (char*) malloc (258*sizeof(char));
		
		if (indice+1 >= 258) {
			std::cout << "The maze is bigger than 257x257" << std::endl;
			return;
		}
		tab[j][indice+1] = '\0';
		for (; indice >= 0; indice--) {
			tab[j][indice] = line[indice];
		}
		j += 1;
	}
}


void Labyrinthe::walls_create (char **tab) {
	int limit = 258;
	Wall *walls = (Wall *) malloc(limit*sizeof(Wall));
	int nb_walls = 0;
	int height = this->height();
	for (int i = 0; i < height; i++) {	//we find horizontal walls first
		int j = 0;
		for (; tab[i][j] != '\0'; j++) {
			if (tab[i][j] == '+') {
				int k = j+1;
				for (; tab[i][k] != '\0' && Labyrinthe::is_wall(tab[i][k]); k++);
				if (j < k-1) {
					if (nb_walls >= limit) {
						limit *= 2;
						walls = (Wall *) realloc(walls, limit*sizeof(Wall));
					}
					walls[nb_walls] = { i, j, i, k-1, 0 };
					nb_walls++;
				}
				j = k-1;
			}
		}
	}

	for (int i = 0; i < height; i++) {	//then vertical walls
		int j = 0;
		for (; tab[i][j] != '\0'; j++) {
			if (tab[i][j] == '+') {
				int k = i+1;
				for (; k < height && Labyrinthe::is_wall(tab[k][j]); k++);
				if (i < k-1) {
					if (nb_walls >= limit) {
						limit *= 2;
						walls = (Wall *) realloc(walls, limit*sizeof(Wall));
						std::cout << "walls need to be realloc'ed\n";
					}
					walls[nb_walls] = { i, j, k-1, j, 0 };
					nb_walls++;
				}
			}
		}
	}
	this->_walls = walls;
	this->_nwall = nb_walls;
}


void Labyrinthe::objects_create(char **tab) {
	Box	*boxes = (Box *) malloc(this->_nboxes * sizeof(Box));
	this->_guards = new Mover* [this->_nguards + 1];
	int height = this->height();
	int width = this->width();
	
	int j,i = 0, nb_boxes = 0, nb_guards = 1;
	for (; i < height; i++) {
		for (j = 0; j < width; j++) {
			switch (tab[i][j]) { //we try to find what type of char it is
				case '+' :
					tab[i][j] = '1';
					break;
				case '-' :
					tab[i][j] = '1';
					break;
				case '|' :
					tab[i][j] = '1';
					break;
				case 'x' :
					boxes[nb_boxes]._x = i;
					boxes[nb_boxes]._y = j;
					boxes[nb_boxes]._ntex = 0;
					tab[i][j] = '1';
					nb_boxes++;
					break;
				case 'C' :
					this->_guards[0] = new Chasseur (this);
					this->_guards[0]->_x = i*scale;
					this->_guards[0]->_y = j*scale;
					break;
				case 'G' :
					this->_guards[nb_guards] = new Gardien (this, "Lezard");
					this->_guards[nb_guards]->_x = i*scale;
					this->_guards[nb_guards]->_y = j*scale;
					tab[i][j] = '1';
					((Gardien *) this->_guards[nb_guards])->give_life();
					nb_guards++;
					break;
				case 'T':
					this->_treasor._x = i;
					this->_treasor._y = j;
					tab[i][j] = BOUNTY_TAG;
					break;
				default :
					if (tab[i][j] < 97 || tab[i][j] > 122)
						tab[i][j] = EMPTY;
					break;
			}
		}
	}
	// Define a dummy Gardien
	this->_guards[nb_guards] = new Gardien (this, "Lezard");
	std::cout << "There are " << nb_guards << " guards in the entire scene" << std::endl;

	this->_boxes = boxes;
}


/*void Labyrinthe::poster_create(char**tab) {
	Wall *posters = (Wall *) malloc(NB_BOXES * sizeof(Wall));
	
	int limit = NB_BOXES;
	//annoying to take care of realloc and since it's not core to the game I'll just let it be for now
	int height = this->height();
	int width = this->width();
	
	int j,i = 0, nb_boxes = 0;
	for (; i < height; i++) {
		for (j = 0; j < width; j++) {
			else if (tab[i][j] > 96 && tab[i][j] < 123) {
				if (nb_posters < limit) {
					posters[nb_boxes] = (Wall *) malloc(sizeof(Wall));
					posters[nb_boxes]->_x1 = i;
					posters[nb_boxes]->_x2 = i;
					posters[nb_boxes]->_y1 = j;
					posters[nb_boxes]->_y2 = j;
					posters[nb_boxes]->_ntex = 0;
					nb_posters++;
				}
			}
		}
	}
}*/

void Labyrinthe::destroyGardienByIndex(int i){

	// Store the pointer
	Mover * local_ptr = this->_guards[i];

	// Update the Gardien to the dummy one
	//this->_guards[i] = this->_guards[this->_nguards];

	// Get its position
	int x = (int) this->_guards[i]->_x / Environnement::scale ;
	int y = (int) this->_guards[i]->_y / Environnement::scale ;

	// Update the Gardien
	local_ptr->tomber();
	((Gardien *) local_ptr)->remove_life();
	
	// Delete the Gardien
	//delete local_ptr;

	// Free the space in the data
	this->_data[x][y] = EMPTY;

	// Rester au floor
	local_ptr->rester_au_sol();
}


Labyrinthe::Labyrinthe (char* filename)
{
	std::ifstream lab_file;
	lab_file.open(filename, std::ifstream::in);
	
	if(!lab_file.is_open()) {
		// need to crash with error message
		std::cout << "filename for maze layout not found or not opennable" << std::endl;
		return;
	}
	
	char *line;
	if ((line = (char*) malloc(2048)) == NULL) {
		std::cout << "malloc crash" << std::endl;
		return;
	}
	
	char **tab = this->find_measurements(&lab_file, line);
	std::cout << "walls_create" << std::endl;
	walls_create (tab);
	std::cout << "end walls_create" << std::endl;
	objects_create (tab);
	
	this->_data = tab;
	this->print();

	// une affiche.
	//  (attention: pour des raisons de rapport d'aspect, les affiches doivent faire 2 de long)
	/*static Wall affiche [] = {
		{ 4, 0, 6, 0, 0 },		// première affiche.
		{ 8, 0, 10, 0, 0 },		// autre affiche.
	};*/

/* DEB - NOUVEAU */
	// 2 marques au sol.
	static Box	marques [] = {
		{ 50, 14, 0 },
		{ 20, 15, 0 },
	};
/* FIN - NOUVEAU */

	// deux affiches.
	_npicts = 0;
	//_picts = affiche;
	// la deuxième à une texture différente.
	char	tmp [128];
	//sprintf (tmp, "%s/%s", texture_dir, "voiture.jpg");
	//_picts [1]._ntex = wall_texture (tmp);
	//_boxes = caisses;

/* DEB - NOUVEAU */
	// mettre une autre texture à la deuxième caisse.
	//sprintf (tmp, "%s/%s", texture_dir, "boite.jpg");
	//caisses [1]._ntex = wall_texture (tmp);

	// mettre les marques au sol.
	sprintf (tmp, "%s/%s", texture_dir, "p1.gif");
	marques [0]._ntex = wall_texture (tmp);
	sprintf (tmp, "%s/%s", texture_dir, "p3.gif");
	marques [1]._ntex = wall_texture (tmp);
	_nmarks = 2;
	_marks = marques;
/* FIN - NOUVEAU */

	std::cout << "coucou fin" << std::endl;
}

