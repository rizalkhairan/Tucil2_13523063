#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <vector>
#include <iterator>

typedef unsigned char pixel;

enum Channels {
    RED = 0,
    GREEN = 1,
    BLUE = 2
};

class Image {
private:
    int width, height;      // Whole image dimensions
    int size;               // Number of pixels in the image

    // Image color channels
    std::vector<std::vector<pixel>> r;
    std::vector<std::vector<pixel>> g;
    std::vector<std::vector<pixel>> b;
    const std::vector<std::vector<pixel>> *getChannel(Channels channel) const;
public:
    // Constructors and destructors
    Image(std::string address);
    Image(int width, int height, pixel r, pixel g, pixel b);
    Image(const Image &other);
    ~Image();
    
    // Dimension getters
    int getSize();
    int getWidth();
    int getHeight();

    // Save the image to a file
    void save(std::string address);
    
    // For accessing all pixel values
    class Iterator {
    private:
        const std::vector<std::vector<pixel>> *channel;
        size_t startRow, startCol;
        size_t endRow, endCol;
        size_t currentRow, currentCol;
    public:
        Iterator(const std::vector<std::vector<pixel>> *channel,
            size_t startRow, size_t startCol,
            size_t endRow, size_t endCol)
            : channel(channel), startRow(startRow), startCol(startCol),
            endRow(endRow), endCol(endCol), currentRow(startRow), currentCol(startCol) {};
            
        // Iterator traits
        using iterator_category = std::forward_iterator_tag;
        using value_type = pixel;
        
        pixel operator*() const { return (*channel)[currentRow][currentCol]; }
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

    // Use these methods to iterate over a channel of an image subblock, as to be used with the error calculation
    Iterator beginBlock(size_t startRow, size_t startCol, size_t endRow, size_t endCol, Channels channel) const;
    Iterator endBlock(size_t startRow, size_t startCol, size_t endRow, size_t endCol, Channels channel) const;
};

#endif