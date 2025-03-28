#include "quadtree.hpp"
#include "image.hpp"

/* QuadTreeNode */
QuadTreeNode::QuadTreeNode()
    : rowStart(0), colStart(0), rowEnd(0), colEnd(0),
    averageR(0), averageG(0), averageB(0), error(0), isDivisible(true), isLeaf(true) {
    for (int i = 0; i < 4; i++) {
        children[i] = nullptr;
    }
}

QuadTreeNode::QuadTreeNode(int rowStart, int colStart, int rowEnd, int colEnd)
    : rowStart(rowStart), colStart(colStart), rowEnd(rowEnd), colEnd(colEnd),
    averageR(0), averageG(0), averageB(0), error(0), isDivisible(true), isLeaf(true) {
    for (int i = 0; i < 4; i++) {
        children[i] = nullptr;
    }
}

// Error calculation that set the error attribute
void QuadTreeNode::calculateError(const Image& image, ErrorMethod errorMethod){
    double errorR, errorG, errorB;

    errorR = ErrorMetrics::calculateChannelError(errorMethod,
        image.beginBlock(rowStart, colStart, rowEnd, colEnd, Channels::RED),
        image.endBlock(rowStart, colStart, rowEnd, colEnd, Channels::RED));
    errorG = ErrorMetrics::calculateChannelError(errorMethod,
        image.beginBlock(rowStart, colStart, rowEnd, colEnd, Channels::GREEN),
        image.endBlock(rowStart, colStart, rowEnd, colEnd, Channels::GREEN));
    errorB = ErrorMetrics::calculateChannelError(errorMethod,
        image.beginBlock(rowStart, colStart, rowEnd, colEnd, Channels::BLUE),
        image.endBlock(rowStart, colStart, rowEnd, colEnd, Channels::BLUE));
    
    error = ErrorMetrics::calculateError(errorMethod, errorR, errorG, errorB);
    return;
}

// Average calculation that set the averageR, averageG, and averageB attributes
void QuadTreeNode::calculateAverage(const Image& image){
    averageR = 0;
    averageG = 0;
    averageB = 0;
    long count = 0;

    for (auto it = image.beginBlock(rowStart, colStart, rowEnd, colEnd, Channels::RED);
        it != image.endBlock(rowStart, colStart, rowEnd, colEnd, Channels::RED); ++it) {
        averageR += *it;
        count++;
    }
    averageR /= count;

    count = 0;
    for (auto it = image.beginBlock(rowStart, colStart, rowEnd, colEnd, Channels::GREEN);
        it != image.endBlock(rowStart, colStart, rowEnd, colEnd, Channels::GREEN); ++it) {
        averageG += *it;
        count++;
    }
    averageG /= count;

    count = 0;
    for (auto it = image.beginBlock(rowStart, colStart, rowEnd, colEnd, Channels::BLUE);
        it != image.endBlock(rowStart, colStart, rowEnd, colEnd, Channels::BLUE); ++it) {
        averageB += *it;
        count++;
    }
    averageB /= count;
    
    return;
}



/* --------------------------------------------------*/
/* QuadTree */

// Constructor and destructor
QuadTree::QuadTree(const Image& image) : image(image), nodeCount(0), treeDepth(0) {
    root = std::make_unique<QuadTreeNode>(0, 0, image.getHeight(), image.getWidth());
}
QuadTree::~QuadTree() {}

// Getters
int QuadTree::getNodeCount() const { return nodeCount; }
int QuadTree::getTreeDepth() const { return treeDepth; }

/* Divide and conquer */

// Divide nodes per level
// Returns the number of nodes divided
int QuadTree::divideNode(QuadTreeNode& node, int minBlockWidth, int minBlockHeight, double errorThreshold, ErrorMethod errorMethod) {
    // Case 1: Node is an intermediate node
    if (!node.isLeaf) {
        int count = 0;
        for (int i = 0; i < 4; i++) {
            count += divideNode(*node.children[i], minBlockWidth, minBlockHeight, errorThreshold, errorMethod);
        }
        return count;
    }

    // Case 2: Node is a leaf node (divide if necessary)
    if (!node.isDivisible) return 0;    // Node is immediately ignored

    if (node.rowEnd - node.rowStart <= minBlockHeight || node.colEnd - node.colStart <= minBlockWidth) {
        // The node is smaller than the minimum block size
        node.isDivisible = false;
        return 0;
    }

    node.calculateError(image, errorMethod);
    if (node.error <= errorThreshold) {
        // The node is below the error threshold/the pixels are similar
        node.isDivisible = false;
        return 0;
    }

    // Divide the node
    int rowMid = (node.rowStart + node.rowEnd) / 2;
    int colMid = (node.colStart + node.colEnd) / 2;

    // Create children
    // Divided into these 4 panels in order:
    // 0 1
    // 2 3
    node.isLeaf = false;
    node.children[0] = std::make_unique<QuadTreeNode>(node.rowStart, node.colStart, rowMid, colMid);
    node.children[1] = std::make_unique<QuadTreeNode>(node.rowStart, colMid+1, rowMid, node.colEnd);
    node.children[2] = std::make_unique<QuadTreeNode>(rowMid+1, node.colStart, node.rowEnd, colMid);
    node.children[3] = std::make_unique<QuadTreeNode>(rowMid+1, colMid+1, node.rowEnd, node.colEnd);

    return 4;
}

// Merge nodes. Calculate average RGB value from each leaf node
void QuadTree::mergeNode(QuadTreeNode& node, Image& outputImage) const {
    if (node.isLeaf) {
        // Fill the block with the average color
        node.calculateAverage(outputImage);
        outputImage.paintBlockPixel(node.rowStart, node.colStart, node.rowEnd, node.colEnd,
            node.averageR, node.averageG, node.averageB, false);
        return;
    }

    // Merge children
    for (int i = 0; i < 4; i++) {
        mergeNode(*node.children[i], outputImage);
    }
}

// Divide all current divisible leaf nodes per level
void QuadTree::divide(int minBlockWidth, int minBlockHeight, double errorThreshold, ErrorMethod errorMethod) {
    int count;
    count = divideNode(*root, minBlockWidth, minBlockHeight, errorThreshold, errorMethod);
    nodeCount += count;
    treeDepth++;
}

// Merge the current tree into an Image
Image QuadTree::merge() const {
    Image outputImage(image.getWidth(), image.getHeight(), 0, 0, 0);
    mergeNode(*root, outputImage);
    return std::move(outputImage);
}