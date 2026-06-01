#ifndef ALGORITHMS_HPP
#define ALGORITHMS_HPP

#include <opencv2/opencv.hpp>

// Manual Contrast Enhancement algorithms
cv::Mat myLinearStretching(const cv::Mat& image);
cv::Mat myPercentileStretching(const cv::Mat& image, double lowPercentile = 0.01, double highPercentile = 0.99);
cv::Mat myEqualizeHist(const cv::Mat& image);
cv::Mat myCLAHE(const cv::Mat& image, double clipLimit = 4.0, cv::Size gridSize = cv::Size(8, 8));
cv::Mat myGammaCorrection(const cv::Mat& image, double gamma);
cv::Mat myLogTransform(const cv::Mat& image);

#endif // ALGORITHMS_HPP
