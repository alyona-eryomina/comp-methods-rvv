#include "slam.h"

// size - размерность матрицы
// A - матрица коэффициентов
// b - матрица свободных членов
// х - результат
extern inline uint32_t conjugate_gradient(uint32_t size, double* A, double* b, double* x, double eps, SLAMGradBuf* buf) {
    uint32_t iterations = 0;

    double spr_0 = 0.0f;
    // Начальное приближение
    for (int i = 0; i < size; i++) {
        x[i] = 0.0;
        buf->r[i] = b[i];
        buf->z[i] = b[i];
        spr_0 += buf->r[i] * buf->r[i];
    }

    double spz = 0.0f;
    double spr_1 = 0.0f;
    double beta = 0.0f;
    do {
        // Вычисляем числитель и знаменатель для коэффициента
        // alpha = (rk-1,rk-1)/(Azk-1,zk-1) */
        spz = 0.0f;
        for (int i = 0; i < size; i++) {
            buf->s[i] = 0.0f;
            for (int j = 0; j < size; j++) {
                buf->s[i] += A[i * size + j] * buf->z[j];
            }
            spz += buf->s[i] * buf->z[i];
        }
        double alpha = spr_0 / spz;

        // Вычисляем вектор решения: xk = xk-1+ alpha * zk-1, 
        // вектор невязки: rk = rk-1 - alpha * A * zk-1 и числитель для betaa равный (rk,rk)
        spr_1 = 0;
        for (int i = 0; i < size; i++) {
            x[i] += alpha * buf->z[i];

            buf->r[i] -= alpha * buf->s[i];
            spr_1 += buf->r[i] * buf->r[i];
        }

        // вычисляем beta
        beta = spr_1 / spr_0;

        // Вычисляем вектор спуска: zk = rk+ beta * zk-1
        for (int i = 0; i < size; i++) {
            buf->z[i] = buf->r[i] + beta * buf->z[i];
        }

        spr_0 = spr_1;

        iterations += 1;
        //printf("%d: %f\n", iterations, sqrt(spr_0));
    } while (sqrt(spr_0) > eps);

    return iterations;
}

// #include <vector> 
// #include <math.h> 
// #include <omp.h> 
 
// struct CRSMatrix 
// { 
//     int n; // число строк в матрице 
//     int m; // число столбцов в матрице 
//     int nz; // число ненулевых элементов в разреженной симметричной матрице, лежащих не ниже главной диагонали 
//     std::vector<double> val; // массив значений матрицы по строкам 
//     std::vector<int> colIndex; // массив номеров столбцов 
//     std::vector<int> rowPtr; // массив индексов начала строк 
// };
 
// void Multiplicate(CRSMatrix & A, CRSMatrix & AT, double* x, double* b) 
// { 
//     #pragma omp parallel for 
//     for (int i = 0; i < A.n; ++i) 
//     { 
//         b[i] = 0.0; 
//         for (int j = A.rowPtr[i]; j < A.rowPtr[i + 1]; ++j) 
//         { 
//             if (A.colIndex[j] != i) 
//             { 
//                 b[i] += A.val[j] * x[A.colIndex[j]]; 
//             } 
//         } 
//         for (int j = AT.rowPtr[i]; j < AT.rowPtr[i + 1]; ++j) 
//         { 
//             b[i] += AT.val[j] * x[AT.colIndex[j]]; 
//         } 
//     } 
// } 
 
// // A - указатель на структуру CRSMatrix (nxn) 
// // b - столбец nx1 
// // eps - критерий остановки 
// // max_iter - критерий остановки: число итераций больше max_iter 
// // count - число выполненных итераций алгоритмом 
// void SLE_Solver_CRS(CRSMatrix & A, double * b, double eps, int max_iter, double * x, int & count) 
// { 
//     double* r = new double[A.n]; 
//     double* z = new double[A.n]; 
//     double* tmp = new double[A.n]; 
//     double ak, bk, dp_r0, dp_r1, dp_z; 
//     int S = 0; 
 
//     CRSMatrix AT; 
//     AT.n = A.n; 
//     AT.m = A.m; 
//     AT.nz = A.nz; 
//     AT.val = std::vector<double>(A.nz, 0.0); 
//     AT.colIndex = std::vector<int>(A.nz, 0); 
//     AT.rowPtr = std::vector<int>(A.n + 1, 0); 
 
//     for (int i = 0; i < A.nz; ++i) 
//     { 
//         AT.rowPtr[A.colIndex[i] + 1]++; 
//     } 
     
//     for (int i = 0; i <= A.n; ++i) 
//     { 
//         int tmpp = AT.rowPtr[i]; 
//         AT.rowPtr[i] = S; 
//         S += tmpp; 
//     } 
     
//     int iindex; 
//     for (int i = 0; i < A.n; ++i) 
//     { 
//         for (int j = A.rowPtr[i]; j < A.rowPtr[i + 1]; ++j) 
//         { 
//             iindex = AT.rowPtr[A.colIndex[j] + 1]; 
//             AT.val[iindex] = A.val[j]; 
//             AT.colIndex[iindex] = i; 
//             AT.rowPtr[A.colIndex[j] + 1]++; 
//         } 
//     } 
     
//     #pragma omp parallel for  
//     for (int i = 0; i < A.n; ++i) 
//     { 
//         x[i] = 0.0; 
//         r[i] = b[i]; 
//         z[i] = b[i]; 
//     } 
 
//     dp_r0 = 0; 
//     #pragma omp parallel for reduction(+:dp_r0) 
//     for (int i = 0; i < A.n; ++i) 
//     { 
//         dp_r0 += r[i] * r[i]; 
//     } 
 
//     for (count = 0; count < max_iter; count++) 
//     { 
//         // 1 
//         Multiplicate(A, AT, z, tmp); 
 
//         dp_z = 0; 
//         #pragma omp parallel for reduction(+:dp_z) 
//         for (int i = 0; i < A.n; ++i) 
//         { 
//             dp_z += tmp[i] * z[i]; 
//         } 
 
//         ak = dp_r0 / dp_z; 
 
//         // 2 + 3 
//         #pragma omp parallel for 
//         for (int i = 0; i < A.n; ++i) 
//         { 
//             x[i] += z[i] * ak; 
//             r[i] -= tmp[i] * ak; 
//         } 
 
//         dp_r1 = 0.0; 
//         #pragma omp parallel for reduction(+:dp_r1) 
//         for (int i = 0; i < A.n; ++i) 
//         { 
//             dp_r1 += r[i] * r[i]; 
//         } 
 
//         // 4 
//         bk = dp_r1 / dp_r0; 
 
//         // 5 
//         #pragma omp parallel for 
//         for (int i = 0; i < A.n; ++i) 
//         { 
//             z[i] = r[i] + z[i] * bk; 
//         } 
 
//         if (bk == 0.0 || sqrt(dp_r1) < eps) 
//         { 
//             break; 
//         } 
 
//         dp_r0 = dp_r1; 
//     } 
 
//     delete[] tmp; 
//     delete[] r; 
//     delete[] z; 
// }