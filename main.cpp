#include <iostream>
#include <vector>
#include <iomanip>
#include <span>
#include <cmath>
#include <functional>
#include "matrix.hpp"
#include "idx.hpp"
#include "neural.hpp"
#include <SDL2/SDL.h>
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
    Graphix()
    {
        mode = START;
        window_width = 1280;
        window_height = 720;
        should_quit = false;
        image_data = std::vector<uint8_t>(28 * 28, 0);
        prediction = Matrix(10, 1);

        if (SDL_Init(SDL_INIT_VIDEO) != 0)
        {
            throw std::runtime_error("Could not initialize SDL");
        }

        if (TTF_Init() != 0)
        {
            SDL_Quit();
            throw std::runtime_error("Could not initialize TTF");
        }

        font = TTF_OpenFont("./intel_variable.ttf", 25);
        if (font == nullptr)
        {
            TTF_Quit();
            SDL_Quit();
            throw std::runtime_error("Could not open font for use");
        }

        window = SDL_CreateWindow("MIA", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, SDL_WINDOW_SHOWN);
        if (window == NULL)
        {
            SDL_Quit();
            throw std::runtime_error("Could not create window");
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (renderer == NULL)
        {
            SDL_DestroyWindow(window);
            SDL_Quit();
            throw std::runtime_error("Could not create window renderer");
        }

        SDL_Surface *surface = NULL;
        const SDL_Color white = {.r = 255, .g = 255, .b = 255, .a = 255};
        for (int i = 0; i < 10; i++)
        {
            const std::string number_str = std::to_string(i);
            surface = TTF_RenderUTF8_Blended(font, number_str.c_str(), white);
            textures[number_str] = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);
        }

        TTF_SetFontSize(font, 70);
        surface = TTF_RenderUTF8_Blended(font, "Bienvenue au MIA", white);
        textures["Bienvenue au MIA"] = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        TTF_SetFontSize(font, 40);
        surface = TTF_RenderUTF8_Blended(font, "MIA's Accurate* Guess", white);
        textures["MIA's Accurate* Guess"] = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        TTF_SetFontSize(font, 30);
        surface = TTF_RenderUTF8_Blended(font, "Commence", white);
        textures["Commence"] = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        surface = TTF_RenderUTF8_Blended(font, "Quitte", white);
        textures["Quitte"] = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        surface = TTF_RenderUTF8_Blended(font, "Switch Mode", white);
        textures["Switch Mode"] = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        surface = TTF_RenderUTF8_Blended(font, "Prev Image", white);
        textures["Prev Image"] = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        surface = TTF_RenderUTF8_Blended(font, "Next Image", white);
        textures["Next Image"] = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        surface = TTF_RenderUTF8_Blended(font, "Clear", white);
        textures["Clear"] = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        is_button_hovered["Commence"] = false;
        is_button_hovered["Quitte"] = false;
        is_button_hovered["Switch Mode"] = false;
        is_button_hovered["Prev Image"] = false;
        is_button_hovered["Next Image"] = false;
        is_button_hovered["Clear"] = false;
    }

    void putMousePathToImage()
    {

        if (mouse_coords.first < padding || mouse_coords.first > padding + drawing_rect_size || mouse_coords.second < ui_height + padding || mouse_coords.second > padding + ui_height + drawing_rect_size)
            return;
        const int image_width = 28;
        const int effectiveX = (mouse_coords.first - padding) / (drawing_rect_size / image_width);
        const int effectiveY = (mouse_coords.second - padding - ui_height) / (drawing_rect_size / image_width);

        for (int x = effectiveX - brush_radius; x <= effectiveX + brush_radius; x++)
        {
            if (x < 0 || x > 27)
                continue;
            for (int y = effectiveY - brush_radius; y <= effectiveY + brush_radius; y++)
            {
                if (y < 0 || y > 27)
                    continue;
                int pixel_index = x + image_width * y;
                int additive;
                if (brush_radius == 0)
                {
                    additive = 255;
                }
                else
                {
                    additive = 255.0 * (1.0 - (std::hypot(x - effectiveX, y - effectiveY) / std::hypot(brush_radius, brush_radius)));
                }
                if (additive + image_data[pixel_index] > 255)
                {
                    image_data[pixel_index] = 255;
                }
                else
                {
                    image_data[pixel_index] += additive;
                }
            }
        }
    }

    void reset(bool full) {
        prediction = Matrix(prediction.rows, prediction.columns);
        image_data = std::vector<uint8_t>(image_data.size(), 0);
        if (full) {
            is_mouse_down = false;
            for (auto item : is_button_hovered) {
                is_button_hovered[item.first] = false;
            }
            selectedImage = 0;
        }
    }

    /*
     i already had this coded up and i'm feeling soo lazy to change it.
     Instead of rendering directly based on the x and y (which are relative to the box border)
     i'm converting them to be relative to the text initially (adding back paddings)
     So i don't have to refactor this working code :)
     */
    void drawAndManageButton(std::string text, int x, int y, int font_size) {
        TTF_SetFontSize(font, font_size);
        SDL_Rect rect;
        TTF_SizeText(font, text.c_str(), &rect.w, &rect.h);
        rect.x = x + 10;
        rect.y = y + 10;

        SDL_Rect border = rect;
        border.x -= 10;
        border.y -= 10;
        border.w += 20;
        border.h += 20;

        // set hovering flag if within bounding box
        is_button_hovered[text] = (mouse_coords.first > border.x && mouse_coords.first < border.x + border.w && mouse_coords.second > border.y && mouse_coords.second < border.y + border.h);

        if (is_button_hovered[text])
        {
            SDL_SetTextureColorMod(textures[text], 0, 0, 0);
        }
        else
        {
            SDL_SetTextureColorMod(textures[text], 255, 255, 255);
        }
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        if (is_button_hovered[text] == true)
        {
            SDL_RenderFillRect(renderer, &border);
        }
        else
        {
            SDL_RenderDrawRect(renderer, &border);
        }
        SDL_RenderCopy(renderer, textures[text], nullptr, &rect);

    }

    // can only work in `SCROLL_THROUGH` mode
    void changeImage(int selection, LabelledImageData& img_provider, std::function<Matrix(int)> guess) {
        if (selection < 0 || selection >= img_provider.entry_count || mode != SCROLL_THROUGH) return;
        selectedImage = selection;
        updateImage(img_provider.getRawImageBytes(selectedImage));
        prediction = guess(selectedImage);
    }

    void cycle(LabelledImageData &img_provider, std::function<Matrix(int)> guess, std::function<Matrix(const std::vector<uint8_t> &)> guess_user_drawn)
    {
        if (should_quit)
            return;

        // handle events like clicks and that
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                should_quit = true;
            }
            else if (e.type == SDL_KEYUP)
            {
                if (mode == SCROLL_THROUGH)
                {
                    if (e.key.keysym.sym == SDLK_LEFT)
                    {
                        changeImage(--selectedImage, img_provider, guess);
                    }
                    else if (e.key.keysym.sym == SDLK_RIGHT)
                    {
                        changeImage(++selectedImage, img_provider, guess);
                    }
                }
                if (e.key.keysym.sym == SDLK_c)
                {
                    reset(false);
                }
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                is_mouse_down = true;
                mouse_coords.first = e.button.x;
                mouse_coords.second = e.button.y;
                if (mode == START)
                {
                    if (is_button_hovered["Commence"])
                    {
                        mode = SCROLL_THROUGH;
                    }
                    if (is_button_hovered["Quitte"])
                    {
                        should_quit = true;
                    }
                    
                } else if (mode == USER_DRAW || mode == SCROLL_THROUGH) {
                    if (mode == USER_DRAW) {
                        if (is_button_hovered["Clear"]) {
                            reset(false);
                        }
                    }
                    if (mode == SCROLL_THROUGH) {
                        if (is_button_hovered["Next Image"]) {
                            changeImage(++selectedImage, img_provider, guess);
                        }
                        if (is_button_hovered["Prev Image"]) {
                            changeImage(--selectedImage, img_provider, guess);
                        }
                    }
                    if (is_button_hovered["Switch Mode"]) {
                        mode = mode == SCROLL_THROUGH ? USER_DRAW : SCROLL_THROUGH;
                        reset(true);
                    }
                }
            }
            else if (e.type == SDL_MOUSEBUTTONUP)
            {
                if (mode == USER_DRAW)
                    prediction = guess_user_drawn(image_data);

                is_mouse_down = false;
                mouse_coords.first = e.button.x;
                mouse_coords.second = e.button.y;
            }
            else if (e.type == SDL_MOUSEMOTION)
            {
                mouse_coords.first = e.motion.x;
                mouse_coords.second = e.motion.y;
            }
        }

        // clear the screen for drawing
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        

        if (mode == SCROLL_THROUGH || mode == USER_DRAW)
        {
            int next_w, next_h;
            std::string text = "Switch Mode";
            TTF_SetFontSize(font, 30);
            TTF_SizeText(font, text.c_str(), &next_w, &next_h);
            int next_x = padding;
            drawAndManageButton(text, next_x, padding, 30);

            TTF_SetFontSize(font, 25);
            const SDL_Rect border_rectangle = {
                padding,
                ui_height + padding,
                drawing_rect_size,
                drawing_rect_size,
            };

            // draw image pixels
            const int pixel_size = 20;
            for (int i = 0; i < (int)image_data.size(); i++)
            {
                const int x = i % 28;
                const int y = i / 28;
                const SDL_Rect pixel_rectangle = {
                    .x = padding + x * pixel_size,
                    .y = ui_height + padding + y * pixel_size,
                    .w = pixel_size,
                    .h = pixel_size};
                SDL_SetRenderDrawColor(renderer, image_data[i], image_data[i], image_data[i], 255);
                SDL_RenderFillRect(renderer, &pixel_rectangle);
            }

            // draw the image border on top of the image
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &border_rectangle);

            // draw render bar graph
            const int bar_height = 25;
            const int full_bar_length = (window_width - drawing_rect_size) * 2 / 3;
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

            const int spacing = 10;
            int bar_x = 0;

            // Calculating the mia_text dimensions early is done so i can push everything to the other side using these values 
            TTF_SetFontSize(font, bar_height);

            for (int i = 0; i < prediction.rows; i++)
            {
                const std::string number_text = std::to_string(i);
                SDL_Rect text_rect = {
                    .x = padding + drawing_rect_size + padding,
                    .y = ui_height + padding + (bar_height + padding) * i,
                };
                TTF_SizeUTF8(font, number_text.c_str(), &(text_rect.w), &(text_rect.h));
                text_rect.y -= (text_rect.h - 25) / 2;
                SDL_RenderCopy(renderer, textures[number_text], NULL, &text_rect);

                const SDL_Rect bar_rect = {
                    .x = text_rect.x + text_rect.w + padding,
                    .y = ui_height + padding + (bar_height + padding) * i,
                    .w = (int)((double)full_bar_length * prediction.at(i, 0)),
                    .h = bar_height,
                };
                SDL_RenderFillRect(renderer, &bar_rect);

                const SDL_Rect full_rect = {
                    .x = bar_rect.x,
                    .y = ui_height + padding + (bar_height + padding) * i,
                    .w = full_bar_length,
                    .h = bar_height,
                };
                SDL_RenderDrawRect(renderer, &full_rect);
                if (i + 1 >= prediction.rows) {
                    bar_x = full_rect.x + full_rect.w;
                }
            }

            TTF_SetFontSize(font, 40);
            SDL_Rect mia_guess_rect;
            text = "MIA's Accurate* Guess";
            TTF_SizeText(font, text.c_str(), &mia_guess_rect.w, &mia_guess_rect.h);
            mia_guess_rect.x = padding + drawing_rect_size + padding;
            mia_guess_rect.y = ui_height + padding + drawing_rect_size - mia_guess_rect.h;

            SDL_RenderCopy(renderer, textures[text], nullptr, &mia_guess_rect);

            // draw arrow to highest guess
            int max_index = 0;
            for (int i = 1; i < prediction.rows; i++)
            {
                if (prediction.at(max_index, 0) < prediction.at(i, 0))
                    max_index = i;
            }

            const int mia_font_size = 40;
            const int long_line_x = window_width - padding;
            const int long_line_base_y = ui_height + padding + drawing_rect_size - mia_font_size / 2;
            const int long_line_top_y = ui_height + padding + (bar_height + padding) * max_index + bar_height / 2;
            SDL_RenderDrawLine(renderer, mia_guess_rect.x + mia_guess_rect.w + spacing, long_line_base_y, long_line_x, long_line_base_y); // short lower
            SDL_RenderDrawLine(renderer, long_line_x, long_line_base_y, long_line_x, long_line_top_y); // long
            SDL_RenderDrawLine(renderer, long_line_x, long_line_top_y, bar_x + padding, long_line_top_y); // short upper
            SDL_RenderDrawLine(renderer, bar_x + padding, long_line_top_y, bar_x + 3*padding, long_line_top_y+1*padding); // slanted lower
            SDL_RenderDrawLine(renderer, bar_x + padding, long_line_top_y, bar_x + 3*padding, long_line_top_y-1*padding); // slanted upper
        }

        if (mode == SCROLL_THROUGH) {
            int next_w, next_h;
            std::string text = "Next Image";
            TTF_SetFontSize(font, 30);
            TTF_SizeText(font, text.c_str(), &next_w, &next_h);
            int next_x = window_width - padding - next_w - 20;
            drawAndManageButton(text, next_x, padding, 30);

            text = "Prev Image";
            TTF_SizeText(font, text.c_str(), &next_w, &next_h);
            drawAndManageButton(text, next_x - padding - next_w - 20, padding, 30);

        }
        if (mode == USER_DRAW)
        {
            if (is_mouse_down)
            {
                putMousePathToImage();
            }

            int next_w, next_h;
            std::string text = "Clear";
            TTF_SetFontSize(font, 30);
            TTF_SizeText(font, text.c_str(), &next_w, &next_h);
            int next_x = window_width - padding - next_w - 20;
            drawAndManageButton(text, next_x, padding, 30);

        }

        if (mode == START)
        {
            SDL_GetMouseState(&mouse_coords.first, &mouse_coords.second);
            SDL_Rect welcome_rect;
            // title
            TTF_SetFontSize(font, 70);
            std::string text = "Bienvenue au MIA";
            TTF_SizeText(font, text.c_str(), &welcome_rect.w, &welcome_rect.h);
            welcome_rect.x = (window_width / 2) - (welcome_rect.w / 2);
            welcome_rect.y = (window_height / 2) - (welcome_rect.h) - padding;
            SDL_RenderCopy(renderer, textures[text], nullptr, &welcome_rect);

            int commence_w, commence_h;
            
            TTF_SetFontSize(font, 30);
            text = "Commence";
            TTF_SizeText(font, text.c_str(), &commence_w, &commence_h);
            drawAndManageButton(text, (window_width / 2) - (commence_w + 20) - padding, (window_height / 2) + padding + 10, 30);
           
            text = "Quitte";
            TTF_SizeText(font, text.c_str(), &commence_w, &commence_h);
            drawAndManageButton(text, (window_width / 2) + padding + 10, (window_height / 2) + padding + 10, 30);
        }

        SDL_RenderPresent(renderer);
    }

    void updateImage(const std::vector<uint8_t> &image)
    {
        if (image.size() != image_data.size())
        {
            throw std::runtime_error("Cannot assign image with different dimensions");
        }
        image_data = image;
    }

    void free_textures()
    {
        for (auto &pair : textures)
        {
            SDL_DestroyTexture(pair.second);
        }
    }

    ~Graphix()
    {
        free_textures();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
};

int main(int argc, char **argv)
{
    std::mt19937 rng(std::random_device{}());
    std::vector<std::string> args(argv, argv + argc);
    std::vector<uint8_t> imageFileData = idx::read_file("./training_images/train-images.idx3-ubyte");
    std::vector<uint8_t> labelFileData = idx::read_file("./training_labels/train-labels.idx1-ubyte");
    auto [imgSizes, imgIndex] = idx::decode_idx(imageFileData);
    auto [lblSizes, lblIndex] = idx::decode_idx(labelFileData);

    LabelledImageData training_set(
        std::vector<uint8_t>(
            labelFileData.begin() + lblIndex, labelFileData.end()),
        std::vector<uint8_t>(
            imageFileData.begin() + imgIndex, imageFileData.end()),
        imgSizes[0],
        imgSizes[1],
        imgSizes[2]);

    NN network("best_nn.nn");
    auto data = training_set.getRawImageBytes(2);
    auto guess = [&](int index)
    {
        return network.passthrough(training_set.getEntryImage(index).scale(1.0 / 255.0).flatten());
    };
    auto guess_user_drawn = [&](const std::vector<uint8_t> &image_data)
    {
        Matrix input = Matrix();
        input.rows = training_set.width * training_set.height;
        input.columns = 1;
        input.values = std::vector<double>(image_data.begin(), image_data.end());
        return network.passthrough(input.scale(1.0 / 255.0).flatten());
    };
    Graphix gfx;
    gfx.changeImage(gfx.selectedImage, training_set, guess);
    while (!gfx.should_quit)
    {
        gfx.cycle(training_set, guess, guess_user_drawn);
    }

    // NN network(std::vector<int>{28*28, 16, 16, 10}, rng);
    // Matrix result;
    // const int passNumber = 10000;
    // const int sampleSize = 10000;

    // std::vector<std::pair<Matrix, Matrix>> training_data;
    // training_data.reserve(training_set.entry_count);
    // for (int label = 0; label < 10; label++) {
    //     for (auto& m : training_set.imagesByLabel[label]) {
    //         training_data.emplace_back(m.scale(1.0/255.0).flatten(), training_set.getExpectedAnswerFromLabelNumber(label));
    //     }
    // }

    // for (int i = 0; i < passNumber; i++) {
    //     for (auto& pair : training_data) {
    //         network.train(pair.first, pair.second);
    //     }
    //     network.descend_gradient(sampleSize, 0.01);
    //     std::cout << "Cost: " << network.cost << "\n";
    // }

    // network.save_to_file("bp_nn.nn");
}