#include <stdexcept>
#include <string>
#include <iostream>
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
        image.channel(Channels::ALPHA).fill(1); // Make opaque
    }

    this->img = image;

}
// Image object with given dimensions and color
Image::Image(int width, int height, Quantum r, Quantum g, Quantum b) {
    cimg_library::CImg<Quantum> image(width, height, 1, 3, 0);
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
        Quantum borderPixel[3] = { 0, 0, 0 }; // Black border
        img.draw_rectangle(colStart, rowStart, colEnd - 1, rowStart + 1, borderPixel); // Top border
        img.draw_rectangle(colStart, rowEnd - 1, colEnd - 1, rowEnd, borderPixel); // Bottom border
        img.draw_rectangle(colStart, rowStart, colStart + 1, rowEnd - 1, borderPixel); // Left border
        img.draw_rectangle(colEnd - 1, rowStart, colEnd, rowEnd - 1, borderPixel); // Right border
    }
}

// Save the image to a file
void Image::save(std::string address) {
    // Save the image to the specified address
    img.save(address.c_str());
}


/* Iterator */
// Modifiable iterator
Image::Iterator Image::beginBlock(int startRow, int startCol, int endRow, int endCol, Channels channel) const {
    return Image::Iterator(img, channel, startRow, startCol, endRow, endCol);
}
Image::Iterator Image::endBlock(int startRow, int startCol, int endRow, int endCol, Channels channel) const {
    return Image::Iterator(img, channel, endRow+1, startCol, endRow+1, startCol);
}