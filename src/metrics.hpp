#ifndef METRICS_HPP
#define METRICS_HPP

#include <vector>
#include <string>
#include <opencv2/opencv.hpp>

// Structure to store metrics for a specific method
struct MethodMetrics {
    std::string method;
    double mean;
    double stddev;
    double entropy;
    double mse;    // Mean Squared Error vs OpenCV equivalent (where applicable)
    double psnr;   // Peak Signal-to-Noise Ratio vs OpenCV equivalent (where applicable)
};

// Structure to store all metrics for a single image
struct ImageMetrics {
    std::string filename;
    std::vector<MethodMetrics> methods;
};

// Histogram and CDF Manual calculations
std::vector<int> myCalculateHistogram(const cv::Mat& image);
std::vector<int> myCalculateCDF(const std::vector<int>& hist);

// Manual statistical metrics
double myCalculateMean(const std::vector<int>& hist, int totalPixels);
double myCalculateStdDev(const cv::Mat& image, double mean);
double myCalculateEntropy(const std::vector<int>& hist, int totalPixels);

// Error comparison functions
double getMSE(const cv::Mat& img1, const cv::Mat& img2);
double getPSNR(const cv::Mat& img1, const cv::Mat& img2);

#endif // METRICS_HPP
