#pragma once

class Input {
public:
    // Callbacks that need to be called explicitly (e.g., from Window or Engine events)
    static void KeyCallback(int key, int scancode, int action, int mods);
    static void MouseButtonCallback(int button, int action, int mods);
    static void ScrollCallback(double xoffset, double yoffset);
    static void CursorPosCallback(double xpos, double ypos);

    // Call once per frame (e.g. at the beginning of the frame update) to update frame-based states
    static void Update();

    // Keyboard queries
    static bool GetKeyDown(int key);       // Held down
    static bool GetKeyUp(int key);         // Released
    static bool GetKeyPressed(int key);    // Pressed exactly on the current frame

    // Mouse button queries
    static bool IsButtonDown(int button);      // Held down
    static bool IsButtonReleased(int button);  // Released
    static bool IsButtonPressed(int button);   // Pressed exactly on the current frame

    // Mouse movement & scroll queries
    static void GetMouseScroll(double& x, double& y);
    static void GetCursorPos(double& x, double& y);
    static void GetCursorDelta(double& x, double& y);
};