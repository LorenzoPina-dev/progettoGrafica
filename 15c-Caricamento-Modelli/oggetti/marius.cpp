#include "marius.h"
#include "../common.h"
#include "../../base/assimp/assimp/postprocess.h" // Assimp post processing flags
#include "alfaObject.h"
 
Marius::Marius(scena& s) {
  //trasformazioni iniziali oggetto
  glm::vec3 translate=glm::vec3(0,-17,-8);
  glm::vec3 scale=glm::vec3(10);
  glm::vec3 rotation=glm::vec3(0,180,0);

  //aggiunta di tutte le singole parti come sceneObject in modo da poterle operare come singole
  Mesh* head= new Mesh();
  head->load_mesh("models/marius/head.obj",aiProcess_FlipUVs);
  sceneObject* h=new sceneObject("head",head);
  h->setTranslation(translate);
  h->setRotation(rotation);
  h->setScale(scale);
  s.addObject(h);
  
  Mesh* eyes= new Mesh();
  eyes->load_mesh("models/marius/eyes.obj",aiProcess_FlipUVs);
  sceneObject* e=new sceneObject("eyes",eyes);
  e->setTranslation(translate);
  e->setRotation(rotation);
  e->setScale(scale);
  s.addObject(e);

  AlfaObject* eyebrows= new AlfaObject("eyebrows");
  eyebrows->setModel("models/marius/eyebrows.obj",aiProcess_FlipUVs);
  eyebrows->setTraslazioneInit(translate);
  eyebrows->setRotazioneInit(rotation);
  eyebrows->setScaleInit(scale);
  s.addObject(eyebrows);

  AlfaObject* hairplate= new AlfaObject("hairplate");
  hairplate->setModel("models/marius/hairplate.obj",aiProcess_FlipUVs);
  hairplate->setTraslazioneInit(translate);
  hairplate->setRotazioneInit(rotation);
  hairplate->setScaleInit(scale);
  s.addObject(hairplate);

  AlfaObject* eyelashesUpper= new AlfaObject("eyelashesUpper");
  eyelashesUpper->setModel("models/marius/eyelashesUpper.obj",aiProcess_FlipUVs);
  eyelashesUpper->setTraslazioneInit(translate);
  eyelashesUpper->setRotazioneInit(rotation);
  eyelashesUpper->setScaleInit(scale);
  s.addObject(eyelashesUpper);

  AlfaObject* eyelashesLower= new AlfaObject("eyelashesLower");
  eyelashesLower->setModel("models/marius/eyelashesLower.obj",aiProcess_FlipUVs);
  eyelashesLower->setTraslazioneInit(translate);
  eyelashesLower->setRotazioneInit(rotation);
  eyelashesLower->setScaleInit(scale);
  s.addObject(eyelashesLower);
}

