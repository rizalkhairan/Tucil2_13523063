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
    
    if (config.compressionTarget < 0 || config.compressionTarget > 1) {
        throw std::invalid_argument("Compression target must be between 0 and 1.");
    }
    
    try {
        inputImage = std::make_unique<Image>(config.inputImageAddress);
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

// Compress to a target compression ratio
void Compression::compressTargeted() {
    if (!validated) {
        throw std::runtime_error("Compression not validated. Call validate() first.");
    }

    // Binary search on error threshold
    // File compression is complicated but assume that the error threshold is directly proportional to the compression ratio
    std::unique_ptr<Image> midImage;
    std::unique_ptr<QuadTree> tempTree;
    std::string midAddress="__temp3__"+config.extension;
    double midRatio, bestRatio;
    // Thresholds for binary search
    double low = 0.0;
    double high = 255 * 64;     // Seems to be an upper bound max error possible on edge test case. On real image, the iteration may converge more slowly
    double mid;
    double best = high;
    
    int MAX_ITERATIONS = 20;
    double ERROR_TOLERANCE = 0.1;
    long long originalSize = calculateFileSize(config.inputImageAddress);

    // Make a tree that fully divides the image into smallest possible blocks
    tempTree = std::make_unique<QuadTree>(*inputImage, config.minBlockArea, low, config.errorMethod);
    tempTree->divideExhaust();

    // Find the lowest ratio that is above the target ratio
    int iteration = 0;
    while (high - low > ERROR_TOLERANCE && iteration < MAX_ITERATIONS) {
        mid = (high + low) / 2.0;
        midImage = std::make_unique<Image>(tempTree->mergeThreshold(mid, false));
        midImage->save(midAddress);
        midRatio = calculateCompressionRatio(originalSize, calculateFileSize(midAddress));
        
        if (midRatio < config.compressionTarget) {
            low = mid;
        } else {
            high = mid;
            best = mid; // Save the best ratio found so far
        }

        iteration++;
    }

    // Save the image with the highest ratio that is below the target ratio
    tree = std::make_unique<QuadTree>(*inputImage, config.minBlockArea, best, config.errorMethod);
    tree->divideExhaust();
    outputImage = std::make_unique<Image>(tree->merge(-1, false));
}


// Finalize the compression process and save the image
void Compression::save() {
    if (!outputImage) {
        throw std::runtime_error("No output image to save. Call compress() first.");
    }

    // Save the compressed image
    originalSize = calculateFileSize(config.inputImageAddress);
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
    return 100 * (1 - ((double)compressedSize / (double)originalSize));
}
// Calculate file size
long long Compression::calculateFileSize(const std::string& filePath) const {
    auto fileSize = std::filesystem::file_size(filePath);
    return std::filesystem::file_size(filePath);
}

