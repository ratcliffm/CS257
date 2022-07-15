#include "ppm.h"

#include <sstream>
#include <iostream>
#include <string>
#include <cmath>
#include <vector>

using namespace std;

// READ MAGIC FUNCTION
MagicNumber ReadMagic(ifstream &fppm) {  // magic num function working
  MagicNumber magic;
  string line;
  // start from beginning of file and get magic number
  fppm.seekg(0, ios::beg);
  getline(fppm, line);
  // return coresponding file type
  if (line.compare("P3") == 0) {
    magic = MagicNumber::kAscii;
    return magic;
  } else if (line.compare("P6") == 0) {
    magic = MagicNumber::kRawbits;
    return magic;
  } else {
    magic = MagicNumber::kFail;
    return magic;
  }
  return magic;
}

// PARSE ASCII IMG FUNCTION
Image ParseAsciiImage(ifstream &fppm) {

  // check magic number
  MagicNumber num = ReadMagic(fppm);
  while (num != MagicNumber::kAscii) {
    cerr << "Wrong function called, please make sure this is a text file"
         << endl;
    break;
  }
  // Create a new Image struct
  Image img;
  // add magic number to struct
  img.magic = num;

  // read comments from file

  string line;
  vector<string> commentFile;
  getline(fppm, line);
  while (line.at(0) == '#') {
    img.comments.push_back(line);
    getline(fppm, line);
  }

  // get image width and height
  int width, height;
  string nums = line;
  stringstream ss(nums);
  ss >> width >> height;
  img.height = height;
  img.width = width;
  if (width > 999 || height > 999) {
    cerr << "Image too large, please choose another file type" << endl;
  }

  // get max color
  getline(fppm, line);
  img.max_color = atoi(line.c_str());

  // parse pixels into new img
  // create new Pixel struct
  Pixel **pixels = new Pixel *[img.height];
  // only get lines 4 and on
  for (int i = 0; i < img.height; ++i) {
    pixels[i] = new Pixel[img.width];
    for (int j = 0; j < img.width; ++j) {
      // assign color to each row of pixels
      int red, blue, green;
      string greenVal, blueVal, input;
      getline(fppm, input);
      red = atoi(input.c_str());
      pixels[i][j].red = red;
      getline(fppm, greenVal);
      green = atoi(greenVal.c_str());
      pixels[i][j].green = green;
      getline(fppm, blueVal);
      blue = atoi(blueVal.c_str());
      pixels[i][j].blue = blue;
    }
  }
  img.pixels = pixels;
  // Return the final image from the function.
  return img;
}

// RAW BIT IMG
Image ParseRawbitsImage(ifstream &fppm) {
  // check magic number to ensure correct function call
  MagicNumber num = ReadMagic(fppm);
  while (num != MagicNumber::kRawbits) {
    cerr << "Wrong function called, please make sure this is a binary file"
         << endl;
    break;
  }  // copy struct and assign new members
  Image img;
  img.magic = num;

  // grab all comments
  string line;
  vector<string> commentFile;
  getline(fppm, line);
  while (line.at(0) == '#') {
    img.comments.push_back(line);
    getline(fppm, line);
  }

  // get image width and height
  int width, height;
  string nums = line;
  stringstream ss(nums);
  ss >> width >> height;
  img.height = height;
  img.width = width;
  if (width > 999 || height > 999) {
    cerr << "Image too large, please choose another file type" << endl;
  }

  getline(fppm, line);  // find max color
  img.max_color = atoi(line.c_str());

  Pixel **pixels = new Pixel *[img.height];
  // only get lines 4 and on
  for (int i = 0; i < img.height; ++i) {
    pixels[i] = new Pixel[img.width];
    for (int j = 0; j < img.width; ++j) {
      char r;
      Pixel p;  // create a pixel P

      fppm.read(&r, 1);  // read one byte assign it to red
      p.red = reinterpret_cast<unsigned char &>(r);
      fppm.read(&r, 1);  // read one byte assign to green
      p.green = reinterpret_cast<unsigned char &>(r);
      fppm.read(&r, 1);  // read one byte assign to blue
      p.blue = reinterpret_cast<unsigned char &>(r);
      pixels[i][j] = p;  // assign the pixel back into the struct
    }
  }
  img.pixels = pixels;
  // return the struct ready for convolve
  return img;
}

// WRITE ASCII IMAGE FUNCTION
void WriteAsciiImage(const Image &img, ofstream &oppm) {
  // Write the the image struct out to a file.
  // write out file type
  oppm << "P3" << endl;
  // create loop that writes out each comment
  for (unsigned int i = 0; i < img.comments.size(); ++i) {
    oppm << img.comments.at(i) << endl;
  }
  // write out image width height and max color
  oppm << img.width << " " << img.height << endl;
  oppm << img.max_color << endl;
  // create a loop to write out each pixel color for each pixel
  for (int i = 0; i < img.height; ++i) {
    for (int j = 0; j < img.width; ++j) {
      oppm << img.pixels[i][j].red << endl;
      oppm << img.pixels[i][j].green << endl;
      oppm << img.pixels[i][j].blue << endl;
    }
  }
  // Close the file after writing all desired data
  oppm.close();
}

