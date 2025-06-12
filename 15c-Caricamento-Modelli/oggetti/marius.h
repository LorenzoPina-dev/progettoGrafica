#ifndef MARIUS_H
#define MARIUS_H

#include "../mesh.h"
#include "sceneObject.h"
#include "../scena.h"

class Marius {
public:

  Marius(scena& s);

private:

  Marius&operator=(const Marius &other);
  Marius(const Marius &other);
};

#endif 