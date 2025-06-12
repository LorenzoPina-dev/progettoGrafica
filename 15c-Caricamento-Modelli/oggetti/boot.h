#ifndef BOOT_H
#define BOOT_H

#include "../mesh.h"
#include "sceneObject.h"

//sotto classe di sceneObject che gestisce tutta la logica
class Boot: public sceneObject {
public:
  Boot();
  void init() override;
private:  
  Boot&operator=(const Boot &other);
  Boot(const Boot &other);
};

#endif 