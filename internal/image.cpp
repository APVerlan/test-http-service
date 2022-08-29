#include <image.h>
#include <jpeglib.h>

#include <cstdio>
#include <stdexcept>

namespace jpeg_image {

JpegImage JpegImage::ReadImageFromJpegFile(const std::string& filename) {
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

    Image result(cinfo.output_width, cinfo.output_height);
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
                                        std::to_string(cinfo.input_components));
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

void JpegImage::WriteImageToJpegFile(const std::string& filename, const Image& ) {
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    FILE * outfile;
    JSAMPROW row_pointer[1];

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    if ((outfile = fopen(filename, "wb")) == NULL) {
        throw std::runtime_error("Can't open " + filename);
    }

    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = image_width;
    cinfo.image_height = image_height;
    cinfo.input_components = image.GetConfig().num_components;
    
    if (cinfo.input_components == 3) {
        cinfo.in_color_space = JCS_RGB;
    }	else if (cinfo.input_components == 1) {
        cinfo.in_color_space = JCS_GRAYSCALE;
    } else {
        throw std::runtime_error("Bad image: number of channels not 1 or 3 but it's " +
                                std::to_string(cinfo.input_components));
    }

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, .GetConfig().quality, true);

    jpeg_start_compress(&cinfo, true);

    int row_stride = image_width * cinfo.input_components;
    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = &image_buffer[cinfo.next_scanline * row_stride];
        (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    fclose(outfile);
}


}