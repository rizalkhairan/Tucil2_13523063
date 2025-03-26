#include <iostream>
#include <string>
#include <chrono>

// #include "quadtree.h"
// #include "image.h"
// #include "util.h"

int main(){
    /* INPUT */
    std::string inputImageAddress;      // Image to be compressed
    int errorMethod;                    // Error calculation method to be used
    double errorThreshold;              // Error threshold for block division
    int minBlockWidth, minBlockHeight;  // Minimum block size for block division (width, height)
    std::string outputImageAddress;     // Compressed image output address

    std::cout << "Enter the input image address: ";
    std::cin >> inputImageAddress;
    std::cout << "Image to be compressed is " << inputImageAddress << std::endl;

    std::cout << "Error method (enter the number)" << std::endl
                << " 1. Variance |"
                << " 2. Mean Absolute Deviation (MAD) |"
                << " 3. Max Pixel Difference |"
                << " 4. Entropy" << std::endl;
    std::cin >> errorMethod;
    std::cout << "Error method is " << errorMethod << std::endl;

    std::cout << "Enter the threshold: ";
    std::cin >> errorThreshold;
    std::cout << "Error threshold is " << errorThreshold << std::endl;

    std::cout << "Enter the minimum block size: ";
    std::cin >> minBlockWidth >> minBlockHeight;
    std::cout << "Minimum block size is " << minBlockWidth << "x" << minBlockHeight << std::endl;

    std::cout << "Enter the output image address: ";
    std::cin >> outputImageAddress;
    std::cout << "Compressed image will be saved at " << outputImageAddress << std::endl;

    /* PROCESS */
    auto t1 = std::chrono::high_resolution_clock::now();
    // Process here
    auto t2 = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);

    /* OUTPUT */
    std::cout << "Excecution time: " << ms.count() << "ms" << std::endl;
    std::cout << "Image size before compression: " << "MB" << std::endl;
    std::cout << "Image size after compression: " << "MB" << std::endl;
    std::cout << "Compression ratio: " << "x" << std::endl;
    std::cout << "Tree depth: " << "levels" << std::endl;
    std::cout << "Number of nodes: " << "nodes" << std::endl;

    return 0;
}