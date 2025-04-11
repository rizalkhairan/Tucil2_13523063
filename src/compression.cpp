#include <filesystem>
#include "compression.hpp"

void Compression::validate() {
    if (config.inputImageAddress.empty()) {
        throw std::invalid_argument("Input image address is empty.");
    }
    if (config.outputImageAddress.empty()) {
        throw std::invalid_argument("Output image address is empty.");
    }
    if (!config.outputGIFAddress.empty() && ".gif"!=std::filesystem::path(config.outputGIFAddress).extension().string()) {
        throw std::invalid_argument("Output GIF address does not have .gif extension.");
    }
    
    if (!ErrorMetrics::validThreshold(config.errorThreshold, config.errorMethod)) {
        throw std::invalid_argument("Invalid error threshold.");
    }
    if (config.minBlockArea < 1) {
        throw std::invalid_argument("Minimum block area must be at least 1.");
    }
    
    try {
        inputImage = std::make_unique<Image>(config.inputImageAddress);
        originalSize = calculateFileSize(config.inputImageAddress);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to load input image: " + std::string(e.what()));
    }

    // Output image format not supported
    // Allows different input and output file formats
    config.extension = std::filesystem::path(config.outputImageAddress).extension().string();
    if (config.extension!=".jpg" && config.extension!=".jpeg" && config.extension != ".png" && config.extension != ".bmp") {
        throw std::invalid_argument("Unable to save compressed image to unsupported image file format.");
    }
    
    validated = true;
}

// Compress by QuadTree
void Compression::compress() {
    if (!validated) {
        throw std::runtime_error("Compression not validated. Call validate() first.");
    }

    tree = std::make_unique<QuadTree>(*inputImage, config.minBlockArea, config.errorThreshold, config.errorMethod);
    tree->divideExhaust();
    outputImage = std::make_unique<Image>(tree->merge(-1));
}

// Finalize the compression process and save the image
void Compression::save() {
    if (!outputImage) {
        throw std::runtime_error("No output image to save. Call compress() first.");
    }

    // Save the compressed image
    try {
        outputImage->save(config.outputImageAddress);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to save output image: " + std::string(e.what()));
    }
    compressedSize = calculateFileSize(config.outputImageAddress);
    compressionRatio = calculateCompressionRatio(originalSize, compressedSize);
}

// Form GIF image that visualizes the compression process
void Compression::formGIF() {
    if (!tree) {
        throw std::runtime_error("Call compress() first.");
    }

    int quality = GIF_QUALITY;      // 66.7% quality. Best quality produce rather gorgeous gifs but takes a lot of time (10 frames require about 1 minute to process)
    bool useGlobalColorMap = false; // Global color map would not look great accross all depths but color quantization is supposedly faster(?)
    int loop = 0;                   // Endless loop
    int preAllocSize = 0;           // No idea what this parameter would do but it works just fine
    int delay = GIF_DELAY;         
    int loopDelay = GIF_LOOP_DELAY;

    // Create GIF encoder
    GifEncoder gifEncoder;
    if (!gifEncoder.open(config.outputGIFAddress, inputImage->getWidth(), inputImage->getHeight(), quality, useGlobalColorMap, loop, preAllocSize)) {
        throw std::runtime_error("Failed to open GIF encoder.");
    }

    for (int i=1;i<=getTreeDepth();i++){
        // Merge the tree at the current depth and save it as a GIF frame
        auto gifImage = std::make_unique<Image>(tree->merge(i, false));
        if (i==getTreeDepth()) {
            gifImage->pushFrame(gifEncoder, loopDelay); // Last frame has longer delay
        } else {
            gifImage->pushFrame(gifEncoder, delay);
        }
    }

    if (!gifEncoder.close()) {
        throw std::runtime_error("Failed to close GIF encoder.");
    }
}

// Compression information
long long Compression::getOriginalSize() const { return originalSize; }
long long Compression::getCompressedSize() const { return compressedSize; }
double Compression::getCompressionRatio() const { return compressionRatio; }
int Compression::getTreeDepth() const { return tree ? tree->getTreeDepth() : 0; }
int Compression::getNodeCount() const { return tree ? tree->getNodeCount() : 0; }

// Utility methods
// Calculate compression ratio
double Compression::calculateCompressionRatio(long long originalSize, long long compressedSize) const {
    if (originalSize == 0) {
        throw std::invalid_argument("Original size cannot be zero.");
    }
    return (1 - ((double)compressedSize / (double)originalSize));
}
// Calculate file size
long long Compression::calculateFileSize(const std::string& filePath) const {
    auto fileSize = std::filesystem::file_size(filePath);
    return std::filesystem::file_size(filePath);
}

