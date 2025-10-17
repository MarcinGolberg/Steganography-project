# C++ Steganography Tool

A command-line utility written in C++ for hiding and extracting secret messages within image files using Least Significant Bit (LSB) steganography. The tool supports `.bmp` and `.ppm` image formats.

---

## Features

* **Encrypt**: Hide a text message within a `.bmp` or `.ppm` image file.
* **Decrypt**: Extract a hidden message from a steganographically modified image.
* **Check Capacity**: Verify if an image has enough space to hide a given message before attempting encryption.
* **File Info**: Display metadata for `.bmp` and `.ppm` files, such as dimensions, size, and color depth.
* **Cross-Platform**: Built with CMake for straightforward compilation on various operating systems.

---

## How It Works

The tool employs the **Least Significant Bit (LSB)** steganography technique. It works by altering the last bit of each byte in the image's pixel data to store the bits of the secret message.

1.  **Encryption**: The message is first prefixed with a special marker (`MSG:`) and appended with a null terminator. This entire payload is converted into a binary string. Each bit of this string is then written to the LSB of a corresponding byte in the image's pixel data by using a bitwise AND operation with `0xFE` and a bitwise OR operation with the message bit.
2.  **Decryption**: The process is reversed. The LSB from each byte of the image data is read to reconstruct the binary string. This string is converted back into text. The tool then looks for the `MSG:` marker to locate and return the hidden message.

---

## Prerequisites

To build and run this project, you will need the following installed on your system:
* A C++ compiler that supports the C++20 standard.
* **CMake** (version 3.28 or newer).
* **Git** (required for automatically fetching the `{fmt}` library).

---

## Building the Project

The project uses CMake to manage the build process. The `{fmt}` formatting library is fetched automatically using `FetchContent`.

1.  **Clone the repository:**
    ```bash
    git clone <your-repository-url>
    cd Steganography-project
    ```

2.  **Create a build directory:**
    ```bash
    mkdir build
    cd build
    ```

3.  **Configure the project with CMake:**
    ```bash
    cmake ..
    ```

4.  **Compile the project:**
    ```bash
    cmake --build .
    ```
    This will create an executable named `Steganography_project` inside the `build` directory.

---

## Usage

Run the executable from the command line, followed by a command and the required arguments.

**Syntax:**
```bash
./Steganography_project <command> [arguments...]
````

**Important:** If a file path contains spaces, enclose it in double quotes (`""`).

### Commands

  * **Show Help Information**

    ```bash
    ./Steganography_project -h
    ```

  * **Display Image File Information**

    ```bash
    ./Steganography_project -i "path/to/your/image.bmp"
    ```

  * **Encrypt a Message**

    ```bash
    ./Steganography_project -e "path/to/your/image.ppm" "Your secret message"
    ```

  * **Decrypt a Message**

    ```bash
    ./Steganography_project -d "path/to/your/image.bmp"
    ```

  * **Check if a Message Can Be Encrypted**

    ```bash
    ./Steganography_project -c "path/to/your/image.ppm" "A very long message to check"
    ```

-----

## Project Structure

The project code is organized into several key components:

  * `main.cpp`: The main entry point. It handles parsing command-line arguments and calling the appropriate functions.
  * `ImageHandler.cpp` / `.h`: A module responsible for reading and writing `.bmp` and `.ppm` image files, including handling their specific header formats and pixel data.
  * `Steganography.cpp` / `.h`: Contains the core logic for the LSB encryption and decryption processes.
  * `CMakeLists.txt`: The build script that defines the project structure, dependencies (like the `{fmt}` library), and compilation settings.
