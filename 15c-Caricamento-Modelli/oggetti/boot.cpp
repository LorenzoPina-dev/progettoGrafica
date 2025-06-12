#include "boot.h"
#include "../common.h"
 
Boot::Boot() : sceneObject("Boot") {}

void Boot::init(void) {
  m.load_mesh("models/boot/boot.obj");

  //trasformazioni per centrare l'oggetto
  transform.translate(0,0,-5);
  transform.scale(0.2f);

  initialized = true;
}
