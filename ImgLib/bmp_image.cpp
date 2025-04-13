#include "bmp_image.h"
#include "pack_defines.h"

#include <array>
#include <fstream>
#include <string_view>

using namespace std;

namespace img_lib
{

PACKED_STRUCT_BEGIN BitmapFileHeader
{
    uint8_t signature[2]
    { 'B', 'M' };
    uint32_t size;
    int32_t reserved = 0;
    uint32_t padding = 54;
}
PACKED_STRUCT_END

PACKED_STRUCT_BEGIN BitmapInfoHeader
{
    uint32_t header_size = 40;
    int32_t width = 0;
    int32_t height = 0;
    uint16_t number_of_planes = 1;
    uint16_t number_of_bits_per_pixel = 24;
    uint32_t compression = 0;
    uint32_t the_number_of_bytes_in_data;
    int32_t h_res = 11811;
    int32_t v_res = 11811;
    int32_t colors = 0;
    int32_t number_of_significant_colors = 0x1000000;
}
PACKED_STRUCT_END

static const short the_number_is_rounded_off_as_follows = 4;
static const short number_of_colors = 3;
static const short number_to_round_up = 3;

static int GetBMPStride(int w)
{
    return the_number_is_rounded_off_as_follows * ((w * number_of_colors + number_to_round_up) / the_number_is_rounded_off_as_follows);
}

// напишите эту функцию
bool SaveBMP(const Path &file, const Image &image)
{
    ofstream out(file, ios::binary);
    BitmapFileHeader file_header;
    BitmapInfoHeader info_header;

    int h = image.GetHeight();
    int w = image.GetWidth();

    int padding = GetBMPStride(w);
    std::vector<char> buff(padding);

    info_header.width = w;
    info_header.height = h;
    file_header.size = file_header.padding + padding * h;
    info_header.the_number_of_bytes_in_data = padding * h;

    out.write(reinterpret_cast<char*>(&file_header), sizeof(BitmapFileHeader));
    out.write(reinterpret_cast<char*>(&info_header), sizeof(BitmapInfoHeader));

    for (int y = h - 1; y >= 0; --y)
    {
        const Color *line = image.GetLine(y);
        for (int x = 0; x < w; ++x)
        {
            buff[x * 3 + 0] = static_cast<char>(line[x].b);
            buff[x * 3 + 1] = static_cast<char>(line[x].g);
            buff[x * 3 + 2] = static_cast<char>(line[x].r);
        }
        out.write(buff.data(), padding);
    }

    return out.good();
}

bool FileHeadingsAreSuitable(BitmapFileHeader& file_header, BitmapInfoHeader& info_header){
    return file_header.signature[0] == 'B'
            && file_header.signature[1] == 'M'
            && file_header.padding == 54
            && info_header.header_size == 40
            && info_header.number_of_planes == 1
            && info_header.number_of_bits_per_pixel == 24
            && info_header.compression == 0
            && info_header.h_res == 11811
            && info_header.v_res == 11811
            && info_header.colors == 0
            && info_header.number_of_significant_colors == 0x1000000;
}

// напишите эту функцию
Image LoadBMP(const Path &file)
{
    ifstream ifs(file, ios::binary);
    BitmapFileHeader file_header;
    BitmapInfoHeader info_header;

    ifs.read(reinterpret_cast<char*>(&file_header), sizeof(BitmapFileHeader));

    if (ifs.fail()) {
           return {};
   }

    ifs.read(reinterpret_cast<char*>(&info_header), sizeof(BitmapInfoHeader));

    if (ifs.fail()) {
           return {};
   }

    if (!FileHeadingsAreSuitable(file_header, info_header))
          return{};

        const int w = info_header.width;
        const int h = info_header.height;
        const int padding = GetBMPStride(w);
        std::vector<char> buff(padding);
        Image image(w, h, Color::Black());

        for (int y = h - 1; y >= 0; --y)
        {
            Color* line = image.GetLine(y);
            ifs.read(buff.data(), padding);

            for (int x = 0; x < w; ++x)
            {
                line[x].b = static_cast<byte>(buff[x * 3 + 0]);
                line[x].g = static_cast<byte>(buff[x * 3 + 1]);
                line[x].r = static_cast<byte>(buff[x * 3 + 2]);
            }
        }

        return image;
    }
}
