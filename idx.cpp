#include <fstream>
#include <iostream>
#include "idx.hpp"

#define MAGICNUMBER_TYPE_INDEX 2
#define DIMENSION_NUMBER_INDEX 3

std::vector<uint8_t> idx::read_file(const std::string& filepath) {
    // open the file in binary mode
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for reading");
    }

    // get size of bytes within
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);


    std::vector<uint8_t> bytes(size);
    if (!file.read(reinterpret_cast<char*>(bytes.data()), size)) {
        throw std::runtime_error("Could not read data successfully");
    }

    return bytes;
}

uint32_t int32_from_buffer_msb(std::vector<uint8_t>& buffer, int index) {
    return (buffer.at(index) << 24) + (buffer.at(index+1) << 16) + (buffer.at(index+2) << 8) + buffer.at(index+3);
}

idx::DecodeReturn idx::decode_idx(std::vector<uint8_t>& idxfiledata) {
    uint8_t dimension_count = idxfiledata.at(DIMENSION_NUMBER_INDEX);
    std::vector<uint32_t> dimension_sizes;

    // get the sizes of the various arrays of each dimension
    for (int i = 0; i < dimension_count; i++) {
        int index = DIMENSION_NUMBER_INDEX + 1 + (4 * i);
        uint32_t size = int32_from_buffer_msb(idxfiledata, index);
        dimension_sizes.push_back(size);
    }

    return {
        .sizes = dimension_sizes,
        .imagedata_index = static_cast<uint32_t>(DIMENSION_NUMBER_INDEX + 1 + (4 * dimension_count)),
    };
}

Matrix LabelledImageData::getEntryImage(int index) {
    if (index < 0 || index >= entry_count) {
        throw std::runtime_error("Index out of bounds");
    }

    return Matrix(
        std::vector<double>(
            imagedata.begin() + (width * height) * index,
            imagedata.begin() + (width * height) * (index + 1)
        ),
        width, height
    );
}


Matrix LabelledImageData::getEntryExpectedAnswer(int index) {
    Matrix res(10, 1);
    res.set(getEntryLabel(index), 0, 1);
    return res;
}

int LabelledImageData::getEntryLabel(int index) {
    if (index < 0 || index >= entry_count) {
        throw std::runtime_error("Index out of bounds");
    }

    return labels.at(index);
}

LabelledImageData::LabelledImageData(std::vector<uint8_t> labels, std::vector<uint8_t> imagedata, int entry_count, int width, int height) {
    this->labels = labels;
    this->imagedata = imagedata;
    this->width = width;
    this->height = height;
    this->entry_count = entry_count;
}

std::vector<std::pair<Matrix, Matrix>> LabelledImageData::selectRandomImageLabelPairs(int count, std::mt19937& rng) {
    std::uniform_int_distribution dist(0, entry_count-1);
    std::vector<std::pair<Matrix, Matrix>> pairs(count);
    
    for (int i = 0; i < count; i++) {
        int random_index = dist(rng);
        pairs[i].first = getEntryImage(random_index).scale(1.0 / 255.0).flatten();
        pairs[i].second = getEntryExpectedAnswer(random_index);
    }

    return pairs;
}

void LabelledImageData::drawEntryImage(int index) {
    static std::string ascii = " .'`^\",:;Il!i><~+_-?][}{1)(|\\/*tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";
    Matrix entry = getEntryImage(index);
    for (int j = 0; j < entry.columns; j++) {
        for (int i = 0; i < entry.rows; i++) {
            std::cout << ascii[(int)(entry.at(j, i) * 72 / 255)];
        }
        std::cout << "\n";
    }
}


std::pair<Matrix, Matrix> LabelledImageData::getEntryPair(int index) {
    return std::pair<Matrix, Matrix>{
        getEntryImage(index).flatten(), getEntryExpectedAnswer(index)
    };
}