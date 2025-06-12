#ifndef SKULL_H
#define SKULL_H

#include "../mesh.h"

#pragma once
#include "../mesh.h"
#include "sceneObject.h"
#include "../myshaderclass.h"
#include "../transform.h"

class Skull : public sceneObject {
public:
    Skull();

    void init() override;
    Skull&operator=(const Skull &other);

private:
    Skull(const Skull &other);
};

#endif 

