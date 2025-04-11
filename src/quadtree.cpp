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
    if (rowStart < 0 || colStart < 0 || rowEnd >= image.getHeight() || colEnd >= image.getWidth()) {
        throw std::out_of_range("Block dimensions are out of bounds.");
    }

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
}

// Average calculation that set the averageR, averageG, and averageB attributes
void QuadTreeNode::calculateAverage(const Image& image){
    averageR = 0;
    averageG = 0;
    averageB = 0;
    int count = getArea();
    if (isLeaf) {
        // Each channel iterated separately to optimize cache hit due to CImg data structure
        for (auto it = image.beginBlock(rowStart, colStart, rowEnd, colEnd, Channels::RED);
            it != image.endBlock(rowStart, colStart, rowEnd, colEnd, Channels::RED); ++it) {
            averageR += *it;
        }
        for (auto it = image.beginBlock(rowStart, colStart, rowEnd, colEnd, Channels::GREEN);
        it != image.endBlock(rowStart, colStart, rowEnd, colEnd, Channels::GREEN); ++it) {
            averageG += *it;
        }
        for (auto it = image.beginBlock(rowStart, colStart, rowEnd, colEnd, Channels::BLUE);
        it != image.endBlock(rowStart, colStart, rowEnd, colEnd, Channels::BLUE); ++it) {
            averageB += *it;
        }

        averageR /= count;
        averageG /= count;
        averageB /= count;
    } else {
        // Calculate average by way of weighted average of children
        for (int i = 0; i < 4; i++) {
            if (children[i] == nullptr) {
                throw std::runtime_error("Child node is null.");
            }
            children[i]->calculateAverage(image);
            averageR += children[i]->averageR * children[i]->getArea();
            averageG += children[i]->averageG * children[i]->getArea();
            averageB += children[i]->averageB * children[i]->getArea();
        }
        averageR /= count;
        averageG /= count;
        averageB /= count;
    }

}



/* --------------------------------------------------*/
/* QuadTree */

// Constructor and destructor
QuadTree::QuadTree(const Image& image, int minBlockArea, double errorThreshold, ErrorMethod errorMethod)
    : image(image), nodeCount(1), treeDepth(1), depthOnLastColorCalc(0),
    minBlockArea(minBlockArea), errorThreshold(errorThreshold), errorMethod(errorMethod) {
    root = std::make_unique<QuadTreeNode>(0, 0, image.getHeight()-1, image.getWidth()-1);
    root->calculateError(image, errorMethod);
}
QuadTree::~QuadTree() {}

// Getters
int QuadTree::getNodeCount() const { return nodeCount; }
int QuadTree::getTreeDepth() const { return treeDepth; }

// Calculate average color of all nodes
void QuadTree::calculateAverageColor() const {
    if (root == nullptr) {
        throw std::runtime_error("Root node is null.");
    }
    if (depthOnLastColorCalc == treeDepth) {
        // Average color already calculated
        return;
    }
    // Recursively calculate average color for each node
    root->calculateAverage(image);
}

/* Divide and conquer */

// Divide nodes per level
// Returns the number of nodes divided
int QuadTree::divideNode(QuadTreeNode& node) {
    int count = 0;
    if (!node.isLeaf) {
        // Case 1: Inner node
        // Count how many nodes are created on this subtree
        for (int i = 0; i < 4; i++) {
            if (node.children[i] == nullptr) {
                throw std::runtime_error("Child node is null.");
            }
            count += divideNode(*node.children[i]);
        }
    } else if (!node.isDivisible) {
        // Case 2: Node has been checked to be indivisible
        // Node is ignored
        count = 0;
    } else {
        // Case 3: Leaf node
        // Check if it is divisible
        if (node.getArea() <= minBlockArea) {
            // The node is not larger than the minimum block size
            node.isDivisible = false;
            count = 0;
        } else if ((node.colEnd-node.colStart) * (node.rowEnd-node.rowStart) / 4 < minBlockArea) {
            // If divided, the node will be smaller than the minimum block size
            node.isDivisible = false;
            count = 0;
        } else if (ErrorMetrics::belowThreshold(node.error, errorThreshold, errorMethod)) {
            // The node is below the error threshold/the pixels are similar
            node.isDivisible = false;
            count = 0;
        } else {
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
        
            for (int i = 0; i < 4; i++) {
                if (node.children[i] == nullptr) {
                    throw std::runtime_error("Child node is null.");
                }
                // Calculate error for each child node
                node.children[i]->calculateError(image, errorMethod);
            }
            count = 4;
        }
    
    }
    return count;
}

