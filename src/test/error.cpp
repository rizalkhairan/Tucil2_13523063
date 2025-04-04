// Error calculation testing

// g++ src/test/error.cpp -o src/test/error
// ./src/test/error

#include "../error.hpp"
#include <iostream>
#include <vector>

int main() {
    std::vector<int> data1 = { 1, 2, 3, 4, 5 };
    std::vector<int> data2 = { 0, 1, 2, 3, 4 };
    std::vector<int> data3 = { 1, 1, 1, 1, 1 };
    std::vector<int> data4 = { 0, 0, 0, 4, 8 };
    std::vector<int> data5 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2};
    std::vector<int> data6 = {};

    // Show data
    std::cout << "Data 1: ";
    for (const auto& val : data1) std::cout << val << " ";
    std::cout << std::endl;
    std::cout << "Data 2: ";
    for (const auto& val : data2) std::cout << val << " ";
    std::cout << std::endl;
    std::cout << "Data 3: ";
    for (const auto& val : data3) std::cout << val << " ";
    std::cout << std::endl;
    std::cout << "Data 4: ";
    for (const auto& val : data4) std::cout << val << " ";
    std::cout << std::endl;
    std::cout << "Data 5: ";
    for (const auto& val : data5) std::cout << val << " ";
    std::cout << std::endl;
    std::cout << "Data 6: ";
    for (const auto& val : data6) std::cout << val << " ";
    std::cout << std::endl;

    // Variance
    double variance = ErrorMetrics::calculateChannelError(ErrorMethod::VARIANCE, data1.begin(), data1.end());
    std::cout << "Variance 1: " << variance << std::endl;

    variance = ErrorMetrics::calculateChannelError(ErrorMethod::VARIANCE, data2.begin(), data2.end());
    std::cout << "Variance 2: " << variance << std::endl;

    variance = ErrorMetrics::calculateChannelError(ErrorMethod::VARIANCE, data3.begin(), data3.end());
    std::cout << "Variance 3: " << variance << std::endl;

    variance = ErrorMetrics::calculateChannelError(ErrorMethod::VARIANCE, data4.begin(), data4.end());
    std::cout << "Variance 4: " << variance << std::endl;

    variance = ErrorMetrics::calculateChannelError(ErrorMethod::VARIANCE, data5.begin(), data5.end());
    std::cout << "Variance 5: " << variance << std::endl;

    variance = ErrorMetrics::calculateChannelError(ErrorMethod::VARIANCE, data6.begin(), data6.end());
    std::cout << "Variance 6: " << variance << std::endl;

    // Mean Absolute Deviation
    double mad = ErrorMetrics::calculateChannelError(ErrorMethod::MEAN_ABSOLUTE_DEVIATION, data1.begin(), data1.end());
    std::cout << "Mean Absolute Deviation 1: " << mad << std::endl;

    mad = ErrorMetrics::calculateChannelError(ErrorMethod::MEAN_ABSOLUTE_DEVIATION, data2.begin(), data2.end());
    std::cout << "Mean Absolute Deviation 2: " << mad << std::endl;

    mad = ErrorMetrics::calculateChannelError(ErrorMethod::MEAN_ABSOLUTE_DEVIATION, data3.begin(), data3.end());
    std::cout << "Mean Absolute Deviation 3: " << mad << std::endl;

    mad = ErrorMetrics::calculateChannelError(ErrorMethod::MEAN_ABSOLUTE_DEVIATION, data4.begin(), data4.end());
    std::cout << "Mean Absolute Deviation 4: " << mad << std::endl;

    mad = ErrorMetrics::calculateChannelError(ErrorMethod::MEAN_ABSOLUTE_DEVIATION, data5.begin(), data5.end());
    std::cout << "Mean Absolute Deviation 5: " << mad << std::endl;

    mad = ErrorMetrics::calculateChannelError(ErrorMethod::MEAN_ABSOLUTE_DEVIATION, data6.begin(), data6.end());
    std::cout << "Mean Absolute Deviation 6: " << mad << std::endl;

    // Max Pixel Difference
    double maxDiff = ErrorMetrics::calculateChannelError(ErrorMethod::MAX_PIXEL_DIFFERENCE, data1.begin(), data1.end());
    std::cout << "Max Pixel Difference 1: " << maxDiff << std::endl;

    maxDiff = ErrorMetrics::calculateChannelError(ErrorMethod::MAX_PIXEL_DIFFERENCE, data2.begin(), data2.end());
    std::cout << "Max Pixel Difference 2: " << maxDiff << std::endl;

    maxDiff = ErrorMetrics::calculateChannelError(ErrorMethod::MAX_PIXEL_DIFFERENCE, data3.begin(), data3.end());
    std::cout << "Max Pixel Difference 3: " << maxDiff << std::endl;

    maxDiff = ErrorMetrics::calculateChannelError(ErrorMethod::MAX_PIXEL_DIFFERENCE, data4.begin(), data4.end());
    std::cout << "Max Pixel Difference 4: " << maxDiff << std::endl;

    maxDiff = ErrorMetrics::calculateChannelError(ErrorMethod::MAX_PIXEL_DIFFERENCE, data5.begin(), data5.end());
    std::cout << "Max Pixel Difference 5: " << maxDiff << std::endl;

    maxDiff = ErrorMetrics::calculateChannelError(ErrorMethod::MAX_PIXEL_DIFFERENCE, data6.begin(), data6.end());
    std::cout << "Max Pixel Difference 6: " << maxDiff << std::endl;

    // Entropy
    double entropy = ErrorMetrics::calculateChannelError(ErrorMethod::ENTROPY, data1.begin(), data1.end());
    std::cout << "Entropy 1: " << entropy << std::endl;

    entropy = ErrorMetrics::calculateChannelError(ErrorMethod::ENTROPY, data2.begin(), data2.end());
    std::cout << "Entropy 2: " << entropy << std::endl;

    entropy = ErrorMetrics::calculateChannelError(ErrorMethod::ENTROPY, data3.begin(), data3.end());
    std::cout << "Entropy 3: " << entropy << std::endl;

    entropy = ErrorMetrics::calculateChannelError(ErrorMethod::ENTROPY, data4.begin(), data4.end());
    std::cout << "Entropy 4: " << entropy << std::endl;

    entropy = ErrorMetrics::calculateChannelError(ErrorMethod::ENTROPY, data5.begin(), data5.end());
    std::cout << "Entropy 5: " << entropy << std::endl;

    entropy = ErrorMetrics::calculateChannelError(ErrorMethod::ENTROPY, data6.begin(), data6.end());
    std::cout << "Entropy 6: " << entropy << std::endl;

    return 0;
}