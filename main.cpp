#include <fstream>
#include <iostream>
#include <string>
#include "ppm.h"

using namespace std;

int main(int argc, char *argv[]) {
  // check argc return error code if cot complete
  if (argc != 4) {
    cerr << "Invalid arguments." << endl;
    cerr << "Please ensure your command contains the following: " << endl;
    cerr << "The program to run, the image file, the output file name and "
            "type, and the "
            "convulve function." << endl;
    cerr << "Example: ./cs257final sled_dog_ascii.ppm out.ppm edges" << endl;
    return 1;
  }
  // make kernels here for convolve
  double emboss[3][3] = {{-2, -1, 0}, {-1, 1, 1}, {0, 1, 2}};
  double identity[3][3] = {{0, 0, 0}, {0, 1, 0}, {0, 0, 0}};
  double edges[3][3] = {{-1, -1, -1}, {-1, 8, -1}, {-1, -1, -1}};
  double sharpen[3][3] = {{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}};
  double blur[3][3] = {{.0625, .125, .0625}, {.125, .25, .125},
                       {.0625, .125, .0625}};

  // open file
  ifstream inFS;
  inFS.open(argv[1]);
  string pictureData;

  if (!inFS.is_open()) {  // if file is not open output error code
    cerr << "Error: File did not open." << endl;
    cerr << "Please make sure this is a .ppm binary or .ppm ascii file."
         << endl;
    return 1;
  }

  // if file is open, get magic num
  MagicNumber num = ReadMagic(inFS);
  if (num == MagicNumber::kRawbits) {
    // call parse rawbits
    Image binaryStruct = ParseRawbitsImage(inFS);
    // call convolve rawbits
    ofstream outfile;
    // Open the file for writing
    outfile.open(argv[2]);
    string k = argv[3];  // call rest for kernel type and then output file
    if (k.compare("identity") == 0) {
      Image transformedImg = Convolve(binaryStruct, identity);
      WriteRawbitsImage(transformedImg, outfile);
      // free memory
      FreeImage(transformedImg);
    } else if (k.compare("edges") == 0) {
      Image transformedImg = Convolve(binaryStruct, edges);
      WriteRawbitsImage(transformedImg, outfile);
      FreeImage(transformedImg);
    } else if (k.compare("emboss") == 0) {
      Image transformedImg = Convolve(binaryStruct, emboss);
      WriteRawbitsImage(transformedImg, outfile);
      FreeImage(transformedImg);
    } else if (k.compare("sharpen") == 0) {
      Image transformedImg = Convolve(binaryStruct, sharpen);
      WriteRawbitsImage(transformedImg, outfile);
      FreeImage(transformedImg);
    } else if (k.compare("blur") == 0) {
      Image transformedImg = Convolve(binaryStruct, blur);
      WriteRawbitsImage(transformedImg, outfile);
      FreeImage(transformedImg);
    } else {
      // if incorrect convolve type
      cerr << "ERROR: Program does not support convolve type." << endl;
      cerr << "Please check that the correct convolve opperation was called"
           << endl;
      cerr << "Accepted types are: 'identity', 'edges', 'emboss', 'sharpen', "
              "and 'blur'." << endl;
    }
    // free memory
    FreeImage(binaryStruct);

  } else if (num == MagicNumber::kAscii) {
    // call parse ascii image
    Image asciiStruct = ParseAsciiImage(inFS);
    ofstream outfile;
    outfile.open(argv[2]);
    // call convolve for given kernerl type
    string k = argv[3];
    if (k.compare("identity") == 0) {
      Image transformedImg = Convolve(asciiStruct, identity);
      WriteAsciiImage(transformedImg, outfile);
      FreeImage(transformedImg);
    } else if (k.compare("edges") == 0) {
      Image transformedImg = Convolve(asciiStruct, edges);
      WriteAsciiImage(transformedImg, outfile);
      FreeImage(transformedImg);
    } else if (k.compare("emboss") == 0) {
      Image transformedImg = Convolve(asciiStruct, emboss);
      WriteAsciiImage(transformedImg, outfile);
      FreeImage(transformedImg);
    } else if (k.compare("sharpen") == 0) {
      Image transformedImg = Convolve(asciiStruct, sharpen);
      WriteAsciiImage(transformedImg, outfile);
      FreeImage(transformedImg);
    } else if (k.compare("blur") == 0) {
      Image transformedImg = Convolve(asciiStruct, blur);
      WriteAsciiImage(transformedImg, outfile);
      FreeImage(transformedImg);
    } else {
      cerr << "ERROR: Program does not support convolve type." << endl;
      cerr << "Please check that the correct convolve opperation was called"
           << endl;
      cerr << "Accepted types are: 'identity', 'edges', 'emboss', 'sharpen', "
              "and 'blur'." << endl;
    }
    FreeImage(asciiStruct);

  } else if (num == MagicNumber::kFail) {
    // output error for new file
    cerr << "ERROR: This program does not support that file type." << endl;
    cerr << "Please choose a different file." << endl;
    cerr << "Acceptable file types are 'P3' ascii or 'P6' binary image."
         << endl;
    return 1;
  }

  // close file after all data is gathered
  inFS.close();

  cout << "Image convolution complete. Please see out file for new image."
       << endl;
  return EXIT_SUCCESS;
}