// Merge nodes. Calculate average RGB value from each leaf node
void QuadTree::mergeNodeDepth(QuadTreeNode& node, Image& outputImage, int depth, bool addBorder) const {
    // depth == 0   : Do nothing
    // depth == 1   : Fill the block with the average color
    // depth > 1    : Merge children nodes if exist
    // depth == -1  : Merge all leaf nodes

    // node.isLeaf is only set to false when children of a node are created

    if (depth == 0) {
        // Do nothing
    } else if (depth == 1 || node.isLeaf) {
        // Fill the block with the average color
        // Average color should already be calculated
        outputImage.paintBlockPixel(node.rowStart, node.colStart, node.rowEnd, node.colEnd,
            node.averageR, node.averageG, node.averageB, addBorder);
    } else if (depth > 1 && !node.isLeaf) {
        // Merge nodes up to a certain depth which may not be leaf nodes
        // Or merge all leaf nodes
        for (int i = 0; i < 4; i++) {
            mergeNodeDepth(*node.children[i], outputImage, depth-1, addBorder);
        }
    } else if (depth == -1 && !node.isLeaf){
        for (int i = 0; i < 4; i++) {
            mergeNodeDepth(*node.children[i], outputImage, depth, addBorder);
        }
    }
}

// Merge nodes on variable error threshold
void QuadTree::mergeNodeThreshold(QuadTreeNode& node, Image& outputImage, double errorThreshold, bool addBorder) const {
    // Blocks that already have low error is immediately merged even if it has children
    if (node.error < errorThreshold || node.isLeaf) {
        // Fill the block with the average color
        // Average color should already be calculated
        outputImage.paintBlockPixel(node.rowStart, node.colStart, node.rowEnd, node.colEnd,
            node.averageR, node.averageG, node.averageB, addBorder);
    } else if (!node.isLeaf) {
        // Merge children nodes
        for (int i = 0; i < 4; i++) {
            mergeNodeThreshold(*node.children[i], outputImage, errorThreshold, addBorder);
        }
    }
}

// Divide all current divisible leaf nodes per level
int QuadTree::divide() {
    int count = divideNode(*root);
    nodeCount += count;
    if (count > 0) { treeDepth++; }
    return count;
}

// Divide until exhaustion
void QuadTree::divideExhaust() {
    // Divide until no more nodes can be divided
    int count;
    do {
        count = divide();
    } while (count > 0 && treeDepth < QUADTREE_MAX_DEPTH);
}

// Merge the current tree into an Image up to a certain depth
Image QuadTree::merge(int depth, bool addBorder) const {
    // Create a copy of the original image
    Image outputImage = image;
    if (depth < -1) {
        throw std::invalid_argument("Depth must be greater than or equal to -1.");
    }
    if (depth > treeDepth) {
        throw std::invalid_argument("Depth must be less than or equal to the current tree depth.");
    }

    calculateAverageColor(); // Calculate average color for each node
    mergeNodeDepth(*root, outputImage, depth, addBorder);
    return outputImage;
}

// Merge with variable error threshold
Image QuadTree::mergeThreshold(double errorThreshold, bool addBorder) const {
    // Create a copy of the original image
    Image outputImage = image;
    if (errorThreshold < 0) {
        throw std::invalid_argument("Error threshold must be greater than or equal to 0.");
    }

    calculateAverageColor(); // Calculate average color for each node
    mergeNodeThreshold(*root, outputImage, errorThreshold, addBorder);
    return outputImage;
}