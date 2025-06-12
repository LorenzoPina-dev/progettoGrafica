#include "GestoreMouse.h"
#include "../../base/freeglut/include/GL/freeglut.h"

// Inizializzazione statica dei callback
std::function<void(int, int, int)> GestoreMouse::onMouseDown = nullptr;
std::function<void(int, int, int)> GestoreMouse::onMouseUp = nullptr;
std::function<void(int, int, int)> GestoreMouse::onMouseClick = nullptr;

GestoreMouse::MousePosCallback GestoreMouse::onMouseMove = nullptr;
GestoreMouse::MouseMoveCallback GestoreMouse::onMouseDrag = nullptr;
GestoreMouse::MouseMoveCallback GestoreMouse::onMouseRigthDrag = nullptr;
GestoreMouse::MouseMoveCallback GestoreMouse::onMouseLeftDrag = nullptr;
GestoreMouse::MouseMoveCallback GestoreMouse::onMouseWheelDrag = nullptr;
GestoreMouse::MouseWheelCallback GestoreMouse::onMouseWheel = nullptr;
GestoreMouse::VoidCallback GestoreMouse::onMouseEnter = nullptr;
GestoreMouse::VoidCallback GestoreMouse::onMouseLeave = nullptr;

GestoreMouse::MousePosCallback GestoreMouse::onLeftDown = nullptr;
GestoreMouse::MousePosCallback GestoreMouse::onLeftUp = nullptr;
GestoreMouse::MousePosCallback GestoreMouse::onRightDown = nullptr;
GestoreMouse::MousePosCallback GestoreMouse::onRightUp = nullptr;
GestoreMouse::MousePosCallback GestoreMouse::onWheelDown = nullptr;
GestoreMouse::MousePosCallback GestoreMouse::onWheelUp = nullptr;

int GestoreMouse::lastButton = -1;
int GestoreMouse::xClick = -1;
int GestoreMouse::yClick = -1;
bool GestoreMouse::isDown = false;

std::chrono::high_resolution_clock::time_point GestoreMouse::lastFrameTime=std::chrono::high_resolution_clock::now();

void GestoreMouse::inizializza() {
    glutMouseFunc(mouseCallback);
    glutMotionFunc(motionCallback);
    glutPassiveMotionFunc(passiveMotionCallback);
    glutMouseWheelFunc(wheelCallback);
    glutEntryFunc(entryCallback);
}

void GestoreMouse::mouseCallback(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        isDown = true;
        lastButton = button;
        xClick=x;
        yClick=y;
        lastFrameTime=std::chrono::high_resolution_clock::now();

        if (onMouseDown) onMouseDown(button, x, y);

        if (button == GLUT_LEFT_BUTTON && onLeftDown)
            onLeftDown(x, y);
        else if (button == GLUT_RIGHT_BUTTON && onRightDown)
            onRightDown(x, y);
        else if (button == GLUT_MIDDLE_BUTTON && onWheelDown)
            onWheelDown(x, y);

    } else if (state == GLUT_UP) {
        isDown = false;

        if (onMouseUp) onMouseUp(button, x, y);
        if (onMouseClick) onMouseClick(button, x, y);

        if (button == GLUT_LEFT_BUTTON && onLeftUp)
            onLeftUp(x, y);
        else if (button == GLUT_RIGHT_BUTTON && onRightUp)
            onRightUp(x, y);
        else if (button == GLUT_MIDDLE_BUTTON && onWheelUp)
            onWheelUp(x, y);
    }
    
}

void GestoreMouse::motionCallback(int x, int y) {
    if (onMouseDrag) onMouseDrag(xClick,yClick, x, y);
    if(onMouseLeftDrag && isDown && lastButton==GLUT_LEFT_BUTTON) onMouseLeftDrag(xClick,yClick,x,y);
    if(onMouseRigthDrag && isDown && lastButton==GLUT_RIGHT_BUTTON) onMouseRigthDrag(xClick,yClick,x,y);
    if(onMouseWheelDrag && isDown && lastButton==GLUT_MIDDLE_BUTTON) onMouseWheelDrag(xClick,yClick,x,y);
}

void GestoreMouse::passiveMotionCallback(int x, int y) {
    if (onMouseMove) onMouseMove(x, y);
}

void GestoreMouse::resettaPosIniziale(int x,int y){
    xClick=x;
    yClick=y;
}

void GestoreMouse::wheelCallback(int wheel, int direction, int x, int y) {
    if (onMouseWheel) onMouseWheel(direction, x, y);
}

void GestoreMouse::entryCallback(int state) {
    if (state == GLUT_ENTERED && onMouseEnter) onMouseEnter();
    else if (state == GLUT_LEFT && onMouseLeave) onMouseLeave();
}

// Setters
void GestoreMouse::setOnMouseDown(MouseButtonCallback cb) { onMouseDown = cb; }
void GestoreMouse::setOnMouseUp(MouseButtonCallback cb) { onMouseUp = cb; }
void GestoreMouse::setOnMouseClick(MouseButtonCallback cb) { onMouseClick = cb; }

void GestoreMouse::setOnMouseMove(MousePosCallback cb) { onMouseMove = cb; }
void GestoreMouse::setOnMouseDrag(MouseMoveCallback cb) { onMouseDrag = cb; }
void GestoreMouse::setMouseRigthDrag(MouseMoveCallback cb){onMouseRigthDrag= cb;}
void GestoreMouse::setMouseLeftDrag(MouseMoveCallback cb){onMouseLeftDrag= cb;}
void GestoreMouse::setMouseWheelDrag(MouseMoveCallback cb){onMouseWheelDrag= cb;}
void GestoreMouse::setOnMouseWheel(MouseWheelCallback cb) { onMouseWheel = cb; }
void GestoreMouse::setOnMouseEnter(VoidCallback cb) { onMouseEnter = cb; }
void GestoreMouse::setOnMouseLeave(VoidCallback cb) { onMouseLeave = cb; }

void GestoreMouse::setOnLeftDown(MousePosCallback cb) { onLeftDown = cb; }
void GestoreMouse::setOnLeftUp(MousePosCallback cb) { onLeftUp = cb; }
void GestoreMouse::setOnRightDown(MousePosCallback cb) { onRightDown = cb; }
void GestoreMouse::setOnRightUp(MousePosCallback cb) { onRightUp = cb; }

void GestoreMouse::setOnWheelDown(MousePosCallback cb){onWheelDown = cb;}
void GestoreMouse::setOnWheelUp(MousePosCallback cb){onWheelUp = cb;}

