#include "Steganography.h"
#include "ImageHandler.h"
#include <vector>
#include <bitset>
#include <fmt/core.h>

namespace Steganography {

const std::string marker = "MSG:"; // Marker to indicate message presence so I know what to look for

// Function to encrypt a message into an image file
bool encryptMessage(const std::string& filename, const std::string& message) {
    int width, height, channels, maxVal;
    std::vector<char> data;
    if (!ImageHandler::readImage(filename, data, width, height, channels, maxVal)) {
        fmt::println("Error reading image for encrypting.");
        return false;
    }

    // Convert marker and message to a binary string,
    std::string binaryString;
    // for each character in marker + message, append binaryString with c that is moved i bits to right
    // masking with one to gett a bool value and at last ternary operator (thanks professor Werner) to append if true -> 1, else -> 0
    for (char c : marker + message) {
        for (int i = 7; i >= 0; --i) {
            binaryString.push_back(((c >> i) & 1) ? '1' : '0');
        }
    }
    binaryString.append(8, '0'); // Append null character to denote end of message

    // Check if the message can be encrypted, dosen't get more simple then that
    if (binaryString.length() > data.size()) {
        fmt::println("Insufficient space in image to encrypt message.");
        return false;
    }

    // Put the binary string into the image data
    // for loop going through data, data at i = data at i masked with 0xFE = 254
    // aka data without least significan bit that is then masked with i-th character binaryString
    // if i-th char is 1 -> mask data with 1, else -> mask with 0
    for (int i = 0; i < binaryString.length(); ++i) {
        data[i] = (data[i] & 0xFE) | (binaryString[i] == '1' ? 1 : 0); // Modify only the least significan bit
    }

    // Write the modified image data back to the file
    if (!ImageHandler::writeImage(filename, data, width, height, channels, maxVal)) {
        fmt::println("Error writing encrypted image.");
        return false;
    }

    return true;
}

// Function to extract a message from an image file
std::string extractMessage(const std::string& filename) {
    int width, height, channels, maxVal;
    std::vector<char> data;
    if (!ImageHandler::readImage(filename, data, width, height, channels, maxVal)) {
        fmt::println("Failed to read image for message extraction.");
        return "";
    }

    // Extract the binary string from the image data, mask byte with on to get bool value
    std::string binaryString;
    for (char byte : data) {
        binaryString.push_back((byte & 1) ? '1' : '0');
    }

    // Convert the binary string back to text
    // for loop iterating by 8 (number of bits in byte), bitset used to store 8 bits starting at i
    // then when it finishes converting bits into char that holds 1 byte, it adds character to 'extracted' string
    std::string extracted;
    for (int i = 0; i < binaryString.length(); i += 8) {
        std::bitset<8> bits(binaryString.substr(i, 8));
        char c = 0;
        for (int j = 0; j < 8; ++j) {
            if (bits[j]) {
                c += (1 << j);
            }
        }
        if (c == '\0') break;  // Stop at the first null character
        extracted.push_back(c);
    }

    // Finding marker in extracted returning message i
    if (extracted.find(marker) == 0) {  // Marker should be at the start of extracted, find returns index of first char of marker
        return extracted.substr(marker.size()); // If that's correct, return sub string of marker size
    }

    return ""; // No valid message found
}

// Function to check if a message can be encrypted in an image file
bool canEncryptMessage(const std::string& filename, const std::string& message) {
    int width, height, channels, maxVal;
    std::vector<char> data;
    if (!ImageHandler::readImage(filename, data, width, height, channels, maxVal)) {
        fmt::println("Error reading image for capacity check.");
        return false;
    }

    // Calculate the number of bits needed to encrypt the message
    int neededBits = (marker.size() + message.size()) * 8 + 8; // 8 extra bits for the null terminator https://en.wikipedia.org/wiki/Null-terminated_string
    return neededBits <= data.size() * 8;
}

} // namespace Steganography
