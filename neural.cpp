#include "neural.hpp"
#include <fstream>
#include <cstring>
#include <bit>

double sigmoid(double x) {
    return 1.0 / (1.0 + std::exp(-x));
}

void softmax(Matrix& input) {
    double denominator = 0;
    for (int i = 0; i < (int)input.values.size(); i++) {
        denominator += expf64(input.values[i]);
    }
    for (auto& value : input.values) {
        value = expf64(value) / denominator;
    }
}

double square (double x) {
    return powf64(x, 2);
}


NN::NN(std::vector<int> layer_sizes, std::mt19937& rng) : layer_sizes(layer_sizes) {
    layerCount = layer_sizes.size();
    weights = d_weights = std::vector<Matrix>{};
    weights.reserve(layerCount-1);
    d_weights.reserve(layerCount-1);
    biases = d_biases = std::vector<Matrix>{};
    biases.reserve(layerCount-1);
    d_biases.reserve(layerCount-1);
    activations = std::vector<std::vector<double>>(layerCount, std::vector<double>{});
    d_act_funcs = std::vector<std::vector<double>>(layerCount, std::vector<double>{});

    for (int i = 0; i < layerCount-1; i++) {
        weights.emplace_back(layer_sizes.at(i+1), layer_sizes.at(i), rng, sqrtf64(6.0 / (double)(layer_sizes[i]+layer_sizes[i+1])));
        d_weights.emplace_back(layer_sizes.at(i+1), layer_sizes.at(i));
        biases.emplace_back(layer_sizes.at(i+1), 1);
        d_biases.emplace_back(layer_sizes.at(i+1), 1);
        activations[i].reserve(layer_sizes[i+1]);
        d_act_funcs[i].reserve(layer_sizes[i+1]);
    }
}

Matrix NN::passthrough(const Matrix& inputs) {
    Matrix result = inputs;
    for (int i = 0; i < layerCount - 1; i++) {
        result = Matrix::multiply(weights[i], result);
        result.add(biases[i]);
        result.perform(sigmoid);
    }
    return result;
}

Matrix NN::train(const Matrix& inputs, const Matrix& answer) {
    Matrix result = inputs;
    for (int i = 0; i < layerCount - 1; i++) {
        // typical feedforward step
        result = Matrix::multiply(weights[i], result);
        result.add(biases[i]);

        if (i == layerCount - 2) {
            softmax(result);
        } else {
            result.perform(sigmoid);
        }

        // store activations
        activations[i] = result.values;
        // store derivatives of sigmoid
        if (i < layerCount - 2) {
            d_act_funcs[i] = result.values;
            for (auto &value : d_act_funcs[i]) {
                value = value * (1 - value);
            }
        }
    }

    previousCost = cost;
    cost = 0.0;

    // cross-entropy
    for (int i = 0; i < result.rows; i++) {
        cost -= answer.at(i, 0) * std::log(result.at(i, 0));
    }

    Matrix subtracted = result;
    subtracted.subtract(answer);

    Matrix act_func_matrix;
    Matrix layer_l_error;
    Matrix prev_activation_matrix;

    // backpropagate
    for (int propagateLayer = layerCount - 2; propagateLayer >= 0; propagateLayer--) {
        act_func_matrix.values = d_act_funcs[propagateLayer];
        act_func_matrix.rows = act_func_matrix.values.size();
        act_func_matrix.columns = 1;
        // if just started, use subtracted or nabla_aL_C
        if (propagateLayer == layerCount - 2) {
            layer_l_error = subtracted;
        } else {
            layer_l_error = Matrix::hadamard(act_func_matrix, Matrix::multiply(Matrix::transpose(weights[propagateLayer+1]), layer_l_error));
        }

        // if at first non input layer, use input as the previous activation
        if (propagateLayer == 0) {
            prev_activation_matrix = inputs;
        } else {
            prev_activation_matrix.values = activations[propagateLayer-1];
            prev_activation_matrix.rows = prev_activation_matrix.values.size();
            prev_activation_matrix.columns = 1;
        }

        d_weights[propagateLayer].add(Matrix::multiply(layer_l_error, Matrix::transpose(prev_activation_matrix)));
        d_biases[propagateLayer].add(layer_l_error);
    }

    // mean square error
    // subtracted.perform(square);
    // for (int i = 0; i < subtracted.rows; i++) {
    //     cost += subtracted.at(i, 0);
    // }
    // cost *= 0.5;

    return result;
}

void NN::descend_gradient(int sample_size, double eta) {
    for (unsigned int i = 0; i < weights.size(); i++) {
        weights[i].subtract(Matrix::scale(d_weights[i], eta / sample_size));
        biases[i].subtract(Matrix::scale(d_biases[i], eta / sample_size));
        d_weights[i].values = std::vector<double>(d_weights[i].rows * d_weights[i].columns, 0);
        d_biases[i].values = std::vector<double>(d_biases[i].rows * d_biases[i].columns, 0);
    }
}

