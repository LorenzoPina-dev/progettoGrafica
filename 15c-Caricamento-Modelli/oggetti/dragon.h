#ifndef DRAGON_H
#define DRAGON_H

#include "../mesh.h"
#include "sceneObject.h"

class Dragon : public sceneObject{
public:
  Dragon();
  void init() override;
private:
  Dragon&operator=(const Dragon &other);
  Dragon(const Dragon &other);
};

#endif 