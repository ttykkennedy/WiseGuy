#include "Input.h"
#include <algorithm>
#include <iostream>
#include <string>

static const std::unordered_map<std::string, SDL_Scancode> __keycode_to_scancode =
{
    {"up", SDL_SCANCODE_UP},
    {"down", SDL_SCANCODE_DOWN},
    {"left", SDL_SCANCODE_LEFT},
    {"right", SDL_SCANCODE_RIGHT},

    {"escape", SDL_SCANCODE_ESCAPE},

    {"lshift", SDL_SCANCODE_LSHIFT},
    {"rshift", SDL_SCANCODE_RSHIFT},
    {"lctrl", SDL_SCANCODE_LCTRL},
    {"rctrl", SDL_SCANCODE_RCTRL},
    {"lalt", SDL_SCANCODE_LALT},
    {"ralt", SDL_SCANCODE_RALT},

    {"tab", SDL_SCANCODE_TAB},
    {"return", SDL_SCANCODE_RETURN},
    {"enter", SDL_SCANCODE_RETURN},
    {"backspace", SDL_SCANCODE_BACKSPACE},
    {"delete", SDL_SCANCODE_DELETE},
    {"insert", SDL_SCANCODE_INSERT},

    {"space", SDL_SCANCODE_SPACE},
    {"a", SDL_SCANCODE_A},
    {"b", SDL_SCANCODE_B},
    {"c", SDL_SCANCODE_C},
    {"d", SDL_SCANCODE_D},
    {"e", SDL_SCANCODE_E},
    {"f", SDL_SCANCODE_F},
    {"g", SDL_SCANCODE_G},
    {"h", SDL_SCANCODE_H},
    {"i", SDL_SCANCODE_I},
    {"j", SDL_SCANCODE_J},
    {"k", SDL_SCANCODE_K},
    {"l", SDL_SCANCODE_L},
    {"m", SDL_SCANCODE_M},
    {"n", SDL_SCANCODE_N},
    {"o", SDL_SCANCODE_O},
    {"p", SDL_SCANCODE_P},
    {"q", SDL_SCANCODE_Q},
    {"r", SDL_SCANCODE_R},
    {"s", SDL_SCANCODE_S},
    {"t", SDL_SCANCODE_T},
    {"u", SDL_SCANCODE_U},
    {"v", SDL_SCANCODE_V},
    {"w", SDL_SCANCODE_W},
    {"x", SDL_SCANCODE_X},
    {"y", SDL_SCANCODE_Y},
    {"z", SDL_SCANCODE_Z},
    {"0", SDL_SCANCODE_0},
    {"1", SDL_SCANCODE_1},
    {"2", SDL_SCANCODE_2},
    {"3", SDL_SCANCODE_3},
    {"4", SDL_SCANCODE_4},
    {"5", SDL_SCANCODE_5},
    {"6", SDL_SCANCODE_6},
    {"7", SDL_SCANCODE_7},
    {"8", SDL_SCANCODE_8},
    {"9", SDL_SCANCODE_9},
    {"/", SDL_SCANCODE_SLASH},
    {";", SDL_SCANCODE_SEMICOLON},
    {"=", SDL_SCANCODE_EQUALS},
    {"-", SDL_SCANCODE_MINUS},
    {".", SDL_SCANCODE_PERIOD},
    {",", SDL_SCANCODE_COMMA},
    {"[", SDL_SCANCODE_LEFTBRACKET},
    {"]", SDL_SCANCODE_RIGHTBRACKET},
    {"\\", SDL_SCANCODE_BACKSLASH},
    {"'", SDL_SCANCODE_APOSTROPHE},
};

SDL_Scancode Input::LookupScancode(const std::string& key_name)
{
    auto it = __keycode_to_scancode.find(key_name);
    if (it == __keycode_to_scancode.end())
    {
        return SDL_SCANCODE_UNKNOWN;
    }
    return it->second;
}

void Input::Init()
{
    keyboard_states.clear();
    mouse_button_states.clear();
    mouse_scroll_this_frame = 0.0f;
    mouse_position = glm::vec2(0.0f, 0.0f);
}

