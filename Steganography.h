#pragma once
#include <string>

namespace Steganography {

    // Function to encrypt a message into an image file
    bool encryptMessage(const std::string& filename, const std::string& message);

    // Function to extract a message from an image file
    std::string extractMessage(const std::string& filename);

    // Function to check if a message can be encrypted into an image file
    bool canEncryptMessage(const std::string& filename, const std::string& message);

} // namespace Steganography


