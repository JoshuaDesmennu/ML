#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <unordered_map>
#include "matrix.hpp"
#include "idx.hpp"
#include <SDL2/SDL_ttf.h>


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
    const int padding = 20;
    const int drawing_rect_size = 560;

public:
    AppMode mode;
    int selectedImage = 0;
    int window_width;
    int window_height;
    bool should_quit;
    std::unordered_map<std::string, bool> is_button_hovered;
    int brush_radius = 0;
    const int ui_height = 100;
    Graphix();
    void putMousePathToImage();
    void reset(bool full);
    void drawAndManageButton(std::string text, int x, int y, int font_size);
    void changeImage(int selection, LabelledImageData& img_provider, std::function<Matrix(int)> guess);
    void cycle(LabelledImageData &img_provider, std::function<Matrix(int)> guess, std::function<Matrix(const std::vector<uint8_t> &)> guess_user_drawn);
    void updateImage(const std::vector<uint8_t> &image);
    void free_textures();
    ~Graphix();
};