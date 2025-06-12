#ifndef BOOT_H
#define BOOT_H

#include "../mesh.h"
#include "sceneObject.h"
#include <string>

//sotto classe di sceneObject che gestisce tutta la logica
class AlfaObject: public sceneObject {
public:
  AlfaObject(const std::string& nome);
  void setTraslazioneInit(glm::vec3 traslazione);
  void setRotazioneInit(glm::vec3 rotazione);
  void setScaleInit(glm::vec3 scale);
  void setModel(std::string nomeFile, unsigned int flags=0);
  void init() override;
  void render() override;

private:  
  AlfaObject&operator=(const AlfaObject &other);
  AlfaObject(const AlfaObject &other);
  glm::vec3 traslazioneInit,rotazioneInit,scaleInit;
  std::string nomeFile;
  unsigned int flags;
};

#endif 