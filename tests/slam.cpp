#include <gtest/gtest.h>
#include <vector>
#include <math.h>
#include <random>
#include <iostream>

#include "comp_methods.h"

TEST(SLAMDiagonalTest, JacobiTest) {
    std::vector<double> A;
    std::vector<double> b;
    std::vector<double> x_ref;
    std::vector<double> x_dst;
    double eps = 1e-7;
    double small = 5.0;

    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(-small, small);

    for (uint32_t size = 128; size <= 1024; size *= 2) {
        A.reserve(size * size);
        b.reserve(size);
        x_ref.reserve(size);
        x_dst.reserve(size);

        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                A[i * size + j] = distribution(generator);
                if (i == j) A[i * size + j] += size * 2 * small;
            }
            x_ref[i] = distribution(generator);
        }

        for (int i = 0; i < size; i++) {
            b[i] = 0;
            for (int j = 0; j < size; j++) {
                b[i] += x_ref[j] * A[i * size + j];
            }
        }

        uint32_t iterations = slam(size, A.data(), b.data(), x_dst.data(), eps, SLAM_JACOBI);
        std::cout << "size = " << size << ", iterations = " << iterations << std::endl;

        for (int i = 0; i < size; i++) {
            if (std::abs(x_dst[i] - x_ref[i]) >= eps) {
                FAIL() << "size = " << size << ", i = " << i << ", std::abs(x_dst[i] - x_ref[i]) = " << std::abs(x_dst[i] - x_ref[i]) << std::endl;
            }
        }

        A.clear();
        b.clear();
        x_ref.clear();
        x_dst.clear();
    }
    SUCCEED();
}

TEST(SLAMDiagonalTest, GaussSeidelTest) {
    std::vector<double> A;
    std::vector<double> b;
    std::vector<double> x_ref;
    std::vector<double> x_dst;
    double eps = 1e-7;
    double small = 5.0;

    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(-small, small);

    for (uint32_t size = 128; size <= 1024; size *= 2) {
        A.reserve(size * size);
        b.reserve(size);
        x_ref.reserve(size);
        x_dst.reserve(size);

        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                A[i * size + j] = distribution(generator);
                if (i == j) A[i * size + j] += size * 2 * small;
            }
            x_ref[i] = distribution(generator);
        }

        for (int i = 0; i < size; i++) {
            b[i] = 0;
            for (int j = 0; j < size; j++) {
                b[i] += x_ref[j] * A[i * size + j];
            }
        }

        uint32_t iterations = slam(size, A.data(), b.data(), x_dst.data(), eps, SLAM_GAUSS_SEIDEL);
        std::cout << "size = " << size << ", iterations = " << iterations << std::endl;

        for (int i = 0; i < size; i++) {
            if (std::abs(x_dst[i] - x_ref[i]) >= eps) {
                FAIL() << "size = " << size << ", i = " << i << ", std::abs(x_dst[i] - x_ref[i]) = " << std::abs(x_dst[i] - x_ref[i]) << std::endl;
            }
        }

        A.clear();
        b.clear();
        x_ref.clear();
        x_dst.clear();
    }
    SUCCEED();
}

TEST(SLAMSymmetricTest, ConjugateGradientTest) {
    std::vector<double> A;
    std::vector<double> b;
    std::vector<double> x_ref;
    std::vector<double> x_dst;
    double eps = 1e-7;
    double small = 1.0;

    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(-small, small);

    for (uint32_t size = 128; size <= 1024; size *= 2) {
        A.reserve(size * size);
        b.reserve(size);
        x_ref.reserve(size);
        x_dst.reserve(size);

        for (int i = 0; i < size; i++) {
            for (int j = i; j < size; j++) {
                A[i * size + j] = distribution(generator);
                A[j * size + i] = A[i * size + j];
            }
            x_ref[i] = distribution(generator);
        }

        for (int i = 0; i < size; i++) {
            b[i] = 0;
            for (int j = 0; j < size; j++) {
                b[i] += x_ref[j] * A[i * size + j];
            }
        }

        uint32_t iterations = slam(size, A.data(), b.data(), x_dst.data(), eps, SLAM_CONJ_GRADIEND);
        std::cout << "size = " << size << ", iterations = " << iterations << std::endl;

        for (int i = 0; i < size; i++) {
            if (std::abs(x_dst[i] - x_ref[i]) >= eps) {
                FAIL() << "size = " << size << ", i = " << i << ", std::abs(x_dst[i] - x_ref[i]) = " << std::abs(x_dst[i] - x_ref[i]) << std::endl;
            }
        }

        A.clear();
        b.clear();
        x_ref.clear();
        x_dst.clear();
    }
    SUCCEED();
}
