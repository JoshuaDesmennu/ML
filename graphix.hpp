#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <unordered_map>
#include "matrix.hpp"
#include "idx.hpp"
#include <SDL2/SDL_ttf.h>
#include "input_info.hpp"

enum AppMode
{
    START,
    USER_DRAW,
    SCROLL_THROUGH
};

class Graphix
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event e;
    std::vector<uint8_t> image_data;
    Matrix prediction;
    std::unordered_map<std::string, SDL_Texture *> textures;
    TTF_Font *font;
    bool is_mouse_down = false;
    std::pair<int, int> mouse_coords;
    const SDL_Color white = {.r = 255, .g = 255, .b = 255, .a = 255};
    const int padding = 20;
    const int drawing_rect_size = 560;
    const int button_padding = 10;
    LabelledImageData& img_provider;
    bool input_cursor_on = true;
    int cursor_blink_timer = 0;
    const int cursor_blink_period = 600;
    std::unordered_map<std::string, InputInfo> input_data;
    std::unordered_map<std::string, bool> is_input_hovered;
    std::unordered_map<std::string, bool> is_button_hovered;
    uint64_t ticks_ms = 0;
    const int ui_height = 100;
    int brush_radius = 0;
    void putMousePathToImage();
    void drawAndManageButton(std::string text, int x, int y, int font_size);
    void reset(bool full);
    void submitNumber(LabelledImageData& img_provider, std::function<Matrix(int)>);
    
    public:
    AppMode mode;
    int selectedImage = 0;
    int window_width;
    int window_height;
    bool should_quit;
    Graphix(LabelledImageData& img_provider);
    void cycle(LabelledImageData &img_provider, std::function<Matrix(int)> guess, std::function<Matrix(const std::vector<uint8_t> &)> guess_user_drawn);
    void free_textures();
    void updateImage(const std::vector<uint8_t> &image);
    void changeImage(int selection, LabelledImageData& img_provider, std::function<Matrix(int)> guess);
    ~Graphix();
};