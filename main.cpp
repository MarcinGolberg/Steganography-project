#include <filesystem>
#include "ImageHandler.hpp"
#include "Steganography.hpp"
#include <fmt/core.h>

// The video has different .exe name then this, becouse I read about name requiraments later. Code it the same

namespace fs = std::filesystem;

// Function to print help information for the user
void printHelp() {
    fmt::println("Usage:");
    fmt::println("-i, -info    [file]           Display information about the file (BMP and PPM only).");
    fmt::println("-e, -encrypt [file] [message] Encrypt a message into the file (BMP and PPM only).");
    fmt::println("-d, -decrypt [file]           Extract a message from the file (BMP and PPM only).");
    fmt::println("-c, -check   [file] [message] Check if a message can be encrypted (BMP and PPM only).");
    fmt::println("-h, -help                     Show help information.");
    fmt::println("IMPORTANT: IF THERE IS A SPACE IN FILE PATH, PUT IT IN QUOTES \"\"");
}

// Main function to handle command-line arguments and execute corresponding actions
// argc -> number of strings in argv, argv -> strings themself
int main(int argc, char *argv[]) {
    if (argc < 2) {
        // If no arguments are provided, print help information
        printHelp();
        return 1;
    }

    std::string command = argv[1];
    if (command == "-h" || command == "-help") {
        // If the help command is provided, print help information
        printHelp();
        return 0;
    } else if (argc >= 3) {
        std::string filename = argv[2];
        // Checking if the file exists and is either a BMP or PPM file
        if (!fs::exists(filename) || (!filename.ends_with(".bmp") && !filename.ends_with(".ppm"))) {
            fmt::println("Unsupported file format. Only .bmp and .ppm files are supported.");
            return 1;
        }
        // Process the command and execute the corresponding function
        if ((command == "-i" || command == "-info") && argc == 3) {
            // Display file information
            ImageHandler::printFileInfo(filename);
        } else if ((command == "-e" || command == "-encrypt") && argc == 4) {
            // Encrypt a message into the file
            std::string message = argv[3];
            if (Steganography::encryptMessage(filename, message)) {
                fmt::println("Message successfully encrypted.");
            } else {
                fmt::println("Failed to encrypt message.");
            }
        } else if ((command == "-d" || command == "-decrypt") && argc == 3) {
            // Extract a message from the file
            try {
                std::string message = Steganography::extractMessage(filename);
                fmt::println("Extracted message: '{}'", message);
            } catch (const std::exception &e) {
                fmt::println("Error: {}", e.what());
            }
        } else if ((command == "-c" || command == "-check") && argc == 4) {
            // Check if a message can be encrypted" in the file
            std::string message = argv[3];
            if (Steganography::canEncryptMessage(filename, message)) {
                fmt::println("The message can be encrypted.");
            } else {
                fmt::println("The message cannot be encrypted due to size constraints.");
            }
        } else {
            // If the command is invalid, print help information
            fmt::println("Invalid usage.");
            printHelp();
        }
    } else {
        // If the command is invalid, print help information
        fmt::println("Invalid usage.");
        printHelp();
    }

    return 0;
}
