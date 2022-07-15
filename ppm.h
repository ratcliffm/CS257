#ifndef PPM_H_
#define PPM_H_

#include <fstream>
#include <string>
#include <vector>

enum class MagicNumber { kRawbits, kAscii, kFail };

// These are ints instead of chars to make it easier to scale
// the pixel values without hitting the bounds.
struct Pixel {
  int red;
  int green;
  int blue;
};

struct Image {
  MagicNumber magic;                  // PPM format
  std::vector<std::string> comments;  // All comments
  int width;                          // image width
  int height;                         // image height
  int max_color;                      // maximum color value
  Pixel **pixels;                     // Acts as 2D array of pixels
};

// Reads the magic number of PPM file.
MagicNumber ReadMagic(std::ifstream &fppm);

// Parse a rawbits PPM file.
Image ParseRawbitsImage(std::ifstream &fppm);

// Parses an ASCII PPM file.
Image ParseAsciiImage(std::ifstream &fppm);

// Writes image struct to file in ASCII format.
void WriteAsciiImage(const Image &img, std::ofstream &fppm);

// Writes image struct to file in rawbits format.
void WriteRawbitsImage(const Image &img, std::ofstream &fppm);

// Transforms the image in place.
Image Convolve(const Image &img, double[3][3]);

// Deallocates memory used by image.
void FreeImage(const Image &img);

#endif  // PPM_H_
