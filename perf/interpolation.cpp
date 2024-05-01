#include <benchmark/benchmark.h>
#include <vector>
#include <math.h>
#include <random>
#include <iostream>

#include "comp_methods.h"

class InterpolationFixture : public benchmark::Fixture {
    protected:
    void SetUp(benchmark::State& state) override {
        std::default_random_engine generator;
        std::uniform_real_distribution<double> distribution(left, right);

        for (float x = left; x <= right; x += step)
        {
            pointX.push_back(x);
            pointY.push_back(exp(sin(x)));
        }

        length = state.range(1);
        src.reserve(length);
        dst.reserve(length);

        for (uint32_t i = 0; i < length; ++i)
        {
            src[i] = distribution(generator);
        }

    }

    float left = -2.0f;
    float right = 2.0f;
    float step = 0.05f;
    std::vector<float> pointX, pointY;

    uint32_t length;
    std::vector<float> src;
    std::vector<float> dst;
};

BENCHMARK_DEFINE_F(InterpolationFixture, InterpolationBench)(benchmark::State& state) {
    while (state.KeepRunning()) {
        interpolation(src.data(), dst.data(), length, pointX.data(), pointY.data(), pointY.size(), static_cast<InterpolationMethod>(state.range(0)));
    }
};

BENCHMARK_REGISTER_F(InterpolationFixture, InterpolationBench) -> 
    ArgsProduct({
      {INT_SPLINE_LIN, INT_SPLINE_PAR, INT_SPLINE_CUB},
      benchmark::CreateRange(128, 1024, 2)
});
