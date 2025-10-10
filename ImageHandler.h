#pragma once
#include <string>
#include <vector>

namespace ImageHandler {

    // Function to print information about the image file
    void printFileInfo(const std::string& filename);

    // Function to read image data from a file
    bool readImage(const std::string& filename, std::vector<char>& data, int& width, int& height, int& channels, int& maxVal);

    // Function to write image data to a file
    bool writeImage(const std::string& filename, const std::vector<char>& data, int width, int height, int channels, int maxVal);

} // namespace ImageHandler


