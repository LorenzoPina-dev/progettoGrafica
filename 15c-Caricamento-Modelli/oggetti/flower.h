#ifndef FLOWER_H
#define FLOWER_H

#include "../mesh.h"
#include "sceneObject.h"

class Flower: public sceneObject {
public:

  Flower();
  void init() override;

private:
  Flower&operator=(const Flower &other);
  Flower(const Flower &other);
};

#endif 