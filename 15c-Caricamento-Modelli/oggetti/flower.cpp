#include "flower.h"
#include "../common.h"
#include "../../base/assimp/assimp/postprocess.h" // Assimp post processing flags
 
Flower::Flower() : sceneObject("Flower") {}


void Flower::init(void) {
  m.load_mesh("models/flower/flower.obj", aiProcess_Triangulate);
  
  //trasformazioni per centrare l'oggetto
  transform.rotate(-90, 0 ,0.0f);
  transform.translate(0, -4,-15);

  initialized = true;
}

