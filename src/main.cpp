#include <iostream>
#include <string>
#include <chrono>

// #include "quadtree.h"
#include "image.hpp"
#include "error.hpp"
#include "quadtree.hpp"

int main(){
    /* INPUT */
    std::string inputImageAddress;      // Image to be compressed
    ErrorMethod errorMethod;            // Error calculation method to be used
    double errorThreshold;              // Error threshold for block division
    int minBlockWidth, minBlockHeight;  // Minimum block size for block division (width, height)
    std::string outputImageAddress;     // Compressed image output address

    std::cout << "Enter the input image address: ";
    std::getline(std::cin, inputImageAddress);
    std::cout << "Image to be compressed is " << inputImageAddress << std::endl;

    std::cout << "Error method (enter the number)" << std::endl
                << " 1. Variance |"
                << " 2. Mean Absolute Deviation (MAD) |"
                << " 3. Max Pixel Difference |"
                << " 4. Entropy" << std::endl;

    int errorMethodInput;
    std::cin >> errorMethodInput;
    if (errorMethodInput < 1 || errorMethodInput > 4) {
        std::cerr << "Invalid error method." << std::endl;
        return 1;
    }
    errorMethod = static_cast<ErrorMethod>(errorMethodInput);
    std::cout << "Error method is " << errorMethod << std::endl;

    std::cout << "Enter the threshold: ";
    std::cin >> errorThreshold;
    if (errorThreshold < 0) {
        std::cerr << "Invalid error threshold." << std::endl;
        return 1;
    }
    std::cout << "Error threshold is " << errorThreshold << std::endl;

    std::cout << "Enter the minimum block size: ";
    std::cin >> minBlockWidth >> minBlockHeight;
    if (minBlockWidth <= 0 || minBlockHeight <= 0) {
        std::cerr << "Invalid minimum block size." << std::endl;
        return 1;
    }
    std::cout << "Minimum block size is " << minBlockWidth << "x" << minBlockHeight << std::endl;

    std::cout << "Enter the output image address: ";
    std::cin >> outputImageAddress;
    std::cout << "Compressed image will be saved at " << outputImageAddress << std::endl;

    /* PROCESS */
    Image* inputImage;
    try {
        inputImage = new Image(inputImageAddress); // Load the image
    } catch (const std::exception& e) {
        std::cerr << "Error loading image: " << e.what() << std::endl;
        return 1;
    }
    QuadTree* tree;
    try {
        tree = new QuadTree(*inputImage, minBlockWidth, minBlockHeight, errorThreshold, errorMethod); // Create the quadtree
    } catch (const std::exception& e) {
        std::cerr << "Error creating quadtree: " << e.what() << std::endl;
        delete inputImage;
        return 1;
    }

    // Quantum r = inputImage->img(50, 50, 0, 0); // Get the pixel value at (0, 0) for channel 0 (Red)  
    // std::cout << (int)r << std::endl;
    
    
    // std::cout << "Image dimensions: " << inputImage->getWidth() << "x" << inputImage->getHeight() << std::endl;
    // QuadTreeNode* root = new QuadTreeNode(0, 0, inputImage->getHeight()-1, inputImage->getWidth()-1);
    
    // int min = (int)inputImage->img(0,0,0,0), max = (int)inputImage->img(0,0,0,0);
    // for (int i=0;i<inputImage->getWidth();i++){
    //     for (int j=0;j<inputImage->getHeight();j++){
    //         if ((int)inputImage->img(i,j,0,0) < min){
    //             min = (int)inputImage->img(i,j,0,0);
    //         }
    //         if ((int)inputImage->img(i,j,0,0) > max){
    //             max = (int)inputImage->img(i,j,0,0);
    //         }
    //     }
    // }

    // auto begin = inputImage->beginBlock(0, 0, inputImage->getHeight()-1, inputImage->getWidth()-1, Channels::RED);
    // auto end = inputImage->endBlock(0, 0, inputImage->getHeight()-1, inputImage->getWidth()-1, Channels::RED);
    // auto minIt = (int)*begin;
    // auto maxIt = (int)*begin;
    // int pixelCount = 0;
    // for (auto it = begin; it != end; ++it) {
    //     if ((int)*it < minIt) {
    //         minIt = (int)*it;
    //     }
    //     if ((int)*it > maxIt) {
    //         maxIt = (int)*it;
    //     }
    //     pixelCount++;
    // }
    // std::cout << "Min: " << min << ", Max: " << max << std::endl;
    // std::cout << "Min: " << minIt << ", Max: " << maxIt << std::endl;
    // std::cout << "Pixel count: " << pixelCount << std::endl;
    // root->calculateError(*inputImage, ErrorMethod::MAX_PIXEL_DIFFERENCE); // Calculate the average color of the root node
    // root->calculateAverage(*inputImage); // Calculate the average color of the root node
    // std::cout << "Average color of the root node: " << root->averageR << ", " << root->averageG << ", " << root->averageB << std::endl;
    // std::cout << "Error of the root node: " << root->error << std::endl;

    auto t1 = std::chrono::high_resolution_clock::now();
    
    try {
        tree->divideExhaust(); // Divide the image into blocks
    } catch (const std::exception& e) {
        std::cerr << "Error during division: " << e.what() << std::endl;
        return 1;
    }
    try {
        Image outputImage = tree->merge(-1); // Merge the blocks into an image
        outputImage.save(outputImageAddress); // Save the compressed image
    } catch (const std::exception& e) {
        std::cerr << "Error during merging: " << e.what() << std::endl;
        return 1;
    }

    auto t2 = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);

    /* OUTPUT */
    std::cout << "Excecution time: " << ms.count() << "ms" << std::endl;
    std::cout << "Image size before compression: " << "MB" << std::endl;
    std::cout << "Image size after compression: " << "MB" << std::endl;
    std::cout << "Compression ratio: " << "x" << std::endl;
    std::cout << "Tree depth: " << "levels" << std::endl;
    std::cout << "Number of nodes: " << "nodes" << std::endl;

    std::cout << "-----------------------" << std::endl;
    

    return 0;
}