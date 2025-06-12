// MouseHandler.h
#ifndef MOUSE_HANDLER_H
#define MOUSE_HANDLER_H

#include <functional>
#include <chrono>
#include "../../base/glm/glm/glm.hpp"

class GestoreMouse {
public:
    using MouseButtonCallback = std::function<void(int button, int x, int y)>;
    using MouseMoveCallback   = std::function<void(int xClick,int yClick,int x, int y)>;
    using MouseWheelCallback  = std::function<void(int delta, int x, int y)>;
    using MousePosCallback  = std::function<void(int x, int y)>;
    using VoidCallback        = std::function<void()>;

    static void inizializza();

    // Eventi standard
    static void setOnMouseDown(MouseButtonCallback cb);
    static void setOnMouseUp(MouseButtonCallback cb);
    static void setOnMouseClick(MouseButtonCallback cb);
    static void setOnMouseMove(MousePosCallback cb);
    static void setOnMouseDrag(MouseMoveCallback cb);
    static void setOnMouseWheel(MouseWheelCallback cb);
    static void setOnMouseEnter(VoidCallback cb);
    static void setOnMouseLeave(VoidCallback cb);

    // Eventi specifici
    static void setMouseRigthDrag(MouseMoveCallback cb);
    static void setMouseLeftDrag(MouseMoveCallback cb);
    static void setMouseWheelDrag(MouseMoveCallback cb);
    static void setOnLeftDown(MousePosCallback cb);
    static void setOnLeftUp(MousePosCallback cb);
    static void setOnRightDown(MousePosCallback cb);
    static void setOnRightUp(MousePosCallback cb);
    static void setOnWheelDown(MousePosCallback cb);
    static void setOnWheelUp(MousePosCallback cb);
    
    static void resettaPosIniziale(int x,int y);
private:
    static MouseButtonCallback onMouseDown;
    static MouseButtonCallback onMouseUp;
    static MouseButtonCallback onMouseClick;

    static MousePosCallback onMouseMove;
    static MouseMoveCallback onMouseDrag;
    static MouseMoveCallback onMouseLeftDrag;
    static MouseMoveCallback onMouseRigthDrag;
    static MouseMoveCallback onMouseWheelDrag;
    static MouseWheelCallback onMouseWheel;
    static VoidCallback onMouseEnter;
    static VoidCallback onMouseLeave;

    static MousePosCallback onLeftDown;
    static MousePosCallback onLeftUp;
    static MousePosCallback onRightDown;
    static MousePosCallback onRightUp;
    static MousePosCallback onWheelDown;
    static MousePosCallback onWheelUp;

    static int lastButton;
    static bool isDown;
    static int xClick;
    static int yClick;
    
    static std::chrono::high_resolution_clock::time_point lastFrameTime;

    static void mouseCallback(int button, int state, int x, int y);
    static void motionCallback(int x, int y);
    static void passiveMotionCallback(int x, int y);
    static void wheelCallback(int wheel, int direction, int x, int y);
    static void entryCallback(int state);
};

#endif
