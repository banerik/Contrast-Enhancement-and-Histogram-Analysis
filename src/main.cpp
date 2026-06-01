#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <opencv2/opencv.hpp>
#include "metrics.hpp"
#include "algorithms.hpp"
#include "utils.hpp"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Utilizare standard: " << argv[0] << " <cale_imagine>" << std::endl;
        std::cout << "Utilizare batch (procesare set complet): " << argv[0] << " --batch" << std::endl;
        return -1;
    }
    
    std::string arg1 = argv[1];
    
    if (arg1 == "--batch" || arg1 == "--all") {
        std::cout << "=== RULARE BATCH | PROCESARE SET DATE COMPLET ===" << std::endl;
        
        std::vector<std::string> imageNames = {
            "afmsurf.bmp", "bacteria.bmp", "cameraman.bmp", "cell.bmp", 
            "eight.bmp", "moon.bmp", "pout.bmp", "saturn.bmp", 
            "westconcordorthophoto.bmp"
        };
        
        std::vector<ImageMetrics> allMetrics;
        
        for (const auto& name : imageNames) {
            std::string fullPath = "../data/" + name;
            std::cout << "Se proceseaza: " << name << " ... " << std::flush;
            ImageMetrics m = processSingleImage(fullPath, name, true);
            if (!m.methods.empty()) {
                allMetrics.push_back(m);
                std::cout << "OK!" << std::endl;
            } else {
                std::cout << "Eroare!" << std::endl;
            }
        }
        
        generateMarkdownReport(allMetrics, "../results/Report.md");
        generateCSVMetrics(allMetrics, "../results/metrics.csv");
        
        std::cout << "Toate operatiunile batch s-au incheiat. Rezultatele sunt disponibile in directorul 'results/'." << std::endl;
    } else {
        std::cout << "=== RULARE INDIVIDUALA | IMAGINE SINGURA ===" << std::endl;
        std::string imagePath = arg1;
        
        cv::Mat original = cv::imread(imagePath, cv::IMREAD_GRAYSCALE);
        if (original.empty()) {
            std::cerr << "Nu s-a putut deschide imaginea: " << imagePath << std::endl;
            return -1;
        }
        
        int totalPixels = original.rows * original.cols;
        
        std::vector<int> origHist = myCalculateHistogram(original);
        cv::Mat stretched = myLinearStretching(original);
        cv::Mat equalized = myEqualizeHist(original);
        cv::Mat claheResult = myCLAHE(original, 4.0, cv::Size(8, 8));
        cv::Mat gammaResult = myGammaCorrection(original, 0.6);
        cv::Mat logResult = myLogTransform(original);
        
        cv::Mat histOrig = drawHistogramAndCDF(origHist, myCalculateCDF(origHist), totalPixels);
        cv::Mat histStretched = drawHistogramAndCDF(myCalculateHistogram(stretched), myCalculateCDF(myCalculateHistogram(stretched)), totalPixels);
        cv::Mat histEqualized = drawHistogramAndCDF(myCalculateHistogram(equalized), myCalculateCDF(myCalculateHistogram(equalized)), totalPixels);
        cv::Mat histClahe = drawHistogramAndCDF(myCalculateHistogram(claheResult), myCalculateCDF(myCalculateHistogram(claheResult)), totalPixels);
        
        std::cout << "\n--- Comparatie Performanta (Validare vs OpenCV) ---" << std::endl;
        
        cv::Mat cvStretched, cvEqualized, cvClahe;
        cv::normalize(original, cvStretched, 0, 255, cv::NORM_MINMAX);
        cv::equalizeHist(original, cvEqualized);
        cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(4.0, cv::Size(8,8));
        clahe->apply(original, cvClahe);
        
        std::cout << "Metoda\t\t\tMean\t\tStdDev\t\tEntropy\t\tMSE (vs OpenCV)\tPSNR (vs OpenCV)" << std::endl;
        
        auto printRow = [&](const std::string& name, const cv::Mat& img, const cv::Mat& ref = cv::Mat()) {
            std::vector<int> h = myCalculateHistogram(img);
            double m = myCalculateMean(h, totalPixels);
            double sd = myCalculateStdDev(img, m);
            double ent = myCalculateEntropy(h, totalPixels);
            
            std::cout << name << "\t\t" 
                      << std::fixed << std::setprecision(2) << m << "\t\t" 
                      << sd << "\t\t" 
                      << ent;
            if (!ref.empty()) {
                double mse = getMSE(img, ref);
                double psnr = getPSNR(img, ref);
                std::cout << "\t\t" << std::scientific << std::setprecision(4) << mse << "\t\t" 
                          << std::fixed << std::setprecision(2) << (psnr > 900 ? "Infinity" : std::to_string(psnr) + " dB");
            } else {
                std::cout << "\t\tN/A\t\tN/A";
            }
            std::cout << std::endl;
        };
        
        printRow("Original", original);
        printRow("Stretching", stretched, cvStretched);
        printRow("Equalization", equalized, cvEqualized);
        printRow("CLAHE", claheResult, cvClahe);
        printRow("Gamma (0.6)", gammaResult);
        printRow("Logarithmic", logResult);
        
        cv::imshow("Original Image", original);
        cv::imshow("Original Hist & CDF", histOrig);
        
        cv::imshow("Linear Stretching (Manual)", stretched);
        cv::imshow("Stretched Hist & CDF", histStretched);
        
        cv::imshow("Histogram Equalization (Manual)", equalized);
        cv::imshow("Equalized Hist & CDF", histEqualized);
        
        cv::imshow("CLAHE (Manual)", claheResult);
        cv::imshow("CLAHE Hist & CDF", histClahe);
        
        cv::imshow("Gamma Correction (Manual, g=0.6)", gammaResult);
        cv::imshow("Logarithmic Transform (Manual)", logResult);
        
        std::cout << "\nApasati orice tasta intr-o fereastra de imagine pentru a iesi..." << std::endl;
        cv::waitKey(0);
    }
    
    return 0;
}
