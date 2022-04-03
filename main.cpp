#include <string>
#include "collector.h"
#include "palette.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <fmt/core.h>

Collector colors;

FIBITMAP* load_image(const char* filename)
{
    FREE_IMAGE_FORMAT format = FreeImage_GetFileType(filename);

    FIBITMAP* bitmap = FreeImage_Load(format, filename);

    FIBITMAP* normalized = FreeImage_ConvertTo32Bits(bitmap);
    FreeImage_Unload(bitmap);

    return normalized;
}

FIBITMAP* quantize(FIBITMAP* bitmap)
{
    unsigned width = FreeImage_GetWidth(bitmap);
    unsigned height = FreeImage_GetHeight(bitmap);
    FIBITMAP* result = FreeImage_Allocate(width, height, FreeImage_GetBPP(bitmap));
    RGBQUAD in;
    int index = 0;

    for(unsigned y = 0; y < height; ++y)
    {
        for(unsigned x = 0; x < width; ++x)
        {
            FreeImage_GetPixelColor(bitmap, x, y, &in);
            colors.add_color(convert(in));
            if(is_transparent(in))
                index = 7;
            else
                index = convert_color(convert(in), x, y);
            FreeImage_SetPixelColor(result, x, y, &inky_palette[index]);
        }
    }
    FreeImage_Unload(bitmap);
    return result;
}

FIBITMAP* resize(FIBITMAP* bitmap, size_t target_width, size_t target_height)
{
    FIBITMAP* result = FreeImage_Rescale(bitmap, target_width, target_height, FILTER_BILINEAR);
    FreeImage_Unload(bitmap);
    return result;
}

void save_image(const char* filename, FIBITMAP* bitmap)
{
    FIBITMAP* normalized = FreeImage_ConvertTo32Bits(bitmap);
    FreeImage_Save(FIF_PNG, normalized, filename);
    FreeImage_Unload(normalized);
}

void convert_weather_icon(const char* name)
{
    std::string filename(name);
    std::string source_folder("./images/");
    std::string dest_folder("./out/");

    auto source = source_folder + filename;
    auto dest = dest_folder + filename;

    auto img = load_image(source.c_str());
    img = resize(img, 112, 112);
    img = quantize(img);
    save_image(dest.c_str(), img);
    FreeImage_Unload(img);
}

void save_palette()
{
    FIBITMAP* image = FreeImage_Allocate(PALETTE_SIZE, 1, 24);
    for(int i = 0; i < PALETTE_SIZE; ++i)
    {
        FreeImage_SetPixelColor(image, i, 0, &inky_palette[i]);
    }
    save_image("./out/palette.png", image);
    FreeImage_Unload(image);
}

void convert_to_seven_color()
{
    /*
    for each y from top to bottom do
        for each x from left to right do
            oldpixel := pixels[x][y]
            newpixel := find_closest_palette_color(oldpixel)
            pixels[x][y] := newpixel
            quant_error := oldpixel - newpixel
            pixels[x + 1][y    ] := pixels[x + 1][y    ] + quant_error × 7 / 16
            pixels[x - 1][y + 1] := pixels[x - 1][y + 1] + quant_error × 3 / 16
            pixels[x    ][y + 1] := pixels[x    ][y + 1] + quant_error × 5 / 16
            pixels[x + 1][y + 1] := pixels[x + 1][y + 1] + quant_error × 1 / 16
    */
}

int main(int argc, char** argv)
{
    init_palette();
    FT_Library library;
    auto error = FT_Init_FreeType(&library);
    if(error)
    {
        return -1;
    }
    FreeImage_Initialise();

    for(int i = 0; i < 48; ++i)
    {
        std::string filename = fmt::format("{:02d}.png", i);
        convert_weather_icon(filename.c_str());
    }
    convert_weather_icon("na.png");

    FreeImage_DeInitialise();

    //colors.report_palette();

    return 0;
}
