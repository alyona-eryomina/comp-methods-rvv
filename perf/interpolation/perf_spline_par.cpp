#include "perf.h"

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

    unsigned long start, end, diff_ref, diff_dst;

    for (uint32_t length = 128; length <= 1024; length *= 2)
    {
        std::vector<float> src(length);
        std::vector<float> ref(length);
        std::vector<float> dst(length);

        for (uint32_t i = 0; i < length; ++i)
        {
            src[i] = distribution(generator);
        }

        diff_ref = 100000;
        diff_dst = 100000;

        for (int u = 0; u < 3; u++)
        {
            start = rdcycle();
            refSplinePar(src.data(), ref.data(), length, pointX.data(), pointY.data(), pointY.size());
            end = rdcycle();

            if (diff_ref > (end - start)) diff_ref = end - start;
        }

        for (int u = 0; u < 3; u++)
        {
            start = rdcycle();
            SplinePar(src.data(), dst.data(), length, pointX.data(), pointY.data(), pointY.size());
            end = rdcycle();

            if (diff_dst > (end - start)) diff_dst = end - start;
        }

        std::cout << "len = " << length << "\t diff_ref = " << diff_ref << "\t diff_dst = " << diff_dst << std::endl;
    }

    return 0;
}