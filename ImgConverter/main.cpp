#include <img_lib.h>
#include <jpeg_image.h>
#include <ppm_image.h>
#include <bmp_image.h>

#include <string>
#include <filesystem>
#include <string_view>
#include <iostream>

using namespace std;

namespace image_format_interface{

    enum class Format{JPEG,PPM,BMP,UNKNOWN};

    Format GetFormatByExtension(const img_lib::Path& input_file) {
        const string ext = input_file.extension().string();
        if (ext == ".jpg"sv || ext == ".jpeg"sv) {
            return Format::JPEG;
        }
        if (ext == ".ppm"sv) {
            return Format::PPM;
        }
        if (ext == ".bmp"sv) {
            return Format::BMP;
        }

        return Format::UNKNOWN;
    }


    class ImageFormatInterface {
    public:
        virtual bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const = 0;
        virtual img_lib::Image LoadImage(const img_lib::Path& file) const = 0;
        virtual ~ImageFormatInterface() = default;
    };

    class PPMFormat : public ImageFormatInterface{
        public:
        bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override{
           return img_lib::SavePPM(file, image);
        }
        img_lib::Image LoadImage(const img_lib::Path& file) const {
            return img_lib::LoadPPM(file);
        }
    };

    class JPEGFormat : public ImageFormatInterface{
        public:
        bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override{
           return img_lib::SaveJPEG(file, image);
        }
        img_lib::Image LoadImage(const img_lib::Path& file) const {
            return img_lib::LoadJPEG(file);
        }
    };

    class BMPFormat : public ImageFormatInterface{
        public:
        bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override{
           return img_lib::SaveBMP(file, image);
        }
        img_lib::Image LoadImage(const img_lib::Path& file) const {
            return img_lib::LoadBMP(file);
        }
    };


    ImageFormatInterface* GetFormatInterface(const img_lib::Path& path){
        Format format;
        format = GetFormatByExtension(path);
        switch (format){
        case Format::PPM: return new PPMFormat();
        case Format::JPEG: return new JPEGFormat();
        case Format::BMP: return new BMPFormat();
        default: return nullptr;
        }

    }
}

using namespace image_format_interface;

int main(int argc, const char** argv) {
    if (argc != 3) {
        cerr << "Usage: "sv << argv[0] << " <in_file> <out_file>"sv << endl;
        return 1;
    }

    img_lib::Path in_path = argv[1];
    img_lib::Path out_path = argv[2];

    ImageFormatInterface* in_path_format = GetFormatInterface(in_path);
    ImageFormatInterface* out_path_format = GetFormatInterface(out_path);

    if(in_path_format == nullptr){
        cerr << "Unknown format of the input file"sv << endl;
        return 2;
    }

    if(out_path_format == nullptr){
        cerr << "Unknown format of the output file"sv << endl;
        return 3;
    }

    img_lib::Image image = in_path_format->LoadImage(in_path);
    if (!image) {
        cerr << "Loading failed"sv << endl;
        return 4;
    }

    if (!out_path_format->SaveImage(out_path, image)) {
        cerr << "Saving failed"sv << endl;
        return 5;
    }

    delete in_path_format;
    delete out_path_format;

    cout << "Successfully converted"sv << endl;
}
