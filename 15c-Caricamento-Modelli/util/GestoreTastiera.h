#ifndef GESTORE_TASTIERA_H
#define GESTORE_TASTIERA_H

#include <functional>
#include <map>
#include <algorithm>
#include <vector>

class GestoreTastiera {
public:
    using KeyCallback = std::function<void(unsigned char key, int x, int y)>;
    using SpecialCallback = std::function<void(int key, int x, int y)>;
    using SingleKeyCallback = std::function<void(int x, int y)>;

    static void inizializza();

    // Eventi base
    static void setOnKeyDown(KeyCallback cb);
    static void setOnKeyUp(KeyCallback cb);
    static void setOnSpecialDown(SpecialCallback cb);
    static void setOnSpecialUp(SpecialCallback cb);

    // Eventi con modificatori
    static void setOnKeyDownShift(KeyCallback cb);
    static void setOnKeyDownCtrl(KeyCallback cb);
    static void setOnKeyDownAlt(KeyCallback cb);

    static void setOnKeyUpShift(KeyCallback cb);
    static void setOnKeyUpCtrl(KeyCallback cb);
    static void setOnKeyUpAlt(KeyCallback cb);

    // Stato dei modificatori
    static bool isShiftPressed();
    static bool isCtrlPressed();
    static bool isAltPressed();

    //Aggiunta eventi al key
    static void setKeyEvent(int keyCode, SingleKeyCallback evento, bool speciale=false);

private:
    static KeyCallback onKeyDown;
    static KeyCallback onKeyUp;
    static KeyCallback onKeyDownShift;
    static KeyCallback onKeyDownCtrl;
    static KeyCallback onKeyDownAlt;

    static KeyCallback onKeyUpShift;
    static KeyCallback onKeyUpCtrl;
    static KeyCallback onKeyUpAlt;

    static SpecialCallback onSpecialDown;
    static SpecialCallback onSpecialUp;

    static bool shiftPressed;
    static bool ctrlPressed;
    static bool altPressed;
    static unsigned char lastKey;
    static bool isPressed;
    static std::map<unsigned int,std::vector<SingleKeyCallback>> eventi;
    static std::map<unsigned int,std::vector<SingleKeyCallback>> eventiSpeciali;

    //eventi base glut
    static void keyDownCallback(unsigned char key, int x, int y);
    static void keyUpCallback(unsigned char key, int x, int y);
    static void specialDownCallback(int key, int x, int y);
    static void specialUpCallback(int key, int x, int y);

    static void updateModifierState();
};

#endif
