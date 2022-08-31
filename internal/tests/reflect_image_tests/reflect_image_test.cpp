#include "../compare.h"
#include "../../image.h"

void test(const std::string& in, const std::string& out) {
    auto in_image = jpeg_image::JpegImage::ReadImageFromJpegFile("./internal/tests/reflect_image_tests/test_case/" + in);
    auto out_image = jpeg_image::JpegImage::ReadImageFromJpegFile("./internal/tests/reflect_image_tests/test_case/" + out);

    auto reflect = jpeg_image::ReflectImage(in_image);

    Compare(reflect, out_image);
}

int main(int argc, char **argv) {
    for (int i = 1; i < 3; ++i) {
        test(std::to_string(i) + "_in.jpg", std::to_string(i) + "_out.jpg");
    }
}