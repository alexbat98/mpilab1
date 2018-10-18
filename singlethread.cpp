//
// Created by Александр Баташев on 14/10/2018.
//

#include "Matrix.h"
#include <chrono>

int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 4) {
        std::cout << "Wrong number of arguments!" << std::endl;
    }

    const int n = std::stoi(std::string(argv[1]));
    const int m = std::stoi(std::string(argv[2]));

    bool showMatrix = false;

    if (argc == 4 && std::string(argv[3]) == "true") {
        showMatrix = true;
    }

    Matrix<int> matrix(n, m);

    int *min = new int[m];

    for (int i = 0; i < m; i++) {
        min[i] = INT_MAX;
    }

    matrix.fillWithRand();

    if (showMatrix) {
        matrix.print();
    }

    auto start = std::chrono::system_clock::now();

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            min[j] = std::min(min[j], matrix.raw()[i * m + j]);
        }
    }

//    for (int i = 0; i < m; i++) {
//        std::cout << min[i] << " ";
//    }

    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> diff = end-start;

    std::cout << std::endl << "Complete in " << diff.count() << "s" <<std::endl;

}