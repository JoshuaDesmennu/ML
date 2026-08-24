#include "neural.hpp"
#include "idx.hpp"
#include <iostream>

int main(int argc, char** argv) {
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

    NN model("best_nn_yet.nn");
    training_set.drawEntryImage(std::stoi(argv[1]));
    Matrix result = model.passthrough(training_set.getEntryImage(0).flatten());
    for (int i = 0; i < result.rows; i++) {
        std::cout << i << ": " <<result.at(i, 0) << "\n";
    }

}