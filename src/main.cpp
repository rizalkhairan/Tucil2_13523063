#include <iostream>
#include <string>
#include <memory>
#include <chrono>
#include <filesystem>

#include "image.hpp"
#include "error.hpp"
#include "quadtree.hpp"

struct CompressionConfig {
    std::string inputImageAddress="";       // Image to be compressed
    std::string outputImageAddress="";      // Compressed image output address
    double errorThreshold=0.0;              // Error threshold for block division
    double compressionTarget=0.0;           // Compression percentage target (not implemented yet)
    int minBlockArea=1;                     // Minimum block size for block division (width, height)
    ErrorMethod errorMethod=VARIANCE;       // Error calculation method to be used
};

void compress(std::unique_ptr<Image>& inputImage, std::unique_ptr<Image>& outputImage, std::unique_ptr<QuadTree>& tree, const CompressionConfig& config) {
    tree = std::make_unique<QuadTree>(*inputImage, config.minBlockArea, config.errorThreshold, config.errorMethod);
    tree->divideExhaust(); // Divide the image into blocks
    outputImage = std::make_unique<Image>(tree->merge(-1));
}

long long calculateFileSize(const std::string& filePath) {
    auto fileSize = std::filesystem::file_size(filePath);
    return std::filesystem::file_size(filePath);
}

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
        std::cerr << "Invalid error method." << std::endl;
        return 1;
    }
    config.errorMethod = static_cast<ErrorMethod>(errorMethodInput);

    std::cout << "Enter error threshold: ";
    std::cin >> config.errorThreshold;
    if (config.errorThreshold < 0) {
        std::cerr << "Invalid error threshold." << std::endl;
        return 1;
    }

    std::cout << "Enter minimum block area: ";
    std::cin >> config.minBlockArea;
    if (config.minBlockArea < 1) {
        std::cerr << "Invalid minimum block area." << std::endl;
        return 1;
    }
    
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer
    std::cout << "Enter the output image address: ";
    std::getline(std::cin, config.outputImageAddress);
    
    /* PROCESS */
    std::unique_ptr<Image> inputImage;
    std::unique_ptr<Image> outputImage;
    std::unique_ptr<QuadTree> tree;
    try {
        inputImage = std::make_unique<Image>(config.inputImageAddress);
    } catch (const std::exception& e) {
        std::cerr << "Error loading image: " << e.what() << std::endl;
        return 1;
    }
    
    auto t1 = std::chrono::high_resolution_clock::now();
    
    compress(inputImage, outputImage, tree, config);
    
    auto t2 = std::chrono::high_resolution_clock::now();
    
    /* OUTPUT */
    long long filesizeBefore = calculateFileSize(config.inputImageAddress);
    try {
        outputImage->save(config.outputImageAddress); // Save the compressed/merged image
    } catch (const std::exception& e) {
        std::cerr << "Error saving image: " << e.what() << std::endl;
        return 1;
    }
    long long filesizeAfter = calculateFileSize(config.outputImageAddress);
    double compressionPercentage = 100 * (1 - ((double)filesizeAfter / (double)filesizeBefore));
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);

    std::cout << "------------------------------------------------------------" << std::endl;
    std::cout << "Compression excecution time: " << ms.count() << "ms" << std::endl;
    std::cout << "Image size before compression: " << filesizeBefore << " bytes" << std::endl;
    std::cout << "Image size after compression: " << filesizeAfter << " bytes" << std::endl;
    std::cout << "Compression percentage: " << compressionPercentage << "%" << std::endl;
    std::cout << "Tree depth: " << tree->getTreeDepth() << std::endl;
    std::cout << "Number of nodes: " << tree->getNodeCount() << std::endl;
    
    return 0;
}
