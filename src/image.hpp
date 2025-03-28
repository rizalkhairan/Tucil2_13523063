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

    // Modifiable access
    std::vector<std::vector<pixel>>* getChannel(Channels channel);
    // Read-only access
    const std::vector<std::vector<pixel>>* getChannel(Channels channel) const;
public:
    // Constructors and destructors
    Image(std::string address);
    Image(int width, int height, pixel r, pixel g, pixel b);
    Image(const Image &other);
    ~Image();
    
    // Dimension getters
    int getSize() const;
    int getWidth() const;
    int getHeight() const;

    // Pixel setters
    void paintBlockPixel(int rowStart, int colStart, int rowEnd, int colEnd, pixel r, pixel g, pixel b, bool addBorder);

    // Save the image to a file
    void save(std::string address);
    
    // For accessing all pixel values
    class Iterator {
    private:
        std::vector<std::vector<pixel>> *channel;
        int startRow, startCol;
        int endRow, endCol;
        int currentRow, currentCol;
    public:
        // Modifiable iterator
        Iterator(std::vector<std::vector<pixel>> *channel,
            int startRow, int startCol,
            int endRow, int endCol)
            : channel(channel), startRow(startRow), startCol(startCol),
            endRow(endRow), endCol(endCol), currentRow(startRow), currentCol(startCol) {};
        // Read-only iterator
        Iterator(const std::vector<std::vector<pixel>> *channel,
            int startRow, int startCol,
            int endRow, int endCol)
            : channel(const_cast<std::vector<std::vector<pixel>> *>(channel)), startRow(startRow), startCol(startCol),
            endRow(endRow), endCol(endCol), currentRow(startRow), currentCol(startCol) {};    
        
        // Iterator traits
        using iterator_category = std::forward_iterator_tag;
        using value_type = pixel;
        
        // Modifiable
        pixel& operator*() { return (*channel)[currentRow][currentCol]; }
        // Read-only
        pixel operator*() const { return (*channel)[currentRow][currentCol]; }
        
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

    // Use these methods to iterate over a channel of an image subblock, as to be used with the error calculation
    Iterator beginBlock(int startRow, int startCol, int endRow, int endCol, Channels channel);
    Iterator endBlock(int startRow, int startCol, int endRow, int endCol, Channels channel);

    // Read-only access
    Iterator beginBlock(int startRow, int startCol, int endRow, int endCol, Channels channel) const;
    Iterator endBlock(int startRow, int startCol, int endRow, int endCol, Channels channel) const;
};

#endif