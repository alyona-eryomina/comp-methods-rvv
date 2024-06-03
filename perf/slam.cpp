#include <benchmark/benchmark.h>
#include <vector>
#include <math.h>
#include <random>
#include <iostream>

#include "comp_methods.h"

class SLAMDiagonalFixture : public benchmark::Fixture {
protected:
    void SetUp(benchmark::State& state) override {
        std::default_random_engine generator;
        std::uniform_real_distribution<double> distribution(-small, small);

        size = state.range(1);
        A.reserve(size * size);
        b.reserve(size);
        x.reserve(size);
        x_ref.reserve(size);

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

    uint32_t size;
    std::vector<double> A;
    std::vector<double> b;
    std::vector<double> x;
    std::vector<double> x_ref;
    double eps = 1e-5;

    double small = 5.0;
};

class SLAMSymmetricFixture : public benchmark::Fixture {
protected:
    void SetUp(benchmark::State& state) override {
        std::default_random_engine generator;
        std::uniform_real_distribution<double> distribution(-small, small);

        size = state.range(1);
        A.reserve(size * size);
        b.reserve(size);
        x.reserve(size);
        x_ref.reserve(size);

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
    }

    uint32_t size;
    std::vector<double> A;
    std::vector<double> b;
    std::vector<double> x;
    std::vector<double> x_ref;
    double eps = 1e-5;

    double small = 1.0;
};

BENCHMARK_DEFINE_F(SLAMDiagonalFixture, SLAMDiagonalBench)(benchmark::State& state) {
    uint32_t iterations;
    while (state.KeepRunning()) {
        iterations = slam(size, A.data(), b.data(), x.data(), eps, static_cast<SlamMethod>(state.range(0)));
    }
    //std::cout << "iterations = " << iterations << std::endl;
};

BENCHMARK_DEFINE_F(SLAMSymmetricFixture, SLAMSymmetricBench)(benchmark::State& state) {
    uint32_t iterations;
    while (state.KeepRunning()) {
        iterations = slam(size, A.data(), b.data(), x.data(), eps, static_cast<SlamMethod>(state.range(0)));
    }
    //std::cout << "iterations = " << iterations << std::endl;
};

BENCHMARK_REGISTER_F(SLAMDiagonalFixture, SLAMDiagonalBench) -> 
    ArgsProduct({
      {SLAM_JACOBI, SLAM_GAUSS_SEIDEL},
      benchmark::CreateRange(128, 1024, 2)
});

BENCHMARK_REGISTER_F(SLAMSymmetricFixture, SLAMSymmetricBench) -> 
    ArgsProduct({
      {SLAM_CONJ_GRADIEND},
      benchmark::CreateRange(128, 1024, 2)
});

