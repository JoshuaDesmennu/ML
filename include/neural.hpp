#pragma once

#include <vector>
#include <random>
#include <filesystem>
#include "matrix.hpp"

class NN {
    std::vector<Matrix> biases;
    std::vector<Matrix> d_biases;
    std::vector<Matrix> d_weights;
    std::vector<Matrix> weights;
    std::vector<std::vector<double>> activations;
    std::vector<std::vector<double>> d_act_funcs;
    double previousCost;

    public:
    int layerCount;
    double cost;
    std::vector<int> layer_sizes;
    
    NN(std::vector<int> layer_sizes, std::mt19937& rng);
    NN(std::string filename);
    // takes in a 1-column matrix of `layer_sizes[0]` inputs normalized between 0 and 1
    // Returns a 1-column matrix of `layer_sizes[layerCount-1]` values normalized between 0 and 1
    Matrix passthrough(const Matrix& inputs);
    Matrix train(const Matrix& inputs, const Matrix& answer);
    void descend_gradient(int sample_size, double eta);
    void save_to_file(std::string filename);
};

