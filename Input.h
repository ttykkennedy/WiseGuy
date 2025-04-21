#ifndef INPUT_H
#define INPUT_H

#include "SDL2/SDL.h"
#include "glm/glm.hpp"
#include <unordered_map>
#include <vector>
#include <string>

enum INPUT_STATE
{
    INPUT_STATE_UP,
    INPUT_STATE_JUST_BECAME_DOWN,
    INPUT_STATE_DOWN,
    INPUT_STATE_JUST_BECAME_UP
};

class Input
{
public:
    static void Init();
    static void ProcessEvent(const SDL_Event& e);
    static void LateUpdate();

    static bool GetKey(const std::string& key);
    static bool GetKeyDown(const std::string& key);
    static bool GetKeyUp(const std::string& key);

    static glm::vec2 GetMousePosition();

    static bool GetMouseButton(int button);
    static bool GetMouseButtonDown(int button);
    static bool GetMouseButtonUp(int button);
    static float GetMouseScrollDelta();

    static void HideCursor();
    static void ShowCursor();

private:
    static SDL_Scancode LookupScancode(const std::string& key_name);
    static inline std::unordered_map<SDL_Scancode, INPUT_STATE> keyboard_states;
    static inline float mouse_scroll_this_frame = 0.0f;
    static inline std::unordered_map<int, INPUT_STATE> mouse_button_states;
    static inline glm::vec2 mouse_position{ 0.0f, 0.0f };
};

#endif
