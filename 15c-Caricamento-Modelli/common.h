#ifndef common_H 
#define common_H

#include "../base/glm/glm/glm.hpp"


/**
Struttura dati che contiene gli attributi di un vertice.
*/
struct Vertex {
	glm::vec3 position; ///< Coordinate spaziali
	glm::vec3 normal;   ///< Normale
	glm::vec2 textcoord;///< Coordinate di texture
	float selezionato;//< flag per dire se è selezionato o no

	Vertex();

	Vertex(
  		float x, float y, float z, 
  		float xn, float yn, float zn,
  		float s, float t);

	Vertex(const glm::vec3 &xyz, const glm::vec3 &norm, const glm::vec2 &txt);

};

//enum di utility usati nel progetto
enum ATTRIB_ID {ATTRIB_POSITIONS=0, ATTRIB_NORMALS=1, ATTRIB_COLOR_TEXTURE_COORDS=2,SELECTED=3};

enum TEXTURE_UNIT_ID {TEXTURE_COLOR = 0};
enum assi{f,xy,xz,yz,x,y,z};
enum Trasformazione{none,spostamento,rotazione,scaling};
enum TrasformazioneCamera{NONE,PAN,ROTAZIONE,ZOOM};
enum SelectionMode { ALL, VERTEX, EDGE, FACE };
enum SistemaRiferimento{GLOBALE,LOCALE,CAMERA};
enum Light{AMBIENT,DIRECTIONAL,DIFFUSIVE,SPECULAR,SPECULARSHINE};


#endif