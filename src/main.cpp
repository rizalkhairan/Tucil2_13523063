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
                << "4. Entropy | "
                << "5. SSIM" << std::endl;

    int errorMethodInput;
    std::cin >> errorMethodInput;
    if (errorMethodInput < 1 || errorMethodInput > 5) {
        std::cerr << "[Error] Invalid error method." << std::endl;
        return 1;
    }
    config.errorMethod = static_cast<ErrorMethod>(errorMethodInput);

    std::cout << "Enter error threshold: ";
    std::cin >> config.errorThreshold;

    std::cout << "Enter minimum block area: ";
    std::cin >> config.minBlockArea;

    std::cout << "Target compression ratio (enter 0 to disable): ";
    std::cin >> config.compressionTarget;
    
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer
    std::cout << "Enter the output image address: ";
    std::getline(std::cin, config.outputImageAddress);

    std::cout << "Enter the GIF output address (optional, press enter to skip): ";
    std::getline(std::cin, config.outputGIFAddress);
    
    /* PROCESS */
    Compression compression(config);
    try {
        compression.validate(); // Validate the input parameters
    } catch (const std::exception& e) {
        std::cerr << "[Error] " << e.what() << std::endl;
        return 1;
    }
    
    auto t1 = std::chrono::high_resolution_clock::now();
    
    if (config.compressionTarget > 0) {
        std::cout << "Compressing image to target compression ratio..." << std::endl;
        compression.compressTargeted();
    } else {
        std::cout << "Compressing image..." << std::endl;
        compression.compress();
    }
    
    auto t2 = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    
    /* OUTPUT */
    std::cout << "Saving compressed image..." << std::endl;
    compression.save();
    if (!config.outputGIFAddress.empty()) {
        std::cout << "Generating GIF..." << std::endl;
        try {
            compression.formGIF();
        } catch (const std::exception& e) {
            std::cerr << "[Error] " << e.what() << std::endl;
            return 1;
        }
    }
    
    std::cout << "------------------------------------------------------------" << std::endl;
    std::cout << "Compression excecution time: " << ms.count() << "ms" << std::endl;
    std::cout << "Image size before compression: " << compression.getOriginalSize() << " bytes" << std::endl;
    std::cout << "Image size after compression: " << compression.getCompressedSize() << " bytes" << std::endl;
    std::cout << "Compression percentage: " << 100 * compression.getCompressionRatio() << "%" << std::endl;
    std::cout << "Tree depth: " << compression.getTreeDepth() << std::endl;
    std::cout << "Number of nodes: " << compression.getNodeCount() << std::endl;
    
    return 0;
}
