#ifndef ERROR_HPP
#define ERROR_HPP

#include <map>
#include <cmath>

enum ErrorMethod {
    VARIANCE = 1,
    MEAN_ABSOLUTE_DEVIATION = 2,
    MAX_PIXEL_DIFFERENCE = 3,
    ENTROPY = 4,
    SSIM = 5
};

class ErrorMetrics {
public:
    // Used to calculate the error value of pixels based on various methods
    // Require Iterator object such as the one defined in Image::Iterator
    template <typename Iterator>
    static double calculateChannelError(ErrorMethod method, Iterator begin, Iterator end) {
        if (begin == end) {
            return 0;   // Empty iterator
        }
        switch (method) {
            case VARIANCE:
                return calculateVariance(begin, end);
            case MEAN_ABSOLUTE_DEVIATION:
                return calculateMeanAbsoluteDeviation(begin, end);
            case MAX_PIXEL_DIFFERENCE:
                return calculateMaxPixelDifference(begin, end);
            case ENTROPY:
                return calculateEntropy(begin, end);
            case SSIM:
                return calculateSSIM(begin, end);
            default:
                return 0;
        }
    }

    // Aggregates the error values of each channel into a single value
    static double calculateError(ErrorMethod method, double r, double g, double b) {
        switch (method) {
            case VARIANCE:
            case MEAN_ABSOLUTE_DEVIATION:
            case MAX_PIXEL_DIFFERENCE:
            case ENTROPY:
                return (r + g + b) / 3.0;
            case SSIM:
                return 0.299 * r + 0.587 * g + 0.114 * b;
            default:
                return 0;
        }
    }

    // Check if the error is within a threshold
    static bool belowThreshold(double error, double threshold, ErrorMethod method) {
        switch (method) {
            case VARIANCE:
            case MEAN_ABSOLUTE_DEVIATION:
            case MAX_PIXEL_DIFFERENCE:
            case ENTROPY:
                return error <= threshold;
            case SSIM:
                // For SSIM, we want to check if the error is greater than the threshold
                // The closer to 1, the more similar/better
                return error >= threshold;
            default:
                return true;
        }
    }

    static bool validThreshold(double threshold, ErrorMethod method) {
        switch (method) {
            case VARIANCE:
            case MEAN_ABSOLUTE_DEVIATION:
            case MAX_PIXEL_DIFFERENCE:
            case ENTROPY:
                // Nonegative measure
                return threshold >= 0;
            case SSIM:
                // For SSIM, the threshold should be between -1 and 1
                return threshold >= -1 && threshold <= 1;
            default:
                return false;
        }
    }

private:
    /* Error calculation */
    // Pixels should have unsigned char type

    template <typename Iterator>
    static double calculateVariance(Iterator begin, Iterator end) {
        // Variance error
        // By Var(X) = E[(X-E[X])^2]
        double sum = 0, mean = 0;
        int count = 0;

        // Calculate mean first
        for (auto it = begin; it != end; ++it) {
            sum += *it;
            count++;
        }
        mean = sum / count;

        double variance = 0;
        for (auto it = begin; it != end; ++it) {
            variance += std::pow(*it - mean, 2);
        }
        return variance / count;
        
    }

    template <typename Iterator>
    static double calculateMeanAbsoluteDeviation(Iterator begin, Iterator end) {
        // Mean Absolute Deviation error
        // By MAD(X) = E[|X - E[X]|]
        double sum = 0, mean = 0;
        int count = 0;

        // Calculate mean first
        for (auto it = begin; it != end; ++it) {
            sum += *it;
            count++;
        }
        mean = sum / count;

        double mad = 0;
        for (auto it = begin; it != end; ++it) {
            mad += std::abs(*it - mean);
        }
        return mad / count;
    }

    template <typename Iterator>
    static double calculateMaxPixelDifference(Iterator begin, Iterator end) {
        // Max Pixel Difference error
        // By MaxDiff(X) = max(X) - min(X)
        typename Iterator::value_type min = *begin, max = *begin;
        for (auto it = begin; it != end; ++it) {
            if (*it < min) min = *it;
            if (*it > max) max = *it;
        }
        return max - min;
    }

    template <typename Iterator>
    static double calculateEntropy(Iterator begin, Iterator end) {
        // Entropy error
        // By H = -Σ p(x) * log2(p(x))

        // Calculate frequency of each pixel value for p(x)
        std::map<typename Iterator::value_type, int> histogram;
        int count = 0;
        for (auto it = begin; it != end; ++it) {
            histogram[*it]++;
            count++;
        }
        
        double entropy = 0;
        for (auto& [color, frequency] : histogram) {
            double probability = static_cast<double>(frequency) / count;   // p(x) = frequency / count
            entropy -= probability * std::log2(probability);
        }

        return entropy;
    }

    template <typename Iterator>
    static double calculateSSIM(Iterator begin, Iterator end) {
        // SSIM
        // Value: -1 to 1
        // double C1 = 0.0001 * 65025;
        const double C2 = 0.0009 * 255 * 255;

        // Only the variance of the subblock matter
        double var = calculateVariance(begin, end);

        // Simplified formula
        return C2 / (var + C2);
    }
};

#endif