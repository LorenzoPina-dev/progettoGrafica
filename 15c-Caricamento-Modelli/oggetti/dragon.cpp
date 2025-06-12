#include "dragon.h"
#include "../common.h"
 
Dragon::Dragon() : sceneObject("Dragon") {}


void Dragon::init(void) {
  m.load_mesh("models/dragon.obj");
  
  //trasformazioni per centrare l'oggetto
  transform.translate(0,0,-5);

  initialized = true;
}
