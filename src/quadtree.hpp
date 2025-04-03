#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#include <array>
#include <memory>
#include "image.hpp"
#include "error.hpp"

#define QUADTREE_MAX_DEPTH 50

class QuadTreeNode {
public:
    // Children
    std::array<std::unique_ptr<QuadTreeNode>, 4> children;

    // Node data
    double averageR, averageG, averageB;
    double error;

    // Block boundary
    int rowStart, colStart;
    int rowEnd, colEnd;

    bool isDivisible;   // Default to true
    bool isLeaf;        // Default to true

    // Constructor
    QuadTreeNode();
    QuadTreeNode(int rowStart, int colStart, int rowEnd, int colEnd);
    ~QuadTreeNode() {};

    // Dimension getter
    int getWidth() const { return colEnd - colStart + 1; }
    int getHeight() const { return rowEnd - rowStart + 1; }
    int getArea() const { return getWidth() * getHeight(); }

    // Error calculation that set the error attribute
    void calculateError(const Image& image, ErrorMethod errorMethod);
    
    // Average calculation that set the averageR, averageG, and averageB attributes
    void calculateAverage(const Image& image);
};

class QuadTree {
private:
    // Root node
    std::unique_ptr<QuadTreeNode> root;

    // Image to be compressed
    const Image& image;

    // Tree information
    int nodeCount;  // Root, leaves and internal nodes
    int treeDepth;  // Incremented with each divide call
    int depthOnLastColorCalc; // Depth of the last average color calculation

    // Compression parameters
    int minBlockArea;
    double errorThreshold;
    ErrorMethod errorMethod;

    // Calculate all node's average color
    void calculateAverageColor() const;

    // Divide nodes
    int divideNode(QuadTreeNode& node);

    // Merge nodes up to variable depth. Calculate average RGB value from each leaf node
    void mergeNodeDepth(QuadTreeNode& node, Image& outputImage, int depth) const;

    // Merge nodes on variable error threshold
    void mergeNodeThreshold(QuadTreeNode& node, Image& outputImage, double errorThreshold) const;

public:
    // Constructor and destructor
    QuadTree(const Image& image, int minBlockSize, double errorThreshold, ErrorMethod errorMethod);
    ~QuadTree();

    // Getters
    int getNodeCount() const;
    int getTreeDepth() const;

    // Divide all current divisible leaf nodes per level
    int divide();

    // Divide until exhaustion
    void divideExhaust();
    
    // Merge the current tree into an Image
    Image merge(int depth) const;

    // Merge with variable error threshold
    Image mergeThreshold(double errorThreshold) const;

};

#endif