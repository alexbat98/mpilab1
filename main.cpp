#include <iostream>
#include <mpi.h>
#include <random>
#include <iomanip>
#include <iterator>
#include <algorithm>

void transpose(const int *src, int *dst, const int N, const int M) {
#pragma omp parallel for
    for(int n = 0; n<N*M; n++) {
        int i = n/N;
        int j = n%N;
        dst[n] = src[M*j + i];
    }
}

//template<class RandomIterator>
//void transpose(RandomIterator first, RandomIterator last, int m)
//{
//    const int mn1 = (last - first - 1);
//    const int n   = (last - first) / m;
//    std::vector<bool> visited(last - first);
//    RandomIterator cycle = first;
//    while (++cycle != last) {
//        if (visited[cycle - first])
//            continue;
//        int a = cycle - first;
//        do  {
//            a = a == mn1 ? mn1 : (n * a) % mn1;
//            std::swap(*(first + a), *cycle);
//            visited[a] = true;
//        } while ((first + a) != cycle);
//    }
//}

void printMatrix(int *matrix, int n, int m) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            std::cout << std::setw(5) << matrix[i*m + j] << " ";
        }
        std::cout << std::endl;
    }
}


int main(int argc, char* argv[]) {

    MPI_Init(&argc, &argv);

    int procId, procCount;

    MPI_Comm_rank(MPI_COMM_WORLD, &procId);
    MPI_Comm_size(MPI_COMM_WORLD, &procCount);

    if (argc < 3 || argc > 4) {
        std::cout << "Wrong number of arguments!" << std::endl;
    }

    int n = std::stoi(std::string(argv[1]));
    int m = std::stoi(std::string(argv[2]));

    int *array = nullptr;

    if (procId == 0) { // main process

        array  = new int[n * m];

        bool showMatrix = false;

        if (argc == 4 && std::string(argv[3]) == "true") {
            showMatrix = true;
        }

        std::default_random_engine generator;
        std::uniform_int_distribution<int> distribution(-1000,1000);

        for (int i = 0; i < n * m; i++) {
            array[i] = distribution(generator);
        }

        if (showMatrix) {
            std::cout << "Original matrix" << std::endl;
            printMatrix(array, n, m);
        }

        //transpose(array, array + n*m, m);
        int *nArr = new int[n * m];
        transpose(array, nArr, n, m);
        std::cout << "Transposed matrix" << std::endl;
        printMatrix(nArr, m, n);
        array = nArr;

    }

    int partSize = m / procCount;

    int *matrixPart = new int[n*partSize];

    MPI_Scatter(array, n * partSize, MPI_INT, matrixPart, n * partSize, MPI_INT, 0, MPI_COMM_WORLD);

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
        int tail = m - procCount * partSize; // todo check +1
        for (int i = tail + 1; i < m; i++) {
            int min = INT_MAX;
            for (int j = 0; j < n; j++) {
                if (array[i * n + j] < min) {
                    min = array[i * n + j];
                }
            }
            totalMin[i] = min;
        }

        std::cout << std::endl;
        for (int i = 0; i < m; i++) {
            std::cout << std::setw(5) << totalMin[i] << " ";
        }
        std::cout << std::endl;
    }

    MPI_Finalize();

    return 0;
}