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

    NN network(std::vector<int>{28*28, 16, 16, 10}, rng);
    Matrix result;
    // const int passNumber = 100;
    const int sampleSize = 1000;
    std::vector<std::pair<Matrix, Matrix>> training_data;
    training_data = training_set.selectRandomImageLabelPairs(sampleSize, rng);

    std::cout << "BEFORE\n";
    result = network.passthrough(training_data[0].first);
    result.print_by_column();
    training_data[0].second.print_by_column();

    for (auto& item : training_data) {
        network.train(item.first, item.second);
    }
    network.descend_gradient(sampleSize, 0.1);

    std::cout << "AFTER\n";
    result = network.passthrough(training_data[0].first);
    result.print_by_column();
    training_data[0].second.print_by_column();


    network.save_to_file("bp_nn.nn");
    

}