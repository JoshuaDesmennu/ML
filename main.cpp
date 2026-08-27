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

class Graphix {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event e;
    std::vector<uint8_t> image_data;

    public:
    int selectedImage = 0;
    int window_width;
    int window_height;
    bool should_quit;
    Graphix() {
        window_width = 1280;
        window_height = 720;
        should_quit = false;
        image_data = std::vector<uint8_t>(28 * 28, 255);

        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            throw std::runtime_error("Could not initialize SDL");
        }
        window = SDL_CreateWindow("Number Guesser", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, SDL_WINDOW_SHOWN);
        if (window == NULL) {
            SDL_Quit();
            throw std::runtime_error("Could not create window");
        }
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (renderer == NULL) {
            SDL_DestroyWindow(window);
            SDL_Quit();
            throw std::runtime_error("Could not create window renderer");
        }
    }

    void cycle(LabelledImageData& img_provider, std::function<void(int)> guess) {
        if (should_quit) return;

        // handle events like clicks and that
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                should_quit = true;
            } else if (e.type == SDL_KEYUP) {
                if (e.key.keysym.sym == SDLK_LEFT) {
                    if (selectedImage >= 1) {
                        selectedImage--;
                        updateImage(img_provider.getRawImageBytes(selectedImage));
                        guess(selectedImage);
                    }
                } else if (e.key.keysym.sym == SDLK_RIGHT) {
                    if (selectedImage <= img_provider.entry_count - 2) {
                        selectedImage++;
                        updateImage(img_provider.getRawImageBytes(selectedImage));
                        guess(selectedImage);
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        const int drawing_rect_size = 560;
        const int padding = 20;
        const SDL_Rect border_rectangle = {
            padding-4, padding-4, drawing_rect_size+8, drawing_rect_size+8,
        };
        SDL_RenderDrawRect(renderer, &border_rectangle);

        // draw pixels
        const int pixel_size = 20;
        for (int i = 0; i < (int)image_data.size(); i++) {
            const int x = i % 28;
            const int y = i / 28;
            const SDL_Rect pixel_rectangle = {
                .x = padding + x * pixel_size, 
                .y = padding + y * pixel_size, 
                .w = pixel_size, 
                .h = pixel_size
            };
            SDL_SetRenderDrawColor(renderer, image_data[i], image_data[i], image_data[i], 255);
            SDL_RenderFillRect(renderer, &pixel_rectangle);
        }
        SDL_RenderPresent(renderer);
    }

    void updateImage(const std::vector<uint8_t>& image) {
        if (image.size() != image_data.size()) {
            throw std::runtime_error("Cannot assign image with different dimensions");
        }
        image_data = image;
    }

    ~Graphix() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
};



int main(int argc, char **argv) {
    std::mt19937 rng(std::random_device{}());
    std::vector<std::string> args(argv, argv+argc);
    std::vector<uint8_t> imageFileData = idx::read_file("./training_images/train-images.idx3-ubyte");
    std::vector<uint8_t> labelFileData = idx::read_file("./training_labels/train-labels.idx1-ubyte");
    auto [imgSizes, imgIndex] = idx::decode_idx(imageFileData);
    auto [lblSizes, lblIndex] = idx::decode_idx(labelFileData);

    LabelledImageData training_set(
        std::vector<uint8_t>(
            labelFileData.begin() + lblIndex, labelFileData.end()
        ),
        std::vector<uint8_t>(
            imageFileData.begin() + imgIndex, imageFileData.end()
        ),
        imgSizes[0],
        imgSizes[1],
        imgSizes[2]
    );

    

    NN network("bp_nn.nn");
    auto data = training_set.getRawImageBytes(2);
    auto guess = [&](int index) {
        Matrix result = network.passthrough(training_set.getEntryImage(index).scale(1.0/255.0).flatten());
        result.print_by_column();
    };
    Graphix gfx;
    gfx.updateImage(data);
    guess(gfx.selectedImage);
    while(!gfx.should_quit) {
        gfx.cycle(training_set, guess);
    }



    // NN network(std::vector<int>{28*28, 16, 16, 10}, rng);
    // Matrix result;
    // const int passNumber = 2000;
    // const int sampleSize = 1000;
    // std::vector<std::pair<Matrix, Matrix>> training_data;

    // for (int i = 0; i < passNumber; i++) {
    //     training_data = training_set.selectRandomImageLabelPairs(sampleSize, rng);
    //     for (auto& pair : training_data) {
    //         network.train(pair.first, pair.second);
    //     }
    //     network.descend_gradient(sampleSize, 0.1);
    //     std::cout << "Cost: " << network.cost << "\n";
    // }

    // network.save_to_file("bp_nn.nn");
}