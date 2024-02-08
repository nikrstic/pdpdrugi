#include <omp.h>

#include <chrono>
#include <iostream>
#include <random>

using namespace std;
using namespace std::chrono;

/*
Osnovna operacija koju ćemo vršiti je dodavanje nekog i-tog reda nekom j-tom redu,
pri čemu je i-ti red skaliran factorom.
*/
template <typename T>
void transform_row(T** AAi, int n, int row_i, int row_j, T factor) {
    for (int j = row_i; j < 2 * n; j++)
        AAi[row_j][j] += AAi[row_i][j] * factor;
}

template <typename T>
void gauss_elimination(T** A, T** Ai, int n, int num_threads = 8) {
    // AAi = [A | In]
    T** AAi = new T*[n];
    for (int i = 0; i < n; i++)
        AAi[i] = new T[2 * n];

#pragma omp parallel num_threads(num_threads)
    {
#pragma omp for
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++)
                AAi[i][j] = A[i][j];
            for (int j = n; j < 2 * n; j++)
                AAi[i][j] = 0;
            AAi[i][n + i] = 1;
        }
#pragma omp single
        {
            for (int pivot_i = 0; pivot_i < n; pivot_i++) {
                T pivot = AAi[pivot_i][pivot_i];
                for (int i = pivot_i + 1; i < n; i++) {
                    // dodati i-toj vrsti vrstu pivot_i * factor
                    T factor = -1 * AAi[i][pivot_i] / pivot;
#pragma omp task if (n - pivot_i > 256)
                    transform_row(AAi, n, pivot_i, i, factor);
                }
            }
        }
    }
    // dobila se gornjo trougaona matrica
    // radimo isto samo obrnuto kako bi smo dobili dijagonalnu
    for (int i = n - 1; i >= 0; i--) {
        T pivot = AAi[i][i];
#pragma omp parallel for num_threads(num_threads)
        for (int row_above = i - 1; row_above >= 0; row_above--) {
            T factor = -1 * AAi[row_above][i] / pivot;
            AAi[row_above][i] = 0;
            for (int j = n; j < 2 * n; j++)
                AAi[row_above][j] += AAi[i][j] * factor;
        }
    }
    // [D | D*Ai] -> [In | Ai]
    for (int i = 0; i < n; i++) {
        T pivot = AAi[i][i];
        for (int j = n; j < 2 * n; j++)
            AAi[i][j] /= pivot;
        AAi[i][i] = 1;
    }
    // upisati u Ai
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            Ai[i][j] = AAi[i][n + j];

    for (int i = 0; i < n; i++)
        delete[] AAi[i];
    delete[] AAi;
}

int main() {
    using T = double;

    int n = 4;
    T** A = new T*[n];
    T** Ai = new T*[n];
    for (int i = 0; i < n; i++) {
        A[i] = new T[n];
        Ai[i] = new T[n];
    }

    A[0][0] = 4;
    A[0][1] = 1;
    A[0][2] = 2;
    A[0][3] = -3;
    A[1][0] = -3;
    A[1][1] = 3;
    A[1][2] = -1;
    A[1][3] = 4;
    A[2][0] = -1;
    A[2][1] = 2;
    A[2][2] = 5;
    A[2][3] = 1;
    A[3][0] = 5;
    A[3][1] = 4;
    A[3][2] = 3;
    A[3][3] = -1;

    gauss_elimination(A, Ai, n, 1);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            cout << A[i][j] << " ";
        cout << endl;
    }
    cout << endl;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            cout << Ai[i][j] << " ";
        cout << endl;
    }

    return 0;
}
