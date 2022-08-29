#pragma once

#include <vector>
#include <cstddef>
#include <string>
#include <algorithm>

namespace jpeg_image {

struct RGB {
    int r, g, b;
};

class JpegImage {
public:
    static JpegImage ReadImageFromJpegFile(const std::string filename);

public:
    void WriteImageToJpegFile(const std::string filename);

    void SetSize(size_t width, size_t height) {
        data_.assign(height, std::vector<RGB>(width));
    }

    size_t GetWidth() const {
        if (data_.empty()) {
            return 0;
        }

        return data_[0].size();
    }

    size_t GetHeight() const {
        return data_.size();
    }

    void SetPixel(int y, int x, const RGB& pixel) {
        data_[y][x] = pixel;
    }

    RGB GetPixel(int y, int x) const {
        return data_[y][x];
    }

    RGB& GetPixel(int y, int x) {
        return data_[y][x];
    }

    void SetComment(const std::string& comment) {
        comment_ = comment;
    }

    const std::string& GetComment() const {
        return comment_;
    }

    std::vector<std::vector<RGB>> GetData() const {
        return data_;
    }

private:
    std::vector<std::vector<RGB>> data_;
    std::string comment_;
};

Image ReflectImage(const Image& image) {
    Image result;

    auto image_data = image.GetData();

    for (size_t i = 0; i < data.size(); i += image.GetWidth()) {
        std::reverse(std::next(data.begin(), i),
                    std::next(data.end(), i + image.GetWidth()));
    }

    result.SetConfig(image.GetConfig());
    result.SetData(std::move(data));

    return result;
}

}