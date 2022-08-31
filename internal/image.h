#ifndef IMAGE_H
#define IMAGE_H

#include <vector>
#include <cstddef>
#include <string>
#include <algorithm>
#include <iostream>
#include <stdexcept>

#include <jpeglib.h>

namespace jpeg_image {

struct RGB {
    unsigned char r, g, b;
};

class JpegImage {
public:
    struct JpegConfig {
        size_t width;
        size_t height;
        int num_components;
    };

public:
    static JpegImage ReadImageFromJpegFile(const std::string filename)  {
        struct jpeg_decompress_struct cinfo;
        struct jpeg_error_mgr err;
        FILE *infile = fopen(filename.c_str(), "rb");

        if (!infile) {
            throw std::runtime_error("Can't open " + filename);
        }

        cinfo.err = jpeg_std_error(&err);
        jpeg_create_decompress(&cinfo);
        jpeg_stdio_src(&cinfo, infile);

        (void)jpeg_read_header(&cinfo, true);
        (void)jpeg_start_decompress(&cinfo);

        int row_stride = cinfo.output_width * cinfo.output_components;
        JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)
            ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

        JpegImage result;
        result.SetSize(cinfo.output_width, cinfo.output_height);

        result.config_.width = cinfo.output_width;
        result.config_.height = cinfo.output_height;
        result.config_.num_components = 3;

        size_t y = 0;

        while (cinfo.output_scanline < cinfo.output_height) {
            (void) jpeg_read_scanlines(&cinfo, buffer, 1);
            for (size_t x = 0; x < result.GetWidth(); ++x) {
                RGB pixel;

                if (cinfo.output_components == 3) {
                    pixel.r = buffer[0][x * 3];
                    pixel.g = buffer[0][x * 3 + 1];
                    pixel.b = buffer[0][x * 3 + 2];
                } else if (cinfo.output_components == 1) {
                    pixel.r = pixel.g = pixel.b = buffer[0][x];
                } else {
                    throw std::runtime_error("Bad image: number of channels not 1 or 3 but it's " +
                                            std::to_string(cinfo.num_components));
                }

                result.SetPixel(y, x, pixel);
            }

            ++y;
        }

        (void)jpeg_finish_decompress(&cinfo);
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);

        return result;
    }

public:
    void WriteImageToJpegFile(const std::string filename) const {
        struct jpeg_compress_struct cinfo;
        struct jpeg_error_mgr jerr;

        FILE * outfile;
        JSAMPROW row_pointer[1];

        cinfo.err = jpeg_std_error(&jerr);
        jpeg_create_compress(&cinfo);

        if ((outfile = fopen(filename.c_str(), "wb+")) == NULL) {
            throw std::runtime_error("Can't open " + filename);
        }

        jpeg_stdio_dest(&cinfo, outfile);

        cinfo.image_width = GetConfig().width;
        cinfo.image_height = GetConfig().height;
        cinfo.input_components = GetConfig().num_components;

        if (cinfo.input_components == 3) {
            cinfo.in_color_space = JCS_RGB;
        }	else if (cinfo.input_components == 1) {
            cinfo.in_color_space = JCS_GRAYSCALE;
        } else {
            throw std::runtime_error("Bad image: number of channels not 1 or 3 but it's " +
                                    std::to_string(cinfo.input_components));
        }

        jpeg_set_defaults(&cinfo);

        jpeg_start_compress(&cinfo, true);

        int row_stride =  GetConfig().width * cinfo.input_components;

        auto image_buffer = GetDataPtr();
        while (cinfo.next_scanline < cinfo.image_height) {
            // std::cerr << cinfo.next_scanline << std::endl;
            row_pointer[0] = image_buffer + cinfo.next_scanline * row_stride;
            (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
        }

        jpeg_finish_compress(&cinfo);
        jpeg_destroy_compress(&cinfo);
        fclose(outfile);
    }

public:

    void SetSize(size_t width, size_t height) {
        config_.width = width;
        config_.height = height;
        data_.assign(height * width, RGB{});
    }

    void SetData(const std::vector<RGB>& data) {
        data_ = data;
    }

    void SetConfig(JpegConfig config) {
        config_ = config;
    }

public:

    JpegConfig GetConfig() const {
        return config_;
    }

    size_t GetWidth() const {
        return config_.width;
    }

    size_t GetHeight() const {
        return config_.height;
    }

    void SetPixel(int y, int x, const RGB& pixel) {
        data_[y * config_.width + x] = pixel;
    }

    RGB GetPixel(int y, int x) const {
        return data_[y * config_.width + x];
    }

    RGB& GetPixel(int y, int x) {
        return data_[y * config_.width + x];
    }

    unsigned char* GetDataPtr() const {
        return (unsigned char*)(data_.data());
    }

    auto GetData() const {
        return data_;
    }

private:
    JpegConfig config_;

    std::vector<RGB> data_;
    std::string comment_;
};

JpegImage ReflectImage(const JpegImage& image) {
    JpegImage result;

    auto image_data = image.GetData();

    for (size_t i = 0; i < image_data.size(); i += image.GetWidth()) {
        std::reverse(std::next(image_data.begin(), i),
                    std::next(image_data.begin(), i + image.GetWidth()));
    }

    result.SetConfig(image.GetConfig());
    result.SetData(std::move(image_data));

    return result;
}

}

#endif