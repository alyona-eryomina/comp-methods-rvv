#include <benchmark/benchmark.h>
#include <vector>
#include <math.h>
#include <random>
#include <iostream>

#include "comp_methods.h"

class SLAMFixture : public benchmark::Fixture {
protected:
    void SetUp(benchmark::State& state) override {
        std::default_random_engine generator;
        std::uniform_real_distribution<double> distribution(-small, small);

        size = state.range(1);
        A.reserve(size * size);
        b.reserve(size);
        x.reserve(size);

        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                A[i * size + j] = distribution(generator);
                if (i == j) A[i * size + j] += size * 2 * small;
            }
            b[i] = distribution(generator);
        }
    }

    uint32_t size;
    std::vector<double> A;
    std::vector<double> b;
    std::vector<double> x;
    double eps = 1e-10;

    double small = 5.0;
};

BENCHMARK_DEFINE_F(SLAMFixture, SLAMBench)(benchmark::State& state) {
    while (state.KeepRunning()) {
        slam(size, A.data(), b.data(), x.data(), eps, static_cast<SlamMethod>(state.range(0)));
    }
};

BENCHMARK_REGISTER_F(SLAMFixture, SLAMBench) -> 
    ArgsProduct({
      {SLAM_JACOBI, SLAM_GAUSS_SEIDEL},
      benchmark::CreateRange(128, 1024, 2)
});
