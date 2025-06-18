/*

  Copyright 2011 Etay Meiri

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MESH_H
#define MESH_H
#define GLM_FORCE_RADIANS
class aiScene;
#include "../base/glew/include/GL/glew.h"      //glew.h>
#include "../base/glm/glm/gtc/matrix_transform.hpp"
#include "../base/glm/glm/glm.hpp"
#include "common.h"
#include "texture2D.h"
#include <algorithm>
#include <cstring>
#include <ostream>
#include <unordered_map>
#include <array>
#include <vector>

/**
    Classe che incapsula la gestione dei modelli 3d caricati da file.
    La classe usa la lista dei vertici indicizzati.
    Al momento la classe supporta modelli con una sola texture colore.
    Se il modello non ha una texture associata, viene usata una texture
    di default "white.png"
*/
class Mesh {
public:
  /**
      Struttura dati cpon gli attributi dei vertici
  */
  /*
  struct Vertex
  {
      glm::vec3 position; ///< Coordinate spaziali
      glm::vec3 normal;   ///< Normale
      glm::vec2 textcoord;///< Coordinate di texture
	    float selezionato;//< flag per dire se è selezionato o no

      Vertex();

      Vertex(const glm::vec3& p, const glm::vec3& n, const glm::vec2& t);
  };
*/

  Mesh();

  ~Mesh();

  /**
      Funzione che carica il modello e lo prepara per il rendering.

      @param filename nome del file
      @param flags assimp post processing flags

      @return true se il modello è stato caricato correttamente
  */
  bool load_mesh(const std::string &Filename, unsigned int flags = 0);

  /*
    metodi per applicare una trasformazione su una mesh
  */
  void applyTransformationToVertices(const std::vector<unsigned int> &indices,
                                     const glm::mat4 &transform,
                                     const glm::vec3 &origin=glm::vec3(0),
                                     const glm::mat3 &localAxes= glm::mat3(1.0f));
  void addTransformationToVertices(const std::vector<unsigned int> &indices,
                                   const glm::mat4 &transform,
                                   const glm::vec3 &origin=glm::vec3(0),
                                   const glm::mat3 &localAxes= glm::mat3(1.0f));

  //gestione selezione dato id dei vertici permette di gestire la selezione
  void toggleSelected(unsigned int id);
  void setSelected(unsigned int id, bool valore);
  void clearSelected(const std::vector<unsigned int> &indices);

  /**
      Renderizza l'oggetto in scena usando di default
      l'id 0 per l'attributo posizione (ATTRIB_POSITIONS)
      l'id 1 per l'attributo vettori normali (ATTRIB_NORMALS)
      l'id 2 per l'attributo coordinate di texture (ATTRIB_COLOR_TEXTURE_COORDS)
      l'id 3 per l'attributo selezionato (SELECTED)

      la TextureUnit 0 per la texture colore (TEXTURE_COLOR).
  */
  void render(void);

  std::vector<Vertex> getVertices();

  std::vector<unsigned int> getIndices();

  //dati i 3 vertici permette di ottenere l'id della faccia a cui appartengono
  unsigned int verifica_faccia(unsigned int i1, unsigned int i2,
                               unsigned int i3) const;

  //data id della faccia ottiene i 3 vertici che la compongono
  std::array<unsigned int, 3> getFaceVertexIndices(unsigned int faceID) const;
  
  //dati 2 id di vertici permette di capire se i 2 vertici appartengono allo stesso edge                          
  unsigned int verifica_edge(unsigned int i1, unsigned int i2) const;

  //salva la posizione dei vertici e le loro normali
  void saveState();

  //ottiene la posizione del centroide calcolato sui verciti aventi gli id nel vettore
  glm::vec3 getCentroid(const std::vector<unsigned int> &indices);
  
  //ottiene la normale del centroide calcolato sui verciti aventi gli id nel vettore
  glm::vec3 getCentroidNormal(const std::vector<unsigned int> &indices);

private:

  bool init_from_scene(const aiScene *pScene, const std::string &Filename);

  std::vector<Vertex> Vertices;
  std::vector<unsigned int> Indices;

  //mappa che dagli gli indici dei verttici li mappa negli id delle facce in cui appaiono
  std::unordered_map<unsigned int, std::vector<unsigned int>> vertIndexToFacePositions;

  //mappe usate per salvare posizione e normali di partenza per la fase di trasformazioni
  std::unordered_map<unsigned int, glm::vec3> verticiIniziali;
  std::unordered_map<unsigned int, glm::vec3> normaliIniziali;

  bool _modified = false;

  std::string get_file_path(const std::string &Filename) const;

  unsigned int _num_indices;
  Texture2D _texture;
  GLuint _VAO;
  GLuint _VBO;
  GLuint _IBO;
  // GLuint _ssboSelected = 0;      // Buffer SSBO per selezione
  // std::vector<float> selectedData;

  Mesh(const Mesh &other);
  void updateSelectedSSBO();
};

std::ostream &operator<<(std::ostream &os, const Vertex &v);

#endif /* MESH_H */
