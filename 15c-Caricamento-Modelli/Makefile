# @^ lista delle dipendenze

CC = g++
CCFLAGS = -O3 -s -DNDEBUG

ifeq ($(OS),Windows_NT)
	BASEDIR = ../base
	INCLUDEDIRS += -I$(BASEDIR)/freeglut/include
	INCLUDEDIRS += -I$(BASEDIR)/glew/include
	INCLUDEDIRS += -I$(BASEDIR)/glm
	INCLUDEDIRS += -I$(BASEDIR)/assimp

	LIBDIRS += -L$(BASEDIR)/freeglut/lib
	LIBDIRS += -L$(BASEDIR)/glew/lib/Release/Win32
	LIBDIRS += -L$(BASEDIR)/assimp/lib

	LIBS += -lfreeglut 
	LIBS += -lopengl32 
	LIBS += -lglew32
	LIBS += -lassimp

else
	LIBS += -lglut 
	LIBS += -lGLEW
	LIBS += -lGL
	LIBS += -lassimp
endif


# File .cpp nella root del progetto
SRCS = main.cpp utilities.cpp common.cpp transform.cpp camera.cpp shaderclass.cpp myshaderclass.cpp myShaderSelect.cpp myShaderWire.cpp
SRCS += light.cpp texture2D.cpp mesh.cpp scena.cpp 

# File .cpp trovati automaticamente in oggetti/
OGG_SRC = $(wildcard oggetti/*.cpp)
UTIL_SRC = $(wildcard util/*.cpp)


# File oggetto generati
OBJS = $(patsubst %.cpp,$(OUTDIR)/%.o,$(SRCS)) \
       $(patsubst oggetti/%.cpp,$(OUTDIR)/%.o,$(OGG_SRC))\
       $(patsubst util/%.cpp,$(OUTDIR)/%.o,$(UTIL_SRC))


# Output directory
OUTDIR = out
EXE = bin/caricamento_modelli.exe
# $@ nome del target
# @^ lista delle dipendenze
$(EXE) : $(OBJS)
	$(CC) $(CCFLAGS) $^ $(LIBDIRS) $(LIBS) -o $@

# $@ file oggetto nella root
# $< sorgente corrispondente
$(OUTDIR)/%.o : %.cpp | $(OUTDIR)
	$(CC) -c $(CCFLAGS) $(INCLUDEDIRS) $? -o $@

# $@ file oggetto da oggetti/
# $< sorgente in oggetti/
$(OUTDIR)/%.o : oggetti/%.cpp | $(OUTDIR)
	$(CC) -c $(CCFLAGS) $(INCLUDEDIRS) $? -o $@

	
# $@ file oggetto da oggetti/
# $< sorgente in oggetti/
$(OUTDIR)/%.o : util/%.cpp | $(OUTDIR)
	$(CC) -c $(CCFLAGS) $(INCLUDEDIRS) $? -o $@

# $@ directory di output
$(OUTDIR):
	mkdir -p $(OUTDIR)



# $@ target per pulizia
.PHONY: clean
clean:
	rm -rf $(OUTDIR)
