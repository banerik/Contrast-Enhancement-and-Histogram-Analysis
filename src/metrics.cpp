#include "metrics.hpp"
#include <cmath>

std::vector<int> myCalculateHistogram(const cv::Mat& image) {
    std::vector<int> hist(256, 0);
    for (int r = 0; r < image.rows; ++r) {
        const uchar* row = image.ptr<uchar>(r);
        for (int c = 0; c < image.cols; ++c) {
            hist[row[c]]++;
        }
    }
    return hist;
}

std::vector<int> myCalculateCDF(const std::vector<int>& hist) {
    std::vector<int> cdf(256, 0);
    cdf[0] = hist[0];
    for (size_t i = 1; i < hist.size(); ++i) {
        cdf[i] = cdf[i - 1] + hist[i];
    }
    return cdf;
}

double myCalculateMean(const std::vector<int>& hist, int totalPixels) {
    double sum = 0.0;
    for (int i = 0; i < 256; ++i) {
        sum += i * hist[i];
    }
    return sum / totalPixels;
}

double myCalculateStdDev(const cv::Mat& image, double mean) {
    double varianceSum = 0.0;
    for (int r = 0; r < image.rows; ++r) {
        const uchar* row = image.ptr<uchar>(r);
        for (int c = 0; c < image.cols; ++c) {
            varianceSum += (row[c] - mean) * (row[c] - mean);
        }
    }
    return std::sqrt(varianceSum / (image.rows * image.cols));
}

double myCalculateEntropy(const std::vector<int>& hist, int totalPixels) {
    double entropy = 0.0;
    for (int i = 0; i < 256; ++i) {
        if (hist[i] > 0) {
            double p = (double)hist[i] / totalPixels;
            entropy -= p * std::log2(p);
        }
    }
    return entropy;
}

double getMSE(const cv::Mat& img1, const cv::Mat& img2) {
    if (img1.size() != img2.size() || img1.type() != img2.type()) {
        return -1.0;
    }
    double sumSquareDiff = 0.0;
    for (int r = 0; r < img1.rows; ++r) {
        const uchar* r1 = img1.ptr<uchar>(r);
        const uchar* r2 = img2.ptr<uchar>(r);
        for (int c = 0; c < img1.cols; ++c) {
            double diff = r1[c] - r2[c];
            sumSquareDiff += diff * diff;
        }
    }
    return sumSquareDiff / (img1.rows * img1.cols);
}

double getPSNR(const cv::Mat& img1, const cv::Mat& img2) {
    double mse = getMSE(img1, img2);
    if (mse < 1e-10) return 999.0;
    return 10.0 * std::log10((255.0 * 255.0) / mse);
}
