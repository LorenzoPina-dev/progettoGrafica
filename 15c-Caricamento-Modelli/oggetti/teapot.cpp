#include "teapot.h"
#include "../common.h"
 
Teapot::Teapot() : sceneObject("Skull") {
}

void Teapot::init(void) {
  m.load_mesh("models/teapot.obj");
  transform.translate(0,-1.6,-10);
  initialized = true;
}
