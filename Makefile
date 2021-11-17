RM = rm -f
EXE =
OPT = -O3
#
#	Compilation on Windows for Windows
#
ifeq '$(OS)' "Windows_NT"
FLTK_VERSION = fltk-1.4-windows
EXE = .exe
LD = ld
RM = del
CXX = g++
CXXFLAGS =		-Wall $(OPT)

CONSOLE =		-Wl,--subsystem,console
LINKFLTK_IMG =	$(CONSOLE) -mwindows -L$(FLTK_VERSION) \
				-lfltk_images -lfltk_png -lfltk_z -lfltk_jpeg -lfltk_gl -lfltk \
				-lwinmm -lglu32 -lopengl32 -lole32 -luuid \
				-lcomctl32 -lws2_32 -lwinpthread -static

else
#
#	Compilation on MacOSX for MacOSX
#
ifeq '$(shell uname -s)' "Darwin"
FLTK_VERSION = fltk-1.4-macosx
OS = Darwin
FMOD = fmod/Developer/FMOD\ Programmers\ API\ Mac/api
CXX = g++
LD = ld
CXXFLAGS =		-Wall -Wno-invalid-source-encoding -Wno-deprecated-declarations \
				-I /usr/include/freetype2 -I /usr/X11/include -I $(FMOD)/inc $(OPT)

LINKFLTK_IMG =	-L$(FLTK_VERSION) -lfltk_png -lfltk_z -lfltk_jpeg -lfltk_gl \
				-lfltk -framework Cocoa -framework AGL -framework OpenGL \
				-framework ApplicationServices $(FMOD)/lib/libfmodex.dylib
else
#
#	Compilation on Linux for Linux
#
FLTK_VERSION = fltk-1.4-linux
FMOD = fmod/4.3
CXX = g++
LD = ld
CXXFLAGS =		-Wall -I $(FMOD) -I /usr/include/freetype2 -I /usr/X11/include $(OPT)

LINKFLTK_IMG =	$(FMOD)/libfmodex64.so -L$(FLTK_VERSION) -lfltk_jpeg \
				-lfltk_png -lfltk_z -lfltk_gl -lfltk -lGLU -lGL \
				-lXext -lXft -lXrender -lfontconfig -ldl -lm -lX11 -lXfixes
endif
endif

.SUFFIXES: .h .o .cc $(EXE)

.cc.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

#
#	Début du Makefile propre au projet
#

SRC	=	Labyrinthe.cc Chasseur.cc
H	=	Labyrinthe.h Chasseur.h FireBall.h Sound.h Environnement.h \
		Gardien.h Mover.h
O	=	Labyrinthe.o Chasseur.o

ifeq '$(OS)' "Darwin"
OPENGL_O = OpenGL-macosx.o
else
ifeq '$(OS)' "Windows_NT"
OPENGL_O = OpenGL-windows.o
else
OPENGL_O = OpenGL-linux.o
endif
endif

labh$(EXE):	$(O)
	$(CXX) -no-pie -o $@ $(O) $(OPENGL_O) $(LINKFLTK_IMG)
ifeq '$(OS)' "Darwin"
	install_name_tool -change ./libfmodex.dylib fmod/Developer/FMOD\ Programmers\ API\ Mac/api/lib/libfmodex.dylib $@
endif

clean:
	$(RM) labh labh.exe $(O)

Labyrinthe.o:	Labyrinthe.h Environnement.h Chasseur.h Mover.h Gardien.h
Chasseur.o:		Chasseur.cc Chasseur.h Mover.h
