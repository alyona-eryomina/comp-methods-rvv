#include "tests.h"

int main()
{
    float left = -2.0f;
    float right = 2.0f;
    float step = 0.05f;

    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(left, right);

    std::vector<float> pointX, pointY;

    for (float x = left; x <= right; x += step)
    {
        pointX.push_back(x);
        pointY.push_back(exp(sin(x)));
    }

    uint32_t length = 100;
    std::vector<float> src(length);
    std::vector<float> ref(length);
    std::vector<float> dst(length);

    for (uint32_t i = 0; i < length; ++i)
    {
        src[i] = distribution(generator);
        ref[i] = exp(sin(src[i]));
    }

    SplineCub(src.data(), dst.data(), length, pointX.data(), pointY.data(), pointY.size());

    float max_diff = 0.0f;
    for (uint32_t i = 0; i < length; ++i)
    {
        if (std::abs(ref[i] - dst[i]) > max_diff) max_diff = std::abs(ref[i] - dst[i]);
        
        if (std::abs(ref[i] - dst[i]) > 0.1)
        {
            std::cout << "Error!\nsrc = " << src[i]
                            << "\nref = " << ref[i]
                            << "\ndst = " << dst[i] << std::endl;
            return -1;
        }
    }
    std::cout << "Success! Max error = " << max_diff << "\n";

    return 0;
}