void Input::ProcessEvent(const SDL_Event& e)
{
    switch (e.type)
    {
    case SDL_KEYDOWN:
    {
        SDL_Scancode code = e.key.keysym.scancode;
        auto it = keyboard_states.find(code);
        if (it == keyboard_states.end())
        {
            keyboard_states[code] = INPUT_STATE_JUST_BECAME_DOWN;
        }
        else
        {
            if (it->second == INPUT_STATE_UP || it->second == INPUT_STATE_JUST_BECAME_UP)
            {
                it->second = INPUT_STATE_JUST_BECAME_DOWN;
            }
        }
        break;
    }
    case SDL_KEYUP:
    {
        SDL_Scancode code = e.key.keysym.scancode;
        auto it = keyboard_states.find(code);
        if (it == keyboard_states.end())
        {
            keyboard_states[code] = INPUT_STATE_JUST_BECAME_UP;
        }
        else
        {
            if (it->second == INPUT_STATE_DOWN || it->second == INPUT_STATE_JUST_BECAME_DOWN)
            {
                it->second = INPUT_STATE_JUST_BECAME_UP;
            }
        }
        break;
    }
    case SDL_MOUSEMOTION:
    {
        mouse_position.x = static_cast<float>(e.motion.x);
        mouse_position.y = static_cast<float>(e.motion.y);
        break;
    }
    case SDL_MOUSEBUTTONDOWN:
    {
        int btn = static_cast<int>(e.button.button);
        auto it2 = mouse_button_states.find(btn);
        if (it2 == mouse_button_states.end())
        {
            mouse_button_states[btn] = INPUT_STATE_JUST_BECAME_DOWN;
        }
        else
        {
            if (it2->second == INPUT_STATE_UP || it2->second == INPUT_STATE_JUST_BECAME_UP)
            {
                it2->second = INPUT_STATE_JUST_BECAME_DOWN;
            }
        }
        break;
    }
    case SDL_MOUSEBUTTONUP:
    {
        int btn = static_cast<int>(e.button.button);
        auto it2 = mouse_button_states.find(btn);
        if (it2 == mouse_button_states.end())
        {
            mouse_button_states[btn] = INPUT_STATE_JUST_BECAME_UP;
        }
        else
        {
            if (it2->second == INPUT_STATE_DOWN || it2->second == INPUT_STATE_JUST_BECAME_DOWN)
            {
                it2->second = INPUT_STATE_JUST_BECAME_UP;
            }
        }
        break;
    }
    case SDL_MOUSEWHEEL:
    {
        mouse_scroll_this_frame = e.wheel.preciseY;
        break;
    }
    default:
        break;
    }
}

void Input::LateUpdate()
{
    mouse_scroll_this_frame = 0.0f;

    for (auto& kv : keyboard_states)
    {
        if (kv.second == INPUT_STATE_JUST_BECAME_DOWN)
        {
            kv.second = INPUT_STATE_DOWN;
        }
        else if (kv.second == INPUT_STATE_JUST_BECAME_UP)
        {
            kv.second = INPUT_STATE_UP;
        }
    }

    for (auto& kv : mouse_button_states)
    {
        if (kv.second == INPUT_STATE_JUST_BECAME_DOWN)
        {
            kv.second = INPUT_STATE_DOWN;
        }
        else if (kv.second == INPUT_STATE_JUST_BECAME_UP)
        {
            kv.second = INPUT_STATE_UP;
        }
    }
}

bool Input::GetKey(const std::string& key)
{
    SDL_Scancode sc = LookupScancode(key);
    if (sc == SDL_SCANCODE_UNKNOWN) return false;

    auto it = keyboard_states.find(sc);
    if (it == keyboard_states.end()) return false;
    return (it->second == INPUT_STATE_DOWN || it->second == INPUT_STATE_JUST_BECAME_DOWN);
}

bool Input::GetKeyDown(const std::string& key)
{
    SDL_Scancode sc = LookupScancode(key);
    if (sc == SDL_SCANCODE_UNKNOWN) return false;

    auto it = keyboard_states.find(sc);
    if (it == keyboard_states.end()) return false;
    return (it->second == INPUT_STATE_JUST_BECAME_DOWN);
}

bool Input::GetKeyUp(const std::string& key)
{
    SDL_Scancode sc = LookupScancode(key);
    if (sc == SDL_SCANCODE_UNKNOWN) return false;

    auto it = keyboard_states.find(sc);
    if (it == keyboard_states.end()) return false;
    return (it->second == INPUT_STATE_JUST_BECAME_UP);
}

glm::vec2 Input::GetMousePosition()
{
    return mouse_position;
}

bool Input::GetMouseButton(int button)
{
    auto it = mouse_button_states.find(button);
    if (it == mouse_button_states.end()) return false;
    return (it->second == INPUT_STATE_DOWN || it->second == INPUT_STATE_JUST_BECAME_DOWN);
}

bool Input::GetMouseButtonDown(int button)
{
    auto it = mouse_button_states.find(button);
    if (it == mouse_button_states.end()) return false;
    return (it->second == INPUT_STATE_JUST_BECAME_DOWN);
}

bool Input::GetMouseButtonUp(int button)
{
    auto it = mouse_button_states.find(button);
    if (it == mouse_button_states.end()) return false;
    return (it->second == INPUT_STATE_JUST_BECAME_UP);
}

float Input::GetMouseScrollDelta()
{
    return mouse_scroll_this_frame;
}

void Input::HideCursor()
{
    SDL_ShowCursor(SDL_DISABLE);
}

void Input::ShowCursor()
{
    SDL_ShowCursor(SDL_ENABLE);
}
