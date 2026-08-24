#include <iostream>
#include <vector>
#include <iomanip>
#include <span>
#include <cmath>
#include "matrix.hpp"
#include "idx.hpp"
#include "neural.hpp"

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

    // NN network(std::vector<int>{28*28, 16, 16, 10}, rng);
    NN network("best_nn_yet.nn");
    auto training_data = training_set.getEntryPair(2);
    Matrix result = network.passthrough_store(training_data.first, training_data.second);
    std::cout << "The cost is " << network.cost  << "\n";
    for (auto item : result.values) {
        std::cout << item << ", ";
    }
    std::cout << "\nThe image is a " << training_set.getEntryLabel(2);

}