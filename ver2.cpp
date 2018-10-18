//
// Created by Александр Баташев on 12/10/2018.
//

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
    }

    if (procCount > 1) {
        int partCount = n / procCount;
        int tail = n % procCount;

        int *row = new int[m];
        int *min = nullptr;

        for (int i = 0; i < n - tail; i += partCount) {
            int offset = i == 0 ? 0 : (i - 1)*m;
            int *send = procId == 0 ? matrix->raw() + offset : nullptr;
            min = procId == 0 ? matrix->raw() + (i + partCount - 1) * m : nullptr;
            MPI_Scatter(send, m, MPI_INT, row, m, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Reduce(row, min, m, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);
        }

        if (tail > 0) {
            if (procId == 0) {
                for (int i = n - tail, j = 1; i < n && j <= tail; i++, j++) {
                    MPI_Send(matrix->raw() + i * m, m, MPI_INT, j, 0, MPI_COMM_WORLD);
                }
                if (tail < n) {
                    for (int i = 0; i < m; i++) {
                        row[i] = matrix->raw()[(n - tail - 1) * m + i];
                    }
                } else {
                    for (int i = 0; i < m; i++) {
                        row[i] = INT_MAX;
                    }
                }
            } else if (procId <= tail) {
                MPI_Recv(row, m, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            } else if (procId > tail) {
                for (int i = 0; i < m; i++) {
                    row[i] = INT_MAX;
                }
            }

            MPI_Barrier(MPI_COMM_WORLD);
//            if (tail == n && procId == 0) {
//                min = new int[m];
//            } else {
                min = procId == 0 ? matrix->raw() + (n - 1) * m : nullptr;
//            }
            MPI_Reduce(row, min, m, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);
        }

        if (procId == 0) {
//            for (int i = 0; i < m; i++) {
//                std::cout << min[i] << " ";
//            }
            std::cout << std::endl << "Complete in " << MPI_Wtime() - startTime << "s" << std::endl;

        }

    } else {
        std::cout << "More than 1 process is needed!" << std::endl;
    }

    MPI_Finalize();

    return 0;
}