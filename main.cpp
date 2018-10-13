#include <iostream>
#include <mpi.h>
#include <random>
#include <iomanip>
#include <iterator>
#include <algorithm>
#include <string>
#include "Matrix.h"

int main(int argc, char *argv[]) {

    MPI_Init(&argc, &argv);

    int procId, procCount;
    double startTime = .0;

    MPI_Comm_rank(MPI_COMM_WORLD, &procId);
    MPI_Comm_size(MPI_COMM_WORLD, &procCount);

    if (argc < 3 || argc > 4) {
        std::cout << "Wrong number of arguments!" << std::endl;
    }

    const int n = std::stoi(std::string(argv[1]));
    const int m = std::stoi(std::string(argv[2]));

    Matrix<int> *matrix = nullptr;

    if (procId == 0) { // main process

        matrix = new Matrix<int>(n, m);

        bool showMatrix = false;

        if (argc == 4 && std::string(argv[3]) == "true") {
            showMatrix = true;
        }

        matrix->fillWithRand();

        startTime = MPI_Wtime();

        if (showMatrix) {
            std::cout << "Original matrix" << std::endl;
            matrix->print();
        }

        matrix->transpose();

        if (showMatrix) {
            std::cout << "Transposed matrix" << std::endl;
            matrix->print();
        }

    }

    int partSize = m / procCount;

    int *matrixPart = new int[n * partSize];

    MPI_Scatter(matrix->raw(),
                n * partSize, MPI_INT, matrixPart, n * partSize, MPI_INT, 0,
                MPI_COMM_WORLD);

    int *resMin = new int[partSize];

    for (int i = 0; i < partSize; i++) {
        int min = INT_MAX;
        for (int j = 0; j < n; j++) {
            if (matrixPart[i * n + j] < min) {
                min = matrixPart[i * n + j];
            }
        }
        resMin[i] = min;
    }

    int *totalMin = nullptr;

    if (procId == 0) {
        totalMin = new int[m];
    }

    MPI_Gather(resMin, partSize, MPI_INT, totalMin, partSize, MPI_INT, 0, MPI_COMM_WORLD);

    if (procId == 0) {
        int tail = m - procCount * partSize;
        for (int i = tail + 1; i < m; i++) {
            int min = INT_MAX;
            for (int j = 0; j < n; j++) {
                if (matrix->raw()[i * n + j] < min) {
                    min = matrix->raw()[i * n + j];
                }
            }
            totalMin[i] = min;
        }

        std::cout << std::endl;
        for (int i = 0; i < m; i++) {
            std::cout << std::setw(5) << totalMin[i] << " ";
        }
        std::cout << std::endl;

        std::cout << std::endl << "Complete in " << MPI_Wtime() - startTime << "s" << std::endl;
    }

    MPI_Finalize();

    return 0;
}