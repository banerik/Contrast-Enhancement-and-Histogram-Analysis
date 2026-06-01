#include "algorithms.hpp"
#include "metrics.hpp"
#include <cmath>
#include <algorithm>

cv::Mat myLinearStretching(const cv::Mat& image) {
    double minVal, maxVal;
    cv::minMaxLoc(image, &minVal, &maxVal);
    cv::Mat result(image.rows, image.cols, CV_8UC1);
    
    if (maxVal == minVal) {
        result.setTo(0);
        return result;
    }
    
    double scale = 255.0 / (maxVal - minVal);
    for (int r = 0; r < image.rows; ++r) {
        const uchar* srcRow = image.ptr<uchar>(r);
        uchar* dstRow = result.ptr<uchar>(r);
        for (int c = 0; c < image.cols; ++c) {
            dstRow[c] = cv::saturate_cast<uchar>(cvRound((srcRow[c] - minVal) * scale));
        }
    }
    return result;
}

cv::Mat myPercentileStretching(const cv::Mat& image, double lowPercentile, double highPercentile) {
    std::vector<int> hist = myCalculateHistogram(image);
    std::vector<int> cdf = myCalculateCDF(hist);
    int totalPixels = image.rows * image.cols;
    
    int lowLimit = totalPixels * lowPercentile;
    int highLimit = totalPixels * highPercentile;
    
    int minVal = 0, maxVal = 255;
    for (int i = 0; i < 256; ++i) {
        if (cdf[i] >= lowLimit) {
            minVal = i;
            break;
        }
    }
    for (int i = 255; i >= 0; --i) {
        if (cdf[i] <= highLimit) {
            maxVal = i;
            break;
        }
    }
    
    cv::Mat result(image.rows, image.cols, CV_8UC1);
    if (maxVal <= minVal) {
        result.setTo(minVal);
        return result;
    }
    
    double scale = 255.0 / (maxVal - minVal);
    for (int r = 0; r < image.rows; ++r) {
        const uchar* srcRow = image.ptr<uchar>(r);
        uchar* dstRow = result.ptr<uchar>(r);
        for (int c = 0; c < image.cols; ++c) {
            double stretched = (srcRow[c] - minVal) * scale;
            dstRow[c] = cv::saturate_cast<uchar>(cvRound(stretched));
        }
    }
    return result;
}

cv::Mat myEqualizeHist(const cv::Mat& image) {
    std::vector<int> hist = myCalculateHistogram(image);
    std::vector<int> cdf = myCalculateCDF(hist);
    int totalPixels = image.rows * image.cols;
    
    uchar lut[256];
    double scale = 255.0 / totalPixels;
    for (int i = 0; i < 256; ++i) {
        lut[i] = cv::saturate_cast<uchar>(cvRound(cdf[i] * scale));
    }
    
    cv::Mat result(image.rows, image.cols, CV_8UC1);
    for (int r = 0; r < image.rows; ++r) {
        const uchar* srcRow = image.ptr<uchar>(r);
        uchar* dstRow = result.ptr<uchar>(r);
        for (int c = 0; c < image.cols; ++c) {
            dstRow[c] = lut[srcRow[c]];
        }
    }
    return result;
}

cv::Mat myCLAHE(const cv::Mat& image, double clipLimit, cv::Size gridSize) {
    cv::Mat result(image.rows, image.cols, CV_8UC1);
    int rows = image.rows;
    int cols = image.cols;
    int gh = gridSize.height;
    int gw = gridSize.width;
    
    std::vector<int> tile_w(gw), tile_h(gh);
    std::vector<int> tile_x(gw), tile_y(gh);
    
    int rem_x = cols % gw;
    int base_w = cols / gw;
    int curr_x = 0;
    for (int i = 0; i < gw; ++i) {
        tile_x[i] = curr_x;
        tile_w[i] = base_w + (i < rem_x ? 1 : 0);
        curr_x += tile_w[i];
    }
    
    int rem_y = rows % gh;
    int base_h = rows / gh;
    int curr_y = 0;
    for (int i = 0; i < gh; ++i) {
        tile_y[i] = curr_y;
        tile_h[i] = base_h + (i < rem_y ? 1 : 0);
        curr_y += tile_h[i];
    }
    
    std::vector<double> cx(gw), cy(gh);
    for (int i = 0; i < gw; ++i) {
        cx[i] = tile_x[i] + tile_w[i] / 2.0;
    }
    for (int i = 0; i < gh; ++i) {
        cy[i] = tile_y[i] + tile_h[i] / 2.0;
    }
    
    std::vector<std::vector<std::vector<double>>> tileCDFs(gh, std::vector<std::vector<double>>(gw, std::vector<double>(256, 0.0)));
    
    for (int ty = 0; ty < gh; ++ty) {
        for (int tx = 0; tx < gw; ++tx) {
            int tilePixels = tile_w[tx] * tile_h[ty];
            std::vector<int> hist(256, 0);
            
            for (int y = tile_y[ty]; y < tile_y[ty] + tile_h[ty]; ++y) {
                const uchar* row_ptr = image.ptr<uchar>(y);
                for (int x = tile_x[tx]; x < tile_x[tx] + tile_w[tx]; ++x) {
                    hist[row_ptr[x]]++;
                }
            }
            
            int limit = std::max(1, cvRound(clipLimit * tilePixels / 256.0));
            int excess = 0;
            for (int i = 0; i < 256; ++i) {
                if (hist[i] > limit) {
                    excess += (hist[i] - limit);
                    hist[i] = limit;
                }
            }
            
            int incr = excess / 256;
            int upper = limit - incr;
            for (int i = 0; i < 256; ++i) {
                if (hist[i] <= upper) {
                    excess -= incr;
                    hist[i] += incr;
                } else if (hist[i] < limit) {
                    excess -= (limit - hist[i]);
                    hist[i] = limit;
                }
            }
            
            while (excess > 0) {
                for (int i = 0; i < 256 && excess > 0; ++i) {
                    if (hist[i] < limit) {
                        hist[i]++;
                        excess--;
                    }
                }
            }
            
            double sum = 0.0;
            for (int i = 0; i < 256; ++i) {
                sum += hist[i];
                tileCDFs[ty][tx][i] = (sum / tilePixels) * 255.0;
            }
        }
    }
    
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            int tx0 = -1, tx1 = -1;
            if (x < cx[0]) {
                tx0 = 0; tx1 = 0;
            } else if (x >= cx[gw - 1]) {
                tx0 = gw - 1; tx1 = gw - 1;
            } else {
                for (int i = 0; i < gw - 1; ++i) {
                    if (x >= cx[i] && x < cx[i+1]) {
                        tx0 = i;
                        tx1 = i + 1;
                        break;
                    }
                }
            }
            
            int ty0 = -1, ty1 = -1;
            if (y < cy[0]) {
                ty0 = 0; ty1 = 0;
            } else if (y >= cy[gh - 1]) {
                ty0 = gh - 1; ty1 = gh - 1;
            } else {
                for (int i = 0; i < gh - 1; ++i) {
                    if (y >= cy[i] && y < cy[i+1]) {
                        ty0 = i;
                        ty1 = i + 1;
                        break;
                    }
                }
            }
            
            double a = 0.0;
            if (tx0 != tx1) {
                a = (double)(x - cx[tx0]) / (cx[tx1] - cx[tx0]);
            }
            double b = 0.0;
            if (ty0 != ty1) {
                b = (double)(y - cy[ty0]) / (cy[ty1] - cy[ty0]);
            }
            
            int val = image.at<uchar>(y, x);
            double v00 = tileCDFs[ty0][tx0][val];
            double v01 = tileCDFs[ty0][tx1][val];
            double v10 = tileCDFs[ty1][tx0][val];
            double v11 = tileCDFs[ty1][tx1][val];
            
            double interpolated = (1.0 - a) * (1.0 - b) * v00 + 
                                   a * (1.0 - b) * v01 + 
                                   (1.0 - a) * b * v10 + 
                                   a * b * v11;
            result.at<uchar>(y, x) = cv::saturate_cast<uchar>(cvRound(interpolated));
        }
    }
    
    return result;
}

cv::Mat myGammaCorrection(const cv::Mat& image, double gamma) {
    uchar lut[256];
    for (int i = 0; i < 256; ++i) {
        lut[i] = cv::saturate_cast<uchar>(cvRound(255.0 * std::pow(i / 255.0, gamma)));
    }
    
    cv::Mat result(image.rows, image.cols, CV_8UC1);
    for (int r = 0; r < image.rows; ++r) {
        const uchar* srcRow = image.ptr<uchar>(r);
        uchar* dstRow = result.ptr<uchar>(r);
        for (int c = 0; c < image.cols; ++c) {
            dstRow[c] = lut[srcRow[c]];
        }
    }
    return result;
}

cv::Mat myLogTransform(const cv::Mat& image) {
    uchar lut[256];
    double c = 255.0 / std::log(1.0 + 255.0);
    for (int i = 0; i < 256; ++i) {
        lut[i] = cv::saturate_cast<uchar>(cvRound(c * std::log(1.0 + i)));
    }
    
    cv::Mat result(image.rows, image.cols, CV_8UC1);
    for (int r = 0; r < image.rows; ++r) {
        const uchar* srcRow = image.ptr<uchar>(r);
        uchar* dstRow = result.ptr<uchar>(r);
        for (int c = 0; c < image.cols; ++c) {
            dstRow[c] = lut[srcRow[c]];
        }
    }
    return result;
}