// write rawbit function
void WriteRawbitsImage(const Image &img, ofstream &oppm) {
  // Write the the image struct out to a file.
  oppm << "P6" << endl;
  for (unsigned int i = 0; i < img.comments.size(); ++i) {
    oppm << img.comments.at(i) << endl;
  }
  oppm << img.width << " " << img.height << endl;
  oppm << img.max_color << endl;
  // for each pixel assign to color and then turn back to binary
  for (int h = 0; h < img.height; ++h) {
    for (int w = 0; w < img.width; ++w) {
      unsigned char r = img.pixels[h][w].red;
      unsigned char g = img.pixels[h][w].green;
      unsigned char b = img.pixels[h][w].blue;
      oppm.write(reinterpret_cast<char *>(&r), 1);
      oppm.write(reinterpret_cast<char *>(&g), 1);
      oppm.write(reinterpret_cast<char *>(&b), 1);
    }
  }
  // close file
  oppm.close();
}

// PIXEL IN BOUNDS function to support convolve
// function to determine which kernel to multiply the pixel by
Pixel pixel_in_bound(const Image &img, int originalI, int originalJ, int i,
                     int j) {
  // if img not out of bounds return pixel
  if ((i >= 0 && i < img.height) && (j >= 0 && j < img.width)) {
    return img.pixels[i][j];
  } else {  // if out of bounds, return the pixel in corespondance to middle
            // kernel
    return img.pixels[originalI][originalJ];
  }
}

// CONVOLVE FUNCTION
Image Convolve(const Image &img, double kernel[3][3]) {
  // Declare a new empty image struct and copy img struct into it
  Image ret;
  ret.height = img.height;
  ret.width = img.width;
  ret.magic = img.magic;
  ret.max_color = img.max_color;
  ret.comments = img.comments;
  // Copy the pixels.
  ret.pixels = new Pixel *[ret.height];

  // for each pixel, copy the image pixel into the ret
  for (int i = 0; i < ret.height; ++i) {
    ret.pixels[i] = new Pixel[img.width];
    for (int j = 0; j < ret.width; ++j) {
      Pixel pixel;
      ret.pixels[i][j] = pixel;
    }
  }

  // for each pixel height and row, grab the pixel
  for (int i = 0; i < ret.height; ++i) {  // for pixel rows
    for (int j = 0; j < ret.width; ++j) {

      // take each pixel position and return the pixels to mulitply for each
      // kernel
      vector<vector<Pixel>> pixelPos = {
          // for left row
          {pixel_in_bound(img, i, j, i - 1, j - 1),
           pixel_in_bound(img, i, j, i, j - 1),
           pixel_in_bound(img, i, j, i + 1, j - 1)},
          // for middle row
          {pixel_in_bound(img, i, j, i - 1, j), pixel_in_bound(img, i, j, i, j),
           pixel_in_bound(img, i, j, i + 1, j)},
          // for right row
          {pixel_in_bound(img, i, j, i - 1, j + 1),
           pixel_in_bound(img, i, j, i, j + 1),
           pixel_in_bound(img, i, j, i + 1, j + 1)}};

      // assign red values to each pixel in kernel and calculate sum
      double redSum = 0;
      for (int k = 0; k < 3; k++) {
        for (int m = 0; m < 3; m++) {
          redSum += pixelPos[k][m].red * kernel[k][m];
        }
      }
      // if sum is less than zero make zero
      if (redSum < 0) {
        redSum = 0;
        // if sum is greater than 255 make 255
      } else if (redSum > 255) {
        redSum = 255;
      }
      // update single pixel with total sum
      ret.pixels[i][j].red = redSum;

      // assign green values to each pixel in kernel and calculate sum
      double greenSum = 0;
      for (int k = 0; k < 3; k++) {
        for (int m = 0; m < 3; m++) {
          greenSum += pixelPos[k][m].green * kernel[k][m];
        }
      }
      if (greenSum < 0) {
        greenSum = 0;
      } else if (greenSum > 255) {
        greenSum = 255;
      }
      // update pixel with total sum
      ret.pixels[i][j].green = greenSum;

      // assign blue values to each pixel in kernel and calculate sum
      double blueSum = 0;
      for (int k = 0; k < 3; k++) {
        for (int m = 0; m < 3; m++) {
          blueSum += pixelPos[k][m].blue * kernel[k][m];
        }
      }
      if (blueSum < 0) {
        blueSum = 0;
      } else if (blueSum > 255) {
        blueSum = 255;
      }
      // update pixel with total sum
      ret.pixels[i][j].blue = blueSum;
    }
  }
  return ret;
}

// FREE IMAGE FUNCTION
void FreeImage(const Image &img) {
  // Free pixels
  Pixel **pixels = img.pixels;
  for (int i = 0; i < img.height; ++i) {
    delete[] pixels[i];
    pixels[i] = 0;
  }
  delete[] pixels;
}
