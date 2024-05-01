#include <gtest/gtest.h>
#include <vector>
#include <math.h>
#include <random>
#include <iostream>

#include "comp_methods.h"

class SLAMTest : public testing::Test {
protected:
    void SetUp() override {
        A.reserve(size * size);
        b.reserve(size);
        x_ref.reserve(size);
        x_dst.reserve(size);

        std::default_random_engine generator;
        std::uniform_real_distribution<double> distribution(-small, small);

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
    }

    uint32_t size = 128;
    std::vector<double> A;
    std::vector<double> b;
    std::vector<double> x_ref;
    std::vector<double> x_dst;
    double eps = 1e-10;

    double small = 5.0;
};

TEST_F(SLAMTest, JacobiTest) {
    uint32_t iterations = slam(size, A.data(), b.data(), x_dst.data(), eps, SLAM_JACOBI);

    for (int i = 0; i < size; i++) {
        EXPECT_LT(std::abs(x_dst[i] - x_ref[i]), eps);
    }
    for (int i = 0; i < size; i++) {
        std::cout << x_dst[i] << " ";
    }
    std::cout << std::endl;
    for (int i = 0; i < size; i++) {
        std::cout << x_ref[i] << " ";
    }
    std::cout << std::endl;
    std::cout << "Iterations: " << iterations << std::endl;
}

TEST_F(SLAMTest, GaussSeidelTest) {
    uint32_t iterations = slam(size, A.data(), b.data(), x_dst.data(), eps, SLAM_GAUSS_SEIDEL);

    for (int i = 0; i < size; i++) {
        EXPECT_LT(std::abs(x_dst[i] - x_ref[i]), eps);
    }
    std::cout << "Iterations: " << iterations << std::endl;
}