uint8_t get_byte(uint64_t number, int byte_position) {
    return (number >> (8 * byte_position)) & 0xff;
}

void NN::save_to_file(std::string filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Could not open file");
    }

    int total_buffer_size = 0;
    for (int i = 0; i < layerCount-1; i++) {
        total_buffer_size += weights[i].values.size() + biases[i].values.size();
    }
    std::vector<uint8_t> header_buffer = {
        0xde, 0xad, 0xbe, 0xef, get_byte(sizeof(double), 0), get_byte(sizeof(double), 1),
        get_byte(layerCount, 0), get_byte(layerCount, 1), get_byte(layerCount, 2), get_byte(layerCount, 3),
    };
    header_buffer.reserve(10 + 4 * layerCount);
    for (int i = 0; i < layerCount; i++) {
        header_buffer.push_back(get_byte(layer_sizes[i], 0));
        header_buffer.push_back(get_byte(layer_sizes[i], 1));
        header_buffer.push_back(get_byte(layer_sizes[i], 2));
        header_buffer.push_back(get_byte(layer_sizes[i], 3));
    }

    std::vector<double> file_buffer;
    file_buffer.reserve(total_buffer_size);

    for (int i = 0; i < layerCount-1; i++) {
        file_buffer.insert(file_buffer.end(), weights[i].values.begin(), weights[i].values.end());
        file_buffer.insert(file_buffer.end(), biases[i].values.begin(), biases[i].values.end());
    }

    file.write(
        reinterpret_cast<const char *>(header_buffer.data()),
        header_buffer.size() * sizeof(uint8_t)
    );

    file.write(
        reinterpret_cast<const char *>(file_buffer.data()),
        file_buffer.size() * sizeof(double)
    );
}

int64_t load_number_from_bytes_lsb(const std::vector<uint8_t>& bytes, int position, int byte_count) {
    int64_t result = 0;
    for (int i = 0; i < byte_count; i++) {
        result += bytes[i + position] << (8 * i);
    }
    return result;
}

NN::NN(std::string filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) {
        throw std::runtime_error("Could not open file for reading");
    }

    // get file size;
    std::streamsize file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> file_buffer(file_size);

    // read data into buffer
    if (!file.read(reinterpret_cast<char*>(file_buffer.data()), file_size)) {
        throw std::runtime_error("Could not read contents of file");
    }

    file.close();

    int unit_size = load_number_from_bytes_lsb(file_buffer, 4, 2);
    layerCount = load_number_from_bytes_lsb(file_buffer, 6, 4);
    layer_sizes = std::vector<int>(layerCount);
    for (int i = 0; i < layerCount; i++) {
        layer_sizes[i] = load_number_from_bytes_lsb(file_buffer, 10 + 4 * i, 4);
    }

    weights = d_weights = std::vector<Matrix>{};
    weights.reserve(layerCount-1);
    d_weights.reserve(layerCount-1);
    d_biases = biases = std::vector<Matrix>{};
    biases.reserve(layerCount-1);
    d_biases.reserve(layerCount-1);
    activations = std::vector<std::vector<double>>(layerCount, std::vector<double>{});
    d_act_funcs = std::vector<std::vector<double>>(layerCount, std::vector<double>{});

    for (int i = 0; i < layerCount-1; i++) {
        weights.emplace_back(layer_sizes.at(i+1), layer_sizes.at(i));
        d_weights.emplace_back(layer_sizes.at(i+1), layer_sizes.at(i));
        biases.emplace_back(layer_sizes.at(i+1), 1);
        d_biases.emplace_back(layer_sizes.at(i+1), 1);
        activations[i].reserve(layer_sizes[i+1]);
        d_act_funcs[i].reserve(layer_sizes[i+1]);
    }

    int weight_data_position = 10 + 4 * layerCount;
    int cursor = 0;

    for (int i = 0; i < layerCount-1; i++) {
        if (unit_size == 8) {
            for (auto& number : weights[i].values) {
                std::memcpy(
                    &number,
                    file_buffer.data() + weight_data_position + unit_size * cursor++,
                    sizeof(double)
                );
            }
            for (auto& number : biases[i].values) {
                std::memcpy(
                    &number,
                    file_buffer.data() + weight_data_position + unit_size * cursor++,
                    sizeof(double)
                );
            }
        } else if (unit_size == 4) {
            for (auto& number : weights[i].values) {
                std::memcpy(
                    &number,
                    file_buffer.data() + weight_data_position + unit_size * cursor++,
                    sizeof(double)
                );
            }
            for (auto& number : biases[i].values) {
                std::memcpy(
                    &number,
                    file_buffer.data() + weight_data_position + unit_size * cursor++,
                    sizeof(double)
                );
            }

        }
    }

}