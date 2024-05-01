#include <gtest/gtest.h>
#include <vector>
#include <math.h>
#include <random>
#include <iostream>

#include "comp_methods.h"

class InterpolationTest : public testing::Test {
protected:
    void SetUp() override {
        src.reserve(length);
        ref.reserve(length);
        dst.reserve(length);

        std::default_random_engine generator;
        std::uniform_real_distribution<double> distribution(left, right);

        for (float x = left; x <= right; x += step) {
            pointX.push_back(x);
            pointY.push_back(exp(sin(x)));
        }

        for (uint32_t i = 0; i < length; ++i) {
            src[i] = distribution(generator);
            ref[i] = exp(sin(src[i]));
        }
    }

    float left = -2.0f;
    float right = 2.0f;
    float step = 0.05f;
    std::vector<float> pointX, pointY;

    uint32_t length = 100;
    std::vector<float> src;
    std::vector<float> ref;
    std::vector<float> dst;
};

TEST_F(InterpolationTest, SplineLinTest) {
    float max_err = 0.0f;
    interpolation(src.data(), dst.data(), length, pointX.data(), pointY.data(), pointY.size(), INT_SPLINE_LIN);
    for (int i = 0; i < length; ++i) {
        if (std::abs(dst[i] - ref[i]) > max_err) max_err = std::abs(dst[i] - ref[i]);
        EXPECT_LT(std::abs(dst[i] - ref[i]), 0.05) << "Vectors dst (" << dst[i] << ") and ref (" << ref[i] << ") differ at index " << i << ". src = " << src[i];
    }
    std::cout << max_err << std::endl;
}

TEST_F(InterpolationTest, SplineParTest) {
    float max_err = 0.0f;
    interpolation(src.data(), dst.data(), length, pointX.data(), pointY.data(), pointY.size(), INT_SPLINE_PAR);
    for (int i = 0; i < length; ++i) {
        if (std::abs(dst[i] - ref[i]) > max_err) max_err = std::abs(dst[i] - ref[i]);
        EXPECT_LT(std::abs(dst[i] - ref[i]), 0.05) << "Vectors dst (" << dst[i] << ") and ref (" << ref[i] << ") differ at index " << i << ". src = " << src[i];
    }
    std::cout << max_err << std::endl;
}

TEST_F(InterpolationTest, SplineCubTest) {
    float max_err = 0.0f;
    interpolation(src.data(), dst.data(), length, pointX.data(), pointY.data(), pointY.size(), INT_SPLINE_CUB);
    for (int i = 0; i < length; ++i) {
        if (std::abs(dst[i] - ref[i]) > max_err) max_err = std::abs(dst[i] - ref[i]);
        EXPECT_LT(std::abs(dst[i] - ref[i]), 0.05) << "Vectors dst (" << dst[i] << ") and ref (" << ref[i] << ") differ at index " << i << ". src = " << src[i];
    }
    std::cout << "Max error: " << max_err << std::endl;
}
