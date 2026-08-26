#pragma once

#include <vector>
#include <random>
#include <filesystem>
#include "matrix.hpp"

class NN {
    public:
    std::vector<Matrix> biases;
    std::vector<Matrix> d_biases;
    int layerCount;
    double cost;
    double previousCost;
    
    std::vector<int> layer_sizes;
    std::vector<Matrix> weights;
    std::vector<Matrix> d_weights;
    std::vector<std::vector<double>> activations;
    std::vector<std::vector<double>> d_act_funcs;

    NN(std::vector<int> layer_sizes, std::mt19937& rng);
    NN(std::string filename);

    // takes in a 1-column matrix of 784 inputs normalized between 0 and 1
    // Returns a 1-column matrix of 10 values corresponding with 0 through to 9
    Matrix passthrough(const Matrix& inputs);
    Matrix train(const Matrix& inputs, const Matrix& answer);
    void descend_gradient(int sample_size, double eta);
    void save_to_file(std::string filename);
};

