#include "Labyrinthe.h"
#include "Chasseur.h"
#include "Gardien.h"
#include <stdnoreturn.h>

//====
//Error handling
#define CHECK(op,code) do { if (op == true) rerror(code);} while(0)

char Rnos[5][64] = {"malloc failed", "realloc failed", "file couldn't be openned", "file wasn't fully read or at all", "The maze is bigger than 257x257"};

void rerror(int code) {
	if (errno != 0) perror(Rnos[code]);
	std::cout << code << std::endl;
    exit(EXIT_FAILURE);
}
//===

Sound*	Chasseur::_hunter_fire;	// bruit de l'arme du chasseur.
Sound*	Chasseur::_hunter_hit;	// cri du chasseur touché.
Sound*	Chasseur::_wall_hit;	// on a tapé un mur.
char monstres[8][16] = {"Blade", "drfreak", "garde", "Lezard", "Marvin", "Potator", "Samourai", "Serpent"};


Environnement* Environnement::init (char* filename) {
	return new Labyrinthe (filename);
}

bool Labyrinthe::is_lab (char c) {
	return (c != ' ' && c != '\n' && c != 'x' && c != 'T' && c != 'G' && c != 'C');
}

void Labyrinthe::print() {
	int height = this->width(), width = this->height();
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


/* Function that processes pre-treatment, finds the maze in the file and it's measurements */
void Labyrinthe::find_measurements (std::ifstream *lab_file, char *line) {
	//trying to find the max length and height of the maze
	int maxw = 0, maxh = 0, nb_textures = 0;
	lab_file->getline(line, 2048);
	
	//we malloc an array to contain the maze only
	char **tab = (char**) malloc(258 * sizeof(char*));
	CHECK((tab == NULL), 0);
	memset(tab, '\0', 258);
	
	//we make use of the lines we read to see if some textures are initialised
	textur **textures = (textur**) malloc(26 * sizeof(textur*));
	CHECK((textures == NULL), 0);
	
	//loop until end of file
	bool plus_flag = false;
	while (!lab_file->eof()) {
		//if getline failed and it has not reached eof
		CHECK((lab_file->fail()), 3);
		int i = 0;
		//we try to extract the layout from the rest of the file
		//commentary inside the layout is currently not handled
		for (; i < 2048 && line[i] != '#' && line[i] != '\n' && line[i] != '\0'; i++) {
			if (line[i] == '+') plus_flag = true;
		}
		for (; i > 0 && !this->is_lab(line[i]); i--);
		maxw = (i > maxw)? i:maxw;
		if (plus_flag == true) {			//then we are in the maze for sure
			this->fill_temp_tab(tab, line, i);
			if (i != 0) maxh+=1;			//not a commentary or useless line then increase width
		} else { 							//else we try to find textures
			char tempc, *temps = (char*) malloc(258);
			CHECK((temps == NULL), 0);
			int tempi = sscanf(line, "%c \t %s", &tempc, temps);
			if (tempi == 2 && tempc != ' ' && tempc != '#') {	//we found a texture
				textures[nb_textures] = (textur*) malloc(sizeof(textur));
				CHECK((textures[nb_textures] == NULL), 0);
				textures[nb_textures]->key = tempc;
				textures[nb_textures]->path = (char *) malloc(258);
				CHECK((textures[nb_textures]->path == NULL), 0);
				strcpy(textures[nb_textures]->path, temps);
				nb_textures++;
			}
			free(temps);
		}
		lab_file->getline(line, 2048);		//move on to next line
	}
	
	//pre-treatment is done now we just need to count
	int nb_boxes = 0, nb_guards = 0, nb_treasures = 0, nb_posters = 0, nb_marks = 0;
	for (int i = 0; i < maxh; i++) {
		for (int j = 0; j < maxw; j++) {
			if (tab[i][j] == ' ') {			//we are trying to see if we put a mark on the ground
				if ((rand()%(maxh*maxh)) < 4) {
					tab[i][j] = 'M';		//we mark the maze there
					nb_marks++;
				}
			}
			else if (tab[i][j] == 'x') nb_boxes++;
			else if (tab[i][j] == 'G') nb_guards++;
			else if (tab[i][j] == 'T') nb_treasures++;	//to verify the validity of the layout
			else if (tab[i][j] > 96 && tab[i][j] < 123) nb_posters++;
		}
	}
	this->_height = maxh;
	this->_width = maxw;
	this->_nboxes = nb_boxes;
	this->_nguards = nb_guards+1;	//nb guards + the hunter
	this->_npicts = nb_posters;
	this->_text = textures;
	this->_nbtext = nb_textures;
	this->_nmarks = nb_marks;
	this->_data = tab;
}

/* Function used to create the data segment of the maze */
void Labyrinthe::fill_temp_tab (char **tab, char *line, int indice) {
	static int j = 0;				//keep in memory the line number
	if (indice != 0 && j < 258) {
		tab[j] = (char*) malloc (258*sizeof(char));
		CHECK((tab[j] == NULL), 0);
		
		CHECK((indice+1 >= 258), 4);
		tab[j][indice+1] = '\0';
		for (; indice >= 0; indice--) {
			tab[j][indice] = line[indice];
		}
		j += 1;
	}
}

/* Function that finds out where walls and posters should be */
void Labyrinthe::walls_create () {
	char **tab = this->_data;
	int limit = 258;
	Wall *walls = (Wall *) malloc(limit*sizeof(Wall));
	CHECK((walls == NULL), 0);
	int nb_walls = 0, height = this->width(), width = this->height(), nb_posters = 0;
	Wall *posters = (Wall *) malloc(this->_npicts * sizeof(Wall));
	CHECK((posters == NULL), 0);
	
	for (int i = 0; i < height; i++) {			//we find horizontal walls first
		int j = 0;
		for (; tab[i][j] != '\0'; j++) {
			if (tab[i][j] == '+') {
				int k = j+1;
				for (; tab[i][k] != '\0' && Labyrinthe::is_lab(tab[i][k]); k++);
				if (j < k-1) {
					if (nb_walls >= limit) {
						limit *= 2;
						walls = (Wall *) realloc(walls, limit*sizeof(Wall));
					}
					walls[nb_walls] = { i, j, i, k-1, 0 };
					nb_walls++;
				}
				j = k-1;
			} else if (tab[i][j] > 96 && tab[i][j] < 123) {		//because of line above we can only find poster vertically first
				if (i > 0 && i+1 < height && (tab[i+1][j] == '|' || tab[i+1][j] == '+') && (tab[i-1][j] == '|' || tab[i-1][j] == '+')) {
					posters[nb_posters] = {i-1, j, i+1, j, 0};
					char tmp [256];
					int l = 0;
					for (; l < this->_nbtext && this->_text[l]->key != tab[i][j]; l++);
					if (l < this->_nbtext) {	//finding path
						sprintf (tmp, "%s/%s", texture_dir, this->_text[l]->path);
						posters[nb_posters]._ntex = wall_texture (tmp);
						nb_posters++;
					} else {		//ignoring if path not explicit
						sprintf (tmp, "%s/%s", texture_dir, "brickwall.jpg");
						posters[nb_posters]._ntex = wall_texture (tmp);
						nb_posters++;
					}
				}
			}
		}
	}
	
	for (int i = 0; i < height; i++) {	//then vertical walls
		int j = 0;
		for (; tab[i][j] != '\0'; j++) {
			if (tab[i][j] == '+') {
				int k = i+1;
				for (; k < height && Labyrinthe::is_lab(tab[k][j]); k++);
				if (i < k-1) {
					if (nb_walls >= limit) {
						limit *= 2;
						walls = (Wall *) realloc(walls, limit*sizeof(Wall));
						CHECK((walls == NULL), 1);
					}
					walls[nb_walls] = { i, j, k-1, j, 0 };
					nb_walls++;
				}
			} else if (tab[i][j] > 96 && tab[i][j] < 123) {		//horizontal posters
				if (j > 0 && j+1 < width && (tab[i][j+1] == '-' || tab[i][j+1] == '+') && (tab[i][j-1] == '-' || tab[i][j-1] == '+')) {
					posters[nb_posters] = {i, j-1, i, j+1, 0};
					char tmp [256];
					int l = 0;
					for (; l < this->_nbtext && this->_text[l]->key != tab[i][j]; l++);
					if (l < this->_nbtext) {	//finding path
						sprintf (tmp, "%s/%s", texture_dir, this->_text[l]->path);
						posters[nb_posters]._ntex = wall_texture (tmp);
						nb_posters++;
					} else {		//ignoring if path not provided
						sprintf (tmp, "%s/%s", texture_dir, "brickwall.jpg");
						posters[nb_posters]._ntex = wall_texture (tmp);
						nb_posters++;
						//especially in the given lab the first seen poster path doesn't exist and we don't handle broken paths
					}
				}
			}
		}
	}
	this->_npicts = nb_posters;
	this->_picts = posters;
	this->_walls = walls;
	this->_nwall = nb_walls;
}

/* Function that initialize the other objects */
void Labyrinthe::objects_create() {
	//We start by initialising/allocating ressources
	char **tab = this->data();
	Box	*boxes = (Box *) malloc(this->_nboxes * sizeof(Box));
	CHECK((boxes == NULL), 0);
	Box *marks = (Box *) malloc(this->_nmarks * sizeof(Box));
	CHECK((marks == NULL), 0);
	this->_guards = new Mover* [this->_nguards + 1];
	int height = this->width(), width = this->height();
	
	int j,i = 0, nb_boxes = 0, nb_guards = 1, nb_marks = 0;
	//guards is initialised to 1 to take into account the player in pos 0
	for (; i < height; i++) {
		for (j = 0; j < width; j++) {
			switch (tab[i][j]) { //we try to find what type of char (object) it is
				case '+' :	//wall
					tab[i][j] = '1';
					break;
				case '-' :	//wall
					tab[i][j] = '1';
					break;
				case '|' :	//wall
					tab[i][j] = '1';
					break;
				case 'x' :	//box
					boxes[nb_boxes]._x = i;
					boxes[nb_boxes]._y = j;
					boxes[nb_boxes]._ntex = 0;
					tab[i][j] = '1';
					nb_boxes++;
					break;
				case 'C' :	//hunter
					this->_guards[0] = new Chasseur (this);
					this->_guards[0]->_x = i*scale;
					this->_guards[0]->_y = j*scale;
					break;
				case 'G' :	//guardian
					{
					char *temps = (char*) malloc(16);
					CHECK((temps == NULL), 0);
					strcpy(temps, monstres[(int)(rand()%8)]);	//random texture
					this->_guards[nb_guards] = new Gardien (this, temps); //TODO
					this->_guards[nb_guards]->_x = i*scale;
					this->_guards[nb_guards]->_y = j*scale;
					tab[i][j] = '1';
					((Gardien *) this->_guards[nb_guards])->give_life();
					nb_guards++;
					break;
					}	//essential because i initialize some values in case
				case 'T' :	//treasure
					this->_treasor._x = i;
					this->_treasor._y = j;
					tab[i][j] = BOUNTY_TAG;
					break;
				case 'M' :	//marks on the ground
					{
					char *temps = (char*) malloc(16);
					CHECK((temps == NULL), 0);
					marks[nb_marks]._x = i;
					marks[nb_marks]._y = j;
					sprintf (temps, "%s/p%i.gif", texture_dir, (int) rand()%4+1);	//random texture
					marks[nb_marks]._ntex = wall_texture(temps);
					tab[i][j] = EMPTY;
					nb_marks++;
					break;
					}
				default :	//the rest
					if (tab[i][j] < 97 || tab[i][j] > 122)
						tab[i][j] = EMPTY;
					break;
			}
		}
	}
	// Define a dummy Gardien
	this->_guards[nb_guards] = new Gardien (this, "Lezard");
	this->_boxes = boxes;
	this->_marks = marks;
}

/* Function called to kill a Gardien by index */
void Labyrinthe::destroyGardienByIndex(int i){
	Mover * local_ptr = this->_guards[i];	// Store the pointer

	// Get its position
	int x = (int) this->_guards[i]->_x / Environnement::scale ;
	int y = (int) this->_guards[i]->_y / Environnement::scale ;

	// Mark the Gardien as killed, and make it fall
	local_ptr->tomber();
	((Gardien *) local_ptr)->remove_life();
	
	// Should we deallocate Gardien's memory? Our last opinion: NO!
	bool DEALLOCATE_GARDIEN = false;
	if (DEALLOCATE_GARDIEN){
		this->_guards[i] = this->_guards[this->_nguards];
		delete local_ptr;
	}

	// Free the space in the map so that Chasseur can walk over it
	this->_data[x][y] = EMPTY;

	// Finally: make it touch the floor
	local_ptr->rester_au_sol();
}


Labyrinthe::Labyrinthe (char* filename) {
	std::ifstream lab_file;
	lab_file.open(filename, std::ifstream::in);
	CHECK((!lab_file.is_open()), 2);
	char *line = (char*) malloc(2048);
	CHECK((line == NULL), 0);
	
	//we first find measurements of the maze and apply pre treatments
	find_measurements(&lab_file, line);
	free(line);
	lab_file.close();
	//we then create the walls
	walls_create ();
	//we fill it with the other objects
	objects_create ();
}

void Labyrinthe::set_data (int i, int j, char value) {
	if (i >= 0 && i < this->width() && j >= 0 && j < this->height()) this->_data[i][j] = value;
}


