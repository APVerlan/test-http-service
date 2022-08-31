#include "../compare.h"
#include "../../image.h"

void test(const std::string& filename) {
    auto image = jpeg_image::JpegImage::ReadImageFromJpegFile("./internal/tests/read_write_image_tests/test_case/" + filename);

    image.WriteImageToJpegFile("./internal/tests/read_write_image_tests/test_case/test_" + filename);

    auto new_image = jpeg_image::JpegImage::ReadImageFromJpegFile("./internal/tests/read_write_image_tests/test_case/test_" + filename);

    Compare(image, new_image);
}

int main(int argc, char **argv) {
    for (int i = 1; i < 3; ++i) {
        test(std::to_string(i) + ".jpg");
    }
}