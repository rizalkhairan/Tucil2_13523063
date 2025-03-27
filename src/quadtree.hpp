#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#include <array>
#include <memory>
#include "image.hpp"
#include "error.hpp"

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

    // Constructor
    QuadTreeNode();
    QuadTreeNode(int rowStart, int colStart, int rowEnd, int colEnd)
        : rowStart(rowStart), colStart(colStart), rowEnd(rowEnd), colEnd(colEnd),
        averageR(0), averageG(0), averageB(0), error(0), isDivisible(true) {};
    ~QuadTreeNode() {};
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


    // Divide nodes
    int divideNode(QuadTreeNode& node, int minBlockWidth, int minBlockHeight, double errorThreshold, ErrorMethod errorMethod);

    // Merge nodes
    void mergeNode(const QuadTreeNode& node, Image& outputImage) const;
public:
    // Constructor and destructor
    QuadTree(const Image& image);
    ~QuadTree();

    // Getters
    int getNodeCount() const;
    int getTreeDepth() const;

    // Divide all divisible leaf nodes
    void divide(int minBlockWidth, int minBlockHeight, double errorThreshold, ErrorMethod errorMethod);

    // Merge the current tree into an Image
    Image merge() const;
};

#endif