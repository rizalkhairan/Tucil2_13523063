#include <iostream>
#include <chrono>

#include "compression.hpp"

int main(){
    /* INPUT */
    CompressionConfig config;

    std::cout << "Enter the input image address: ";
    std::getline(std::cin, config.inputImageAddress);

    std::cout << "Error method (enter the number)" << std::endl
                << "1. Variance | "
                << "2. Mean Absolute Deviation (MAD) | "
                << "3. Max Pixel Difference | "
                << "4. Entropy" << std::endl;

    int errorMethodInput;
    std::cin >> errorMethodInput;
    if (errorMethodInput < 1 || errorMethodInput > 4) {
        std::cerr << "[Error] Invalid error method." << std::endl;
        return 1;
    }
    config.errorMethod = static_cast<ErrorMethod>(errorMethodInput);

    std::cout << "Enter error threshold: ";
    std::cin >> config.errorThreshold;

    std::cout << "Enter minimum block area: ";
    std::cin >> config.minBlockArea;
    
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer
    std::cout << "Enter the output image address: ";
    std::getline(std::cin, config.outputImageAddress);
    
    /* PROCESS */
    Compression compression(config);
    try {
        compression.validate(); // Validate the input parameters
    } catch (const std::exception& e) {
        std::cerr << "[Error] " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "Compressing image..." << std::endl;
    auto t1 = std::chrono::high_resolution_clock::now();
    
    compression.compress();
    
    auto t2 = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    
    /* OUTPUT */
    std::cout << "Saving compressed image..." << std::endl;
    compression.save();
    std::cout << "------------------------------------------------------------" << std::endl;
    std::cout << "Compression excecution time: " << ms.count() << "ms" << std::endl;
    std::cout << "Image size before compression: " << compression.getOriginalSize() << " bytes" << std::endl;
    std::cout << "Image size after compression: " << compression.getCompressedSize() << " bytes" << std::endl;
    std::cout << "Compression percentage: " << compression.getCompressionRatio() << "%" << std::endl;
    std::cout << "Tree depth: " << compression.getTreeDepth() << std::endl;
    std::cout << "Number of nodes: " << compression.getNodeCount() << std::endl;
    
    return 0;
}
