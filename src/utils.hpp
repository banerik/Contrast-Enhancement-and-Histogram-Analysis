#ifndef UTILS_HPP
#define UTILS_HPP

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include "metrics.hpp"

// Utility functions for reporting, CSV generation, and visual comparison grids
cv::Mat drawHistogramAndCDF(const std::vector<int>& hist, const std::vector<int>& cdf, int totalPixels);
cv::Mat addLabel(const cv::Mat& src, const std::string& label);

MethodMetrics computeMethodMetrics(const std::string& name, const cv::Mat& enhanced, const std::vector<int>& hist, int totalPixels, const cv::Mat& cvRef = cv::Mat());

void generateMarkdownReport(const std::vector<ImageMetrics>& allMetrics, const std::string& outputPath);
void generateCSVMetrics(const std::vector<ImageMetrics>& allMetrics, const std::string& outputPath);

ImageMetrics processSingleImage(const std::string& inputPath, const std::string& name, bool saveResults);

#endif // UTILS_HPP
