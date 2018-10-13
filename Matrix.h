//
// Created by Александр Баташев on 12/10/2018.
//

#ifndef MPILAB1_MATRIX_H
#define MPILAB1_MATRIX_H

#include <iostream>
#include <iomanip>
#include <random>

template<typename T>
class Matrix {
private:
    T* data;
    int rows;
    int cols;
public:
    Matrix(const int n, const int m) : rows(n), cols(m), data(new T[n*m]) {};
    ~Matrix() {
        delete data;
    }

    Matrix(const Matrix &matrix) {
        rows = matrix.rows;
        cols = matrix.cols;
        data = new T[rows * cols];

        for (int i = 0; i < rows * cols; i++) {
            data[i] = matrix.data[i];
        }
    }

    Matrix &operator=(const Matrix &matrix) {
        delete data;
        rows = matrix.rows;
        cols = matrix.cols;
        data = new T[rows * cols];

        for (int i = 0; i < rows * cols; i++) {
            data[i] = matrix.data[i];
        }
        return *this;
    }

    void transpose() {
        T *temp = new T[cols * rows];
#pragma omp parallel for
        for(int n = 0; n<rows*cols; n++) {
            int i = n/rows;
            int j = n%rows;
            temp[n] = data[cols*j + i];
        }
        delete data;
        data = temp;
        int t = cols;
        cols = rows;
        rows = t;
    }

    void print() {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                std::cout << std::setw(5) << data[i*cols + j] << " ";
            }
            std::cout << std::endl;
        }
    }

    void fillWithRand() {
        std::default_random_engine generator(static_cast<unsigned int>(time(nullptr)));
        std::normal_distribution<double> distribution(1000,700);

        for (int i = 0; i < rows * cols; i++) {
            data[i] = static_cast<T>(distribution(generator));
        }
    }

    T* raw() {
        return data;
    }
};


#endif //MPILAB1_MATRIX_H
