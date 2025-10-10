#include "ImageHandler.hpp"
#include <fstream>
#include <sstream>
#include <fmt/core.h>

namespace ImageHandler {

    //Function to print information about the image file
    void printFileInfo(const std::string &filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            fmt::print("Failed to open file for reading.\n");
            return;
        }

        //Read the entire file content into a vector
        std::vector<char> fileContent;
        char ch;
        while (file.get(ch)) {
            fileContent.push_back(ch);
        }
        int fileSize = fileContent.size();

        if (filename.ends_with(".bmp")) {
            //BMP has a header that contains metadata about the image, width 18-21, height 22-25, bits per pixel 28-29
            //BMP file header + DIB header = 54, that's why header end is moved by 54 bytes
            //https://en.wikipedia.org/wiki/BMP_file_format
            if (fileSize < 54) {
                fmt::print("File seems too small to be a valid BMP.\n");
                return;
            }
            std::vector<char> header(fileContent.begin(), fileContent.begin() + 54);
            //reinterpret_cast is a pointer, so it reads 4 bytes (due to width being an int), in this case: &header[18], &header[19], &header[20], &header[21]
            //<int*> is so 32 bits of reference to header store in int
            int width = *reinterpret_cast<int *>(&header[18]);
            int height = *reinterpret_cast<int *>(&header[22]);
            //255 = 0b11111111 = 8 bits
            //255 to make sure that only 8 bits of header is used
            //then we shift header[29] by 8 bits so we effectively get a 16-bit value
            //bitwise operator OR used to combine them
            int bitsPerPixel = (header[28] & 255) | ((header[29] & 255) << 8);

            //Print BMP file information
            fmt::print("File:           {}\n", filename);
            fmt::print("Size:           {} bytes\n", fileSize);
            fmt::print("Dimensions:     {}x{}\n", width, height);
            fmt::print("Bits per pixel: {}\n", bitsPerPixel);
        } else if (filename.ends_with(".ppm")) {
            //Read PPM header
            std::istringstream fileStream(std::string(fileContent.begin(), fileContent.end()));
            std::string line;
            std::getline(fileStream, line);
            //First line of PPM file should be P6
            if (line != "P6") {
                fmt::print("Invalid PPM file.\n");
                return;
            }
            //Skipping all comments in PPM header
            while (fileStream.peek() == '#') {
                std::getline(fileStream, line);
            }

            //After comments in PPM file there are values for: width, height and maximum color value
            int width, height, maxVal;
            fileStream >> width >> height >> maxVal;

            //Print PPM file information
            fmt::print("File:            {}\n", filename);
            fmt::print("Size:            {} bytes\n", fileSize);
            fmt::print("Dimensions:      {}x{}\n", width, height);
            fmt::print("Max color value: {}\n", maxVal);
        }
    }

    //Function to read image data from a file
    bool readImage(const std::string &filename, std::vector<char> &data, int &width, int &height, int &channels,
                   int &maxVal) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            fmt::print("Failed to open file for reading.\n");
            return false;
        }

        if (filename.ends_with(".bmp")) {
            //Same explanations as in printFileInfo
            //Read BMP header
            std::vector<char> header(54);
            file.read(header.data(), header.size());
            if (!file) {
                fmt::print("Failed to read BMP header.\n");
                return false;
            }

            //Extract image metadata from header
            width = *reinterpret_cast<int *>(&header[18]);
            height = *reinterpret_cast<int *>(&header[22]);
            int bitsPerPixel = (header[28] & 255) | ((header[29] & 255) << 8);
            //Divide by 8 to convert bits per pixel to bytes (color channels)
            channels = bitsPerPixel / 8;
            maxVal = 255; //BMP default max color value

            //pixelDataSize in total number of bytes needed to store pixel data of image
            //NOTE: copy the pixel data starting at the pixel data offset instead of assuming 54-byte header
            int pixelDataOffset = *reinterpret_cast<int*>(&header[10]);
            if (pixelDataOffset < 14) {
                fmt::print("Invalid BMP pixel data offset.\n");
                return false;
            }
            //resize -> shrinks or expands array to size in brackets,
            int pixelDataSize = width * height * channels;
            data.resize(pixelDataSize);
            //.data() returns a pointer to the first element of the vector's internal array
            file.seekg(pixelDataOffset, std::ios::beg);
            file.read(data.data(), pixelDataSize);

            file.close();
            return true;
        } else if (filename.ends_with(".ppm")) {
            std::string line;
            std::getline(file, line);
            if (line != "P6") {
                fmt::print("Invalid PPM file format.\n");
                return false;
            }

            while (file.peek() == '#') {
                std::getline(file, line);
            }

            file >> width >> height >> maxVal;
            channels = 3; //PPM format uses 3 channels (RGB)

            file.get(); //Skip the single whitespace character after max color value

            int pixelDataSize = width * height * channels;
            data.resize(pixelDataSize);
            file.read(data.data(), pixelDataSize);

            file.close();
            return true;
        }

        return false;
    }

    //Function to write image data
    bool writeImage(const std::string &filename, const std::vector<char> &data, int width, int height, int channels,
                    int maxVal) {
        std::ofstream file(filename, std::ios::binary);
        if (!file) {
            fmt::print("Failed to open file for writing.\n");
            return false;
        }

        if (filename.ends_with(".bmp")) {
            //Create and write BMP header
            //All of this why can be found in wikipedia linked above
            std::vector<char> header(54, 0); //Initialize header with zeroes
            header[0] = 'B';
            header[1] = 'M';
            int fileSize = 54 + data.size();
            *reinterpret_cast<int *>(&header[2]) = fileSize;
            *reinterpret_cast<int *>(&header[18]) = width;
            *reinterpret_cast<int *>(&header[22]) = height;
            *reinterpret_cast<int *>(&header[28]) = channels * 8;
            header[10] = 54; //Pixel data offset
            header[14] = 40; //Header size
            header[26] = 1; //Number of planes

            //Write header and pixel data to the file
            file.write(header.data(), header.size());
            file.write(data.data(), data.size());

            file.close();
            return true;
            //For ppm files
        } else if (filename.ends_with(".ppm")) {
            //Write PPM header
            file << "P6\n" << width << " " << height << "\n" << maxVal << "\n";
            file.write(data.data(), data.size());

            file.close();
            return true;
        }

        return false;
    }

    //Function to write image data by COPYING the BMP header from an input BMP.
    //This is for your encryption path: copy header from the original, then paste encrypted data right after it.
    bool writeImage(const std::string &outFilename,
                    const std::string &sourceBmpForHeader,
                    const std::vector<char> &data, int /*width*/, int /*height*/, int /*channels*/,
                    int /*maxVal*/) {
        std::ofstream out(outFilename, std::ios::binary);
        if (!out) {
            fmt::print("Failed to open file for writing.\n");
            return false;
        }

        if (outFilename.ends_with(".bmp")) {
            //Instead of constructing a new header, copy it from the initial image.
            //This also respects BMPs with headers larger than 54 bytes, by using the pixel data offset.
            std::ifstream src(sourceBmpForHeader, std::ios::binary);
            if (!src) {
                fmt::print("Failed to open source BMP for header copy. Falling back to default header.\n");
                out.close();
                //Fallback keeps behavior if caller passes a non-BMP or missing file
                return writeImage(outFilename, data, 0, 0, 0, 255);
            }

            //Read at least the first 14 bytes to get the pixel data offset at bytes 10..13.
            std::vector<char> fileHeader(14);
            src.read(fileHeader.data(), fileHeader.size());
            if (!src) {
                fmt::print("Failed to read source BMP file header. Falling back to default header.\n");
                src.close();
                out.close();
                return writeImage(outFilename, data, 0, 0, 0, 255);
            }

            //pixel data offset is at 10..13 from the start
            int pixelDataOffset = *reinterpret_cast<int *>(&fileHeader[10]);
            if (pixelDataOffset <= 0) {
                fmt::print("Invalid pixel data offset in source BMP. Falling back to default header.\n");
                src.close();
                out.close();
                return writeImage(outFilename, data, 0, 0, 0, 255);
            }

            //Rewind and copy exactly 'pixelDataOffset' bytes as the header (works for headers > 54 too)
            src.seekg(0, std::ios::beg);
            std::vector<char> header(pixelDataOffset);
            src.read(header.data(), header.size());
            if (!src) {
                fmt::print("Failed to copy full header from source BMP. Falling back to default header.\n");
                src.close();
                out.close();
                return writeImage(outFilename, data, 0, 0, 0, 255);
            }

            //Update the file size field in the copied header so the output is a valid BMP.
            //(This doesn’t remove any of your comments; it just keeps the header consistent.)
            //Bytes 2..5 = total file size (little-endian).
            int newFileSize = pixelDataOffset + static_cast<int>(data.size());
            if (header.size() >= 6) {
                *reinterpret_cast<int *>(&header[2]) = newFileSize;
            }

            //Write header and then encrypted pixel data (already prepared in `data`)
            out.write(header.data(), header.size());
            out.write(data.data(), data.size());

            src.close();
            out.close();
            return true;
        } else if (outFilename.ends_with(".ppm")) {
            //Write PPM header (copying is irrelevant for PPM; format is textual)
            out << "P6\n";
            //width/height/maxVal are not needed from the source here because PPM is not the target of this path
            out << "0 0\n255\n";
            out.write(data.data(), data.size());
            out.close();
            return true;
        }

        return false;
    }

} //namespace ImageHandler
