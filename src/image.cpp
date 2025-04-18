#include <stdexcept>
#include <string>
#include "image.hpp"

// Constructors and destructors
// From file
Image::Image(std::string address) {
    cimg_library::CImg<Quantum> image(address.c_str());
    if (image.is_empty()) {
        throw std::runtime_error("Image not found or empty.");
    }
    if (image.spectrum() < 3) {
        throw std::runtime_error("Image do not have at least RGB channels.");
    }

    // Image with alpha channel is set to opaque
    if (image.spectrum() == 4) {
        image.channel(Channels::ALPHA).fill(1);
    }

    this->img = image;

}
// Image object with given dimensions and color
Image::Image(int width, int height, Quantum r, Quantum g, Quantum b) {
    cimg_library::CImg<Quantum> image(width, height, 1, 3, 0);  // width, height, depth, channel count, pixel initial value
    Quantum pixel[3] = { r, g, b };
    image.draw_rectangle(0, 0, width - 1, height - 1, pixel);
    
    this->img = image;
}
// Copy constructor
Image::Image(const Image &other) {
    // Deep copy. Does not share buffer
    this->img = cimg_library::CImg<Quantum>(other.img, false);
}

Image::~Image() { }

// Dimension getters
int Image::getSize() const { return img.width() * img.height(); }
int Image::getWidth() const { return img.width(); }
int Image::getHeight() const { return img.height(); }

// Pixel setters
void Image::paintBlockPixel(int rowStart, int colStart, int rowEnd, int colEnd, Quantum r, Quantum g, Quantum b, bool addBorder) {
    // Check if the coordinates are within the image bounds
    if (rowStart < 0 || colStart < 0 || rowEnd >= img.height() || colEnd >= img.width()) {
        throw std::out_of_range("Coordinates are out of bounds.");
    }

    // Set the pixel values in the specified block
    Quantum pixel[3] = { r, g, b };
    img.draw_rectangle(colStart, rowStart, colEnd, rowEnd, pixel);

    // Add border if requested
    if (addBorder) {
        // Thin (1 pixel) black border on top and right side of the block
        // Does not paint border on image border
        Quantum borderPixel[3] = { 0, 0, 0 }; // Black border
        if (rowStart != 0) {
            img.draw_rectangle(colStart, rowStart, colEnd, rowStart, borderPixel); // Top border
        }
        if (colEnd != img.width() - 1) {
            img.draw_rectangle(colEnd, rowStart, colEnd, rowEnd, borderPixel); // Right border
        }
    }
}

// Save the image to a file
void Image::save(std::string address) {
    // Save the image to the specified address
    img.save(address.c_str());
}

// Push image into a GIF frame
void Image::pushFrame(GifEncoder& gifEncoder, int delay) const {
    // Transform into interleaved RGB form
    // CImg stores data in planar form like so R1R2R3...RnG1G2G3...GnB1B2B3...Bn
    // GIF encoder requires interleaved format R1G1B1R2G2B2...RnGnBn
    uint8_t* data = (uint8_t*)malloc(8 * img.width() * img.height() * 3);
    if (data == nullptr) {
        throw std::runtime_error("Failed to allocate memory for GIF frame.");
    }
    int pos = 0;
    for (auto it = beginBlock(0, 0, img.height() - 1, img.width() - 1, Channels::RED);
            it != endBlock(0, 0, img.height() - 1, img.width() - 1, Channels::RED); ++it) {
        *(data+pos) = *it;
        pos += 3;
    }
    pos = 1;
    for (auto it = beginBlock(0, 0, img.height() - 1, img.width() - 1, Channels::GREEN);
            it != endBlock(0, 0, img.height() - 1, img.width() - 1, Channels::GREEN); ++it) {
        *(data+pos) = *it;
        pos += 3;
    }
    pos = 2;
    for (auto it = beginBlock(0, 0, img.height() - 1, img.width() - 1, Channels::BLUE);
            it != endBlock(0, 0, img.height() - 1, img.width() - 1, Channels::BLUE); ++it) {
        *(data+pos) = *it;
        pos += 3;
    }

    // Immediately transform the data into intermediate buffer so data can be safely freed
    gifEncoder.push(GifEncoder::PIXEL_FORMAT_RGB, data, img.width(), img.height(), delay);
    free(data);
}


/* Iterator */
// Modifiable iterator
Image::Iterator Image::beginBlock(int startRow, int startCol, int endRow, int endCol, Channels channel) const {
    return Image::Iterator(img, channel, startRow, startCol, endRow, endCol);
}
Image::Iterator Image::endBlock(int startRow, int startCol, int endRow, int endCol, Channels channel) const {
    return Image::Iterator(img, channel, endRow+1, startCol, endRow+1, startCol);
}