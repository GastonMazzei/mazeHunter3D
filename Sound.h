#ifndef SOUND_H
#define SOUND_H

#ifdef _WIN32
#	include <windows.h>
#include <stdio.h>
#include <pthread.h>
#else
#include "fmod.hpp"
#include "fmod_errors.h"
#endif

#define MAX_SOUNDS	64

class Sound {
private:
#ifdef _WIN32
	char*	_sound;		// buffer contenant l'image du fichier.
#else
	static FMOD::System*	_system;
	static FMOD::Channel*	_channel;
	FMOD::Sound*			_sound;
#endif
	static int				_nsounds;	// compte d'occurrences.

	void init (void);
public:
	Sound (const char*);							// charge un fichier de sons.
	~Sound ();										// libère les échantillons.
	void play (float volume =1., float pan =0.);	// le fait jouer.
};
#endif
