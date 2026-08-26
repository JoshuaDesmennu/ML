#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <functional>
#include "matrix.hpp"

Matrix::Matrix(int rows, int columns) : values(rows*columns, 0), rows(rows), columns(columns) {}

Matrix::Matrix() : rows(0), columns(0) {}

Matrix::Matrix(int rows, int columns, std::mt19937& rng, double scale) : values(rows * columns, 0.0), rows(rows), columns(columns) {
    static std::uniform_real_distribution<double> dist(-1.0, 1.0);
    std::generate(values.begin(), values.end(), [&]() {
        return scale * (dist(rng));
    });
}

Matrix& Matrix::flatten() {
    rows = rows * columns;
    columns = 1;
    return *this;
}

Matrix::Matrix(std::vector<double> values, int rows, int columns) : values(values), rows(rows), columns(columns) {}

void Matrix::set(int i, int j, double value) {
    values[i * columns + j] = value;
}

double Matrix::at(int i, int j) const {
    return values[i * columns + j];
}

Matrix& Matrix::hadamard(const Matrix& a) {
    if (a.rows != rows || a.columns != columns) {
        throw std::runtime_error("Cannot multiply different sized matrices");
    }
    for (int i = 0; i < rows * columns; i++) {
        values[i] *= a.values[i];
    }
    return *this;
}

Matrix& Matrix::add(const Matrix& a) {
    if (a.rows != rows || a.columns != columns) {
        throw std::runtime_error("Impossible addition");
    }
    for (int i = 0; i < a.rows; i++) {
        for (int j = 0; j < a.columns; j++) {
            this->set(i, j, this->at(i, j) + a.at(i, j));
        }
    }
    return *this;
}

Matrix& Matrix::subtract(const Matrix& a) {
    if (a.rows != rows || a.columns != columns) {
        throw std::runtime_error("Impossible addition");
    }
    for (int i = 0; i < a.rows; i++) {
        for (int j = 0; j < a.columns; j++) {
            this->set(i, j, this->at(i, j) - a.at(i, j));
        }
    }
    return *this;
}

Matrix Matrix::add(const Matrix& a, const Matrix& b) {
    if (a.rows != b.rows || a.columns != b.columns) {
        return Matrix(1, 1);
    }
    Matrix res(a.rows, a.columns);
    for (int i = 0; i < a.rows; i++) {
        for (int j = 0; j < a.columns; j++) {
            res.set(i, j, a.at(i, j) + b.at(i, j));
        }
    }
    return res;
}

Matrix& Matrix::transpose() {
    Matrix transposed = Matrix(columns, rows);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            transposed.set(j, i, at(i, j));
        }
    }
    rows = transposed.rows;
    columns = transposed.columns;
    values = transposed.values;
    return *this;
}

Matrix Matrix::transpose(const Matrix &a) {
    Matrix transposed = Matrix(a.columns, a.rows);
    for (int i = 0; i < a.rows; i++) {
        for (int j = 0; j < a.columns; j++) {
            transposed.set(j, i, a.at(i, j));
        }
    }
    return transposed;
}

Matrix Matrix::subtract(const Matrix& a, const Matrix& b) {
    if (a.rows != b.rows || a.columns != b.columns) {
        return Matrix(1, 1);
    }
    Matrix res(a.rows, a.columns);
    for (int i = 0; i < a.rows; i++) {
        for (int j = 0; j < a.columns; j++) {
            res.set(i, j, a.at(i, j) - b.at(i, j));
        }
    }
    return res;
}

Matrix Matrix::hadamard(const Matrix& a, const Matrix& b) {
    if (a.rows != b.rows || a.columns != b.columns) {
        throw std::runtime_error("Cannot multiply incompatible matrices");
    }

    Matrix result(a.rows, a.columns);
    for (int i = 0; i < a.rows * a.columns; i++) {
        result.values[i] = a.values[i] * b.values[i];
    }
    return result;
}

Matrix& Matrix::scale(double scalar) {
    for (int i = 0; i < this->rows; i++) {
        for (int j = 0; j < this->columns; j++) {
            this->set(i, j, this->at(i, j) * scalar);
        }
    }
    return *this;
}

Matrix Matrix::scale(const Matrix& a, double scalar) {
    Matrix res = a;
    for (int i = 0; i < res.rows; i++) {
        for (int j = 0; j < res.columns; j++) {
            res.set(i, j, res.at(i, j) * scalar);
        }
    }
    return res;
}

Matrix Matrix::multiply(const Matrix& a, const Matrix& b) {
    if (a.columns != b.rows) {
        throw std::runtime_error("Impossible matrix multiplication");
    }
    Matrix res(a.rows, b.columns);
    for (int i = 0; i < a.rows; i++) {
        for (int j = 0; j < b.columns; j++) {
            double sum = 0;
            for (int k = 0; k < a.columns; k++) {
                sum += res.at(i, j) + a.at(i, k) * b.at(k, j);
            }
            res.set(i, j, sum);
        }
    }
    return res;
}

void Matrix::print_by_row() {
    for (int i = 0; i < rows; i++) {
        std::cout << "|";
        for (int j = 0; j < columns; j++) {
            std::cout << at(i, j) << ", ";
        }
        std::cout << "|\n";
    }
}

void Matrix::print_by_column() {
    for (int i = 0; i < columns; i++) {
        std::cout << "|";
        for (int j = 0; j < rows; j++) {
            std::cout << at(j, i) << ", ";
        }
        std::cout << "|\n";
    }
}