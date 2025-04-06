#ifndef COMPRESSION_HPP
#define COMPRESSION_HPP

#include <string>
#include "error.hpp"
#include "image.hpp"
#include "quadtree.hpp"

// Parameters
struct CompressionConfig {
    std::string inputImageAddress="";       // Image to be compressed
    std::string outputImageAddress="";      // Compressed image output address
    std::string extension=".jpg";           // Target file extension
    double errorThreshold=0.0;              // Error threshold for block division
    double compressionTarget=0.0;           // Compression percentage target (not implemented yet)
    int minBlockArea=1;                     // Minimum block size for block division (width, height)
    ErrorMethod errorMethod=VARIANCE;       // Error calculation method to be used
};

class Compression {
private:
    // Compression data
    std::unique_ptr<Image> inputImage, outputImage;
    std::unique_ptr<QuadTree> tree;
    long long originalSize, compressedSize;
    double compressionRatio;

    // Program parameters
    CompressionConfig config;
    bool validated;

public:
    Compression(CompressionConfig conf) : config(conf), validated(false), inputImage(nullptr), outputImage(nullptr), tree(nullptr) {}
    ~Compression() {}

    // Must be called before calling other methods. Will throw exceptions if the parameters are invalid
    void validate();

    // Compression. Form the compression tree and output image in memory
    void compress();
    void compressTargeted();

    // Finalize the compression process and save the image
    void save();

    // Compression information
    long long getOriginalSize() const;
    long long getCompressedSize() const;
    double getCompressionRatio() const;
    int getTreeDepth() const;
    int getNodeCount() const;

    // Utility methods
    // Calculate compression ratio
    double calculateCompressionRatio(long long originalSize, long long compressedSize) const;
    // Calculate file size
    long long calculateFileSize(const std::string& filePath) const;
};


#endif