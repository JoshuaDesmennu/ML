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
#include "graphix.hpp"

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