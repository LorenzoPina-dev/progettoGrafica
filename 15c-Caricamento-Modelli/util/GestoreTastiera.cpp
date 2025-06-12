#include "GestoreTastiera.h"
#include "../../base/freeglut/include/GL/freeglut.h"

GestoreTastiera::KeyCallback GestoreTastiera::onKeyDown = nullptr;
GestoreTastiera::KeyCallback GestoreTastiera::onKeyUp = nullptr;

GestoreTastiera::SpecialCallback GestoreTastiera::onSpecialDown = nullptr;
GestoreTastiera::SpecialCallback GestoreTastiera::onSpecialUp = nullptr;

GestoreTastiera::KeyCallback GestoreTastiera::onKeyDownShift = nullptr;
GestoreTastiera::KeyCallback GestoreTastiera::onKeyDownCtrl = nullptr;
GestoreTastiera::KeyCallback GestoreTastiera::onKeyDownAlt = nullptr;

GestoreTastiera::KeyCallback GestoreTastiera::onKeyUpShift = nullptr;
GestoreTastiera::KeyCallback GestoreTastiera::onKeyUpCtrl = nullptr;
GestoreTastiera::KeyCallback GestoreTastiera::onKeyUpAlt = nullptr;


bool GestoreTastiera::shiftPressed = false;
bool GestoreTastiera::ctrlPressed = false;
bool GestoreTastiera::altPressed = false;
bool GestoreTastiera::isPressed = false;
unsigned char GestoreTastiera::lastKey = ' ';

std::map<unsigned int,std::vector<GestoreTastiera::SingleKeyCallback>> GestoreTastiera::eventi=std::map<unsigned int,std::vector<GestoreTastiera::SingleKeyCallback>>();
std::map<unsigned int,std::vector<GestoreTastiera::SingleKeyCallback>> GestoreTastiera::eventiSpeciali=std::map<unsigned int,std::vector<GestoreTastiera::SingleKeyCallback>>();

void GestoreTastiera::inizializza() {
    //inizializzazioni funzioni glut internamente
    glutKeyboardFunc(keyDownCallback);
    glutKeyboardUpFunc(keyUpCallback);
    glutSpecialFunc(specialDownCallback);
    glutSpecialUpFunc(specialUpCallback);
}

void GestoreTastiera::updateModifierState() {
    int mods = glutGetModifiers();
    shiftPressed = mods & GLUT_ACTIVE_SHIFT;
    ctrlPressed  = mods & GLUT_ACTIVE_CTRL;
    altPressed   = mods & GLUT_ACTIVE_ALT;
}

// Setters
void GestoreTastiera::setOnKeyDown(KeyCallback cb) { onKeyDown = cb; }
void GestoreTastiera::setOnKeyUp(KeyCallback cb) { onKeyUp = cb; }
void GestoreTastiera::setOnSpecialDown(SpecialCallback cb) { onSpecialDown = cb; }
void GestoreTastiera::setOnSpecialUp(SpecialCallback cb) { onSpecialUp = cb; }

void GestoreTastiera::setOnKeyDownShift(KeyCallback cb) { onKeyDownShift = cb; }
void GestoreTastiera::setOnKeyDownCtrl(KeyCallback cb) { onKeyDownCtrl = cb; }
void GestoreTastiera::setOnKeyDownAlt(KeyCallback cb) { onKeyDownAlt = cb; }

void GestoreTastiera::setOnKeyUpShift(KeyCallback cb) { onKeyUpShift = cb; }
void GestoreTastiera::setOnKeyUpCtrl(KeyCallback cb) { onKeyUpCtrl = cb; }
void GestoreTastiera::setOnKeyUpAlt(KeyCallback cb) { onKeyUpAlt = cb; }

// Stato
bool GestoreTastiera::isShiftPressed() { return shiftPressed; }
bool GestoreTastiera::isCtrlPressed()  { return ctrlPressed; }
bool GestoreTastiera::isAltPressed()   { return altPressed; }

// Callback di input
//In base a i modificatori o il tasto premuto genera le callback impostate
void GestoreTastiera::keyDownCallback(unsigned char key, int x, int y) {
    updateModifierState();
    lastKey=key;
    isPressed=true;
    if (onKeyDown) onKeyDown(key, x, y);
    if (shiftPressed && onKeyDownShift) onKeyDownShift(key, x, y);
    if (ctrlPressed && onKeyDownCtrl) onKeyDownCtrl(key, x, y);
    if (altPressed && onKeyDownAlt) onKeyDownAlt(key, x, y);
    if(eventi.count(key)){
        for(const auto& evento:eventi[key])
            evento(x,y);
        glutPostRedisplay();
    } 
}

//In base a i modificatori o il tasto premuto genera le callback impostate
void GestoreTastiera::keyUpCallback(unsigned char key, int x, int y) {
    updateModifierState();
    isPressed=false;
    if (onKeyUp) onKeyUp(key, x, y);
    if (shiftPressed && onKeyUpShift) onKeyUpShift(key, x, y);
    if (ctrlPressed && onKeyUpCtrl) onKeyUpCtrl(key, x, y);
    if (altPressed && onKeyUpAlt) onKeyUpAlt(key, x, y);
}

//In base a i modificatori o il tasto premuto genera le callback impostate
void GestoreTastiera::specialDownCallback(int key, int x, int y) {
    updateModifierState();
    if (onSpecialDown) onSpecialDown(key, x, y);
    if(eventiSpeciali.count(key)) {
        for(const auto& evento:eventiSpeciali[key])
            evento(x,y);
        glutPostRedisplay();
    } 
}

//genera le callback impostate
void GestoreTastiera::specialUpCallback(int key, int x, int y) {
    updateModifierState();
    if (onSpecialUp) onSpecialUp(key, x, y);
}

//in base a speciale aggiunge l'evento o alla map per i tasti normali o in quella speciali
void GestoreTastiera::setKeyEvent(int keyCode, SingleKeyCallback evento, bool speciale){
    if(speciale)
        eventiSpeciali[keyCode].push_back(evento);
    else
        eventi[keyCode].push_back(evento);
}