//
// Created by Александр Баташев on 18/10/2018.
//

#include <mpi.h>
#include <iostream>
#include <random>
#include <iomanip>

void printMatrix(int *matrix, int rows, int columns) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            std::cout << std::setw(5) << matrix[i * columns + j] << " ";
        }
        std::cout << std::endl;

    }
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int procId, procCount;
    double startTime = .0;

    MPI_Comm_rank(MPI_COMM_WORLD, &procId);
    MPI_Comm_size(MPI_COMM_WORLD, &procCount);

    if (argc < 3 || argc > 4) {
        std::cout << "Wrong number of arguments!" << std::endl;
    }

    const int rows = std::stoi(std::string(argv[1]));
    const int columns = std::stoi(std::string(argv[2]));

    int *matrix = nullptr;
    if (procId == 0) {
        matrix = new int[rows * columns];

        std::default_random_engine generator(static_cast<unsigned int>(time(nullptr)));
        std::normal_distribution<double> distribution(1000, 700);

        for (int i = 0; i < rows * columns; i++) {
            matrix[i] = static_cast<int>(distribution(generator));
        }

        bool showMatrix = false;

        if (argc == 4 && std::string(argv[3]) == "true") {
            showMatrix = true;
        }

        if (showMatrix) {
            printMatrix(matrix, rows, columns);
        }

    }

    int partSize = rows / procCount;
    int tail = rows % procCount;

    int *sizes = procId == 0 ? new int[procCount] : nullptr;
    int *displacements = procId == 0 ? new int[procCount] : nullptr;

    if (procId == 0) {
        for (int i = 0; i < procCount - 1; i++) {
            sizes[i] = partSize * columns;
            displacements[i] = i * partSize * columns;
        }
        sizes[procCount - 1] = (partSize + tail) * columns;
        displacements[procCount - 1] = rows * columns - (partSize + tail) * columns;
    }

    startTime = MPI_Wtime();

    int receiveCount =
            procId == procCount - 1 ? (partSize + tail) * columns : partSize * columns;

    int *receiveBuffer = new int[receiveCount];

    MPI_Scatterv(matrix, sizes, displacements, MPI_INT, receiveBuffer, receiveCount, MPI_INT, 0,
                 MPI_COMM_WORLD);

    int *minRow = new int[columns];

    for (int i = 0; i < columns; i++) {
        minRow[i] = INT_MAX;
    }

    for (int i = 0; i < receiveCount / columns; i++) {
        for (int j = 0; j < columns; j++) {
            minRow[j] = std::min(minRow[j], receiveBuffer[i * columns + j]);
        }
    }

    int *result = procId == 0 ? new int[columns] : nullptr;

    MPI_Reduce(minRow, result, columns, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);

    if (procId == 0) {
        std::cout << std::endl;
        for (int i = 0; i < columns; i++) {
            std::cout << result[i] << " ";
        }
        std::cout << std::endl << "Complete in " << MPI_Wtime() - startTime << "s" << std::endl;
    }
    return 0;

}