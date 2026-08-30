#pragma once
#include <functional>
#include <random>

class Matrix {
    public:
    std::vector<double> values;

    int rows;
    int columns;

    Matrix();
    Matrix(int rows, int columns);
    Matrix(int rows, int columns, std::mt19937& rng, double scale);
    Matrix(std::vector<double> values, int rows, int columns);

    void set(int i, int j, double value);
    double at(int i, int j) const;
    Matrix& transpose();
    Matrix& flatten();
    Matrix& add(const Matrix& a);
    Matrix& subtract(const Matrix& a);
    Matrix& scale(double scalar);
    Matrix& hadamard(const Matrix& a);
    static Matrix add(const Matrix& a, const Matrix& b);
    static Matrix subtract(const Matrix& a, const Matrix& b);
    static Matrix scale(const Matrix& a, double scalar);
    static Matrix multiply(const Matrix& a, const Matrix& b);
    static Matrix transpose(const Matrix& a);
    static Matrix hadamard(const Matrix& a, const Matrix& b);
    
    void print_by_column();
    void print_by_row();
    template <typename F>
    void perform(F func) {
        for (auto& value : values) {
            value = func(value);
        }
    }
};

