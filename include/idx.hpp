#pragma once
#include <vector>
#include <filesystem>
#include "matrix.hpp"

namespace idx {
    struct DecodeReturn {
        std::vector<uint32_t> sizes;
        uint32_t imagedata_index;
    };
    std::vector<uint8_t> read_file(const std::string& filepath);
    DecodeReturn decode_idx(std::vector<uint8_t>& idxfiledata);

}

class LabelledImageData {
    std::vector<uint8_t> labels;
    std::vector<uint8_t> imagedata;
    std::vector<std::vector<Matrix>> imagesByLabel;

    public:
    int width;
    int height;
    int entry_count;

    LabelledImageData(std::vector<uint8_t> labels, std::vector<uint8_t> imagedata, int width, int height, int entry_count);
    Matrix getEntryImage(int index);
    int getEntryLabel(int index);
    Matrix getEntryExpectedAnswer(int index);
    std::pair<Matrix, Matrix> getEntryPair(int index);
    std::vector<std::pair<Matrix, Matrix>> selectRandomImageLabelPairs(int count, std::mt19937& rng);
    std::vector<std::pair<Matrix, Matrix>> selectImageLabelPairsOfLabel(int count, std::vector<int> labels, std::mt19937& rng);
    Matrix getExpectedAnswerFromLabelNumber(int label);
    std::vector<uint8_t> getRawImageBytes(int index);
    void drawEntryImage(int index);
};