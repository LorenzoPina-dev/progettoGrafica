#include "skull.h"
#include "../common.h"
#include "../../base/glm/glm/glm.hpp"
#include "../../base/assimp/assimp/postprocess.h" // Assimp post processing flags
#include <iostream>
 
Skull::Skull() : sceneObject("Skull") {
}
void Skull::init() {
    m.load_mesh("models/skull.obj", aiProcess_FlipUVs | aiProcess_Triangulate);
    transform.translate(0,-5,0);
    initialized = true;
}

