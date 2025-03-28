#ifndef ERROR_HPP
#define ERROR_HPP

#include <iterator>
#include <algorithm>
#include <map>
#include <cmath>

enum ErrorMethod {
    VARIANCE = 1,
    MEAN_ABSOLUTE_DEVIATION = 2,
    MAX_PIXEL_DIFFERENCE = 3,
    ENTROPY = 4
};

class ErrorMetrics {
public:
    // Used to calculate the error value of pixels based on various methods
    // Require Iterator object such as the one defined in Image::Iterator
    template <typename Iterator>
    static double calculateChannelError(ErrorMethod method, Iterator begin, Iterator end) {
        switch (method) {
            case VARIANCE:
                return calculateVariance(begin, end);
            case MEAN_ABSOLUTE_DEVIATION:
                return calculateMeanAbsoluteDeviation(begin, end);
            case MAX_PIXEL_DIFFERENCE:
                return calculateMaxPixelDifference(begin, end);
            case ENTROPY:
                return calculateEntropy(begin, end);
            default:
                return 0;
        }
    }

    // Aggregates the error values of each channel into a single value
    static double calculateError(ErrorMethod method, double r, double g, double b) {
        return (r + g + b) / 3;
    }

private:
    /* Error calculation */
    // Pixels should have unsigned char type

    template <typename Iterator>
    static double calculateVariance(Iterator begin, Iterator end) {
        // Variance error
        // By Var(X) = E[X^2] - E[X]^2
        double sum = 0, sumSquared = 0;
        long count = 0;
        
        for (auto it = begin; it != end; ++it) {
            sum += *it;
            sumSquared += (*it) * (*it);
            count++;
        }
        
        if (count == 0) return 0;

        double mean = sum / count;
        return (sumSquared / count) - (mean * mean);
    }

    template <typename Iterator>
    static double calculateMeanAbsoluteDeviation(Iterator begin, Iterator end) {
        // Mean Absolute Deviation error
        // By MAD(X) = E[|X - E[X]|]
        double sum = 0, mean = 0;
        long count = 0;

        // Calculate mean first
        for (auto it = begin; it != end; ++it) {
            sum += *it;
            count++;
        }
        if (count == 0) return 0;
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
        if (begin == end) return 0;

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
        std::map <typename Iterator::value_type, double> frequency;
        long count = 0;
        for (auto it = begin; it != end; ++it) {
            frequency[*it]++;
            count++;
        }

        if (count == 0) return 0;
        
        double entropy = 0;
        for (auto &pair : frequency) {
            double probability = pair.second / count;   // p(x) = frequency / count
            entropy -= probability * std::log2(probability);
        }

        return entropy;
    }
};

#endif