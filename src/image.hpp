#ifndef IMAGE_HPP
#define IMAGE_HPP

#define cimg_display 0      // Disable CImg display methods thus not requiring X11 or gdi
#define cimg_use_jpeg 1     // Use libjpeg for JPEG support
#include "CImg.h"

typedef unsigned char Quantum;      // RGB value type

enum Channels {
    RED = 0,
    GREEN = 1,
    BLUE = 2
};

class Image {
private:
    // Image object
    cimg_library::CImg<Quantum> img;
public:
    // Constructors and destructors
    // From file
    Image(std::string address);
    // Image object with given dimensions and color
    Image(int width, int height, Quantum r, Quantum g, Quantum b);
    // Copy constructor
    Image(const Image &other);
    ~Image();
    
    // Dimension getters
    int getSize() const;
    int getWidth() const;
    int getHeight() const;

    // Pixel setters
    void paintBlockPixel(int rowStart, int colStart, int rowEnd, int colEnd, Quantum r, Quantum g, Quantum b, bool addBorder);

    // Save the image to a file
    void save(std::string address);
    
    // For accessing pixel values
    class Iterator {
    private:
        const cimg_library::CImg<Quantum>& img;
        Channels channel;
        int startRow, startCol;
        int endRow, endCol;
        int currentRow, currentCol;
    public:
        // Modifiable iterator
        Iterator(const cimg_library::CImg<Quantum>& imgref, Channels channel,
            int startRow, int startCol,
            int endRow, int endCol)
            : img(imgref), channel(channel), startRow(startRow), startCol(startCol),
            endRow(endRow), endCol(endCol), currentRow(startRow), currentCol(startCol) { };
        
        // Iterator traits
        using value_type = Quantum;
        
        // Accessor methods
        // Read-only
        Quantum operator*() const { return img.atXY(currentCol, currentRow, channel); }
        
        // Iterator controls
        Iterator& operator++() {
            ++currentCol;
            if (currentCol >= endCol) { // Move to the next row
                currentCol = startCol;
                ++currentRow;
            }
            if (currentRow >= endRow) { // Sentinel. Move to the end
                currentRow = endRow;
                currentCol = endCol;
            }
            return *this;
        }
        bool operator==(const Iterator &other) const {
            return currentRow == other.currentRow && currentCol == other.currentCol;
        }
        bool operator!=(const Iterator &other) const { return !(*this == other); }
    };

    // Use these methods to iterate over a channel of an image subblock, as to be used with the error calculation and QuadTree formation
    Iterator beginBlock(int startRow, int startCol, int endRow, int endCol, Channels channel) const;
    Iterator endBlock(int startRow, int startCol, int endRow, int endCol, Channels channel) const;
};

#endif