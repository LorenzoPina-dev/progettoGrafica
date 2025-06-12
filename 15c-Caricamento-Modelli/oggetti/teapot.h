#ifndef TEAPOT_H
#define TEAPOT_H

#include "../mesh.h"
#include "sceneObject.h"
#include "../myshaderclass.h"

class Teapot : public sceneObject {
public:
  Teapot();


  void init(void) override;
private:   
  Teapot&operator=(const Teapot &other);
  Teapot(const Teapot &other);
};

#endif 