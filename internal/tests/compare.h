#ifndef _COMPARE_H_
#define _COMPARE_H_

#include "../image.h"

#include <cmath>
#include <cassert>


using namespace jpeg_image;

int sqr(int x) {
    return x * x;
}


double Distance(const RGB& lhs, const RGB& rhs) {
    return sqrt(sqr(lhs.r - rhs.r) + sqr(lhs.g - rhs.g) + sqr(lhs.b - rhs.b));
}

void Compare(const JpegImage& actual, const JpegImage& expected) {
    double max = 0;
    double mean = 0;
    assert(actual.GetWidth() == expected.GetWidth());
    assert(actual.GetHeight() == expected.GetHeight());
    for (size_t y = 0; y < actual.GetHeight(); ++y) {
        for (size_t x = 0; x < actual.GetWidth(); ++x) {
            auto actual_data = actual.GetPixel(y, x);
            auto expected_data = expected.GetPixel(y, x);
            auto diff = Distance(actual_data, expected_data);
            max = std::max(max, diff);
            mean += diff;
        }
    }

    mean /= actual.GetWidth() * actual.GetHeight();
    assert(mean <= 5);
}


#endif