#include <iostream>
#include <vector>
#include <iomanip>
#include <span>
#include <cmath>
#include "matrix.hpp"
#include "idx.hpp"
#include "neural.hpp"
#include <SDL2/SDL.h>

class Graphix {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event e;

    public:
    int window_width;
    int window_height;
    bool should_quit;
    Graphix() {
        window_width = 1280;
        window_height = 720;
        should_quit = false;
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

    void cycle() {
        if (should_quit) return;
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                should_quit = true;
            }
        }
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

    NN network(std::vector<int>{28*28, 16, 16, 10}, rng);
    // NN network("bp_nn.nn");
    Matrix result;
    const int passNumber = 2000;
    const int sampleSize = 1000;
    std::vector<std::pair<Matrix, Matrix>> training_data;

    for (int i = 0; i < passNumber; i++) {
        training_data = training_set.selectRandomImageLabelPairs(sampleSize, rng);
        for (auto& pair : training_data) {
            network.train(pair.first, pair.second);
        }
        network.descend_gradient(sampleSize, 0.1);
        std::cout << "Cost: " << network.cost << "\n";
    }

    network.save_to_file("bp_nn.nn");
}