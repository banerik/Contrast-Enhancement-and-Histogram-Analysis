#include "utils.hpp"
#include "algorithms.hpp"
#include "metrics.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>

cv::Mat drawHistogramAndCDF(const std::vector<int>& hist, const std::vector<int>& cdf, int totalPixels) {
    int hist_w = 256, hist_h = 200;
    cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(245, 245, 245));
    
    int maxVal = 0;
    for (int i = 0; i < 256; ++i) {
        if (hist[i] > maxVal) maxVal = hist[i];
    }
    if (maxVal == 0) maxVal = 1;
    
    for (int i = 0; i < 256; ++i) {
        int h_val = cvRound(((double)hist[i] / maxVal) * (hist_h - 20));
        cv::line(histImage, 
                 cv::Point(i, hist_h), 
                 cv::Point(i, hist_h - h_val), 
                 cv::Scalar(120, 120, 120), 1);
    }
    
    for (int i = 1; i < 256; ++i) {
        int y1 = hist_h - cvRound(((double)cdf[i-1] / totalPixels) * (hist_h - 20));
        int y2 = hist_h - cvRound(((double)cdf[i] / totalPixels) * (hist_h - 20));
        cv::line(histImage, 
                 cv::Point(i - 1, y1), 
                 cv::Point(i, y2), 
                 cv::Scalar(0, 0, 200), 1, cv::LINE_AA);
    }
    
    return histImage;
}

cv::Mat addLabel(const cv::Mat& src, const std::string& label) {
    cv::Mat dst;
    if (src.channels() == 1) {
        cv::cvtColor(src, dst, cv::COLOR_GRAY2BGR);
    } else {
        dst = src.clone();
    }
    
    int baseLine = 0;
    cv::Size textSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.45, 1, &baseLine);
    cv::Rect box(5, 5, textSize.width + 10, textSize.height + 10);
    
    cv::Mat roi = dst(box);
    double alpha = 0.65;
    cv::Mat color(roi.size(), roi.type(), cv::Scalar(0, 0, 0));
    cv::addWeighted(color, alpha, roi, 1.0 - alpha, 0.0, roi);
    
    cv::putText(dst, label, cv::Point(10, 5 + textSize.height + 3), 
                cv::FONT_HERSHEY_SIMPLEX, 0.45, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
    
    return dst;
}

MethodMetrics computeMethodMetrics(const std::string& name, const cv::Mat& enhanced, const std::vector<int>& hist, int totalPixels, const cv::Mat& cvRef) {
    MethodMetrics m;
    m.method = name;
    m.mean = myCalculateMean(hist, totalPixels);
    m.stddev = myCalculateStdDev(enhanced, m.mean);
    m.entropy = myCalculateEntropy(hist, totalPixels);
    
    if (!cvRef.empty()) {
        m.mse = getMSE(enhanced, cvRef);
        m.psnr = getPSNR(enhanced, cvRef);
    } else {
        m.mse = 0.0;
        m.psnr = 999.0;
    }
    return m;
}

void generateMarkdownReport(const std::vector<ImageMetrics>& allMetrics, const std::string& outputPath) {
    std::ofstream out(outputPath);
    if (!out.is_open()) {
        std::cerr << "Nu s-a putut crea fisierul de raport: " << outputPath << std::endl;
        return;
    }
    
    out << "# Proiect: Îmbunătățirea Contrastului și Analiza Histogramelor (PI)\n\n";
    out << "Acest raport a fost generat automat în urma procesării setului de date. Proiectul conține implementări **de la zero** (fără funcții de bibliotecă pentru nucleul algoritmilor) ale tehnicilor de analiză și îmbunătățire a contrastului, conform cerințelor academice.\n\n";
    
    out << "## 1. Introducere Teoretică\n\n";
    out << "### Analiza Histogramei\n";
    out << "Histograma unei imagini grayscale reprezintă distribuția frecvenței de apariție a intensităților de gri ($[0, 255]$). ";
    out << "Analiza histogramei oferă informații despre luminozitate (brighness), contrast global și detalii:\n";
    out << "- **Media ($\\mu$):** Indică luminozitatea medie. Valori mici înseamnă imagini întunecate, valori mari înseamnă imagini supraexpuse.\n";
    out << "  $$\\mu = \\frac{1}{M \\times N} \\sum_{x=0}^{M-1} \\sum_{y=0}^{N-1} I(x,y)$$\n";
    out << "- **Deviația Standard (Standard Deviation - $\\sigma$):** Indică contrastul imaginii. O deviație standard mai mare arată o distribuție mai largă a intensităților, adică un contrast mai ridicat.\n";
    out << "  $$\\sigma = \\sqrt{\\frac{1}{M \\times N} \\sum_{x=0}^{M-1} \\sum_{y=0}^{N-1} (I(x,y) - \\mu)^2}$$\n";
    out << "- **Entropia (Entropy - $H$):** Măsoară quantitatea de informație și detaliile texturale. O entropie ridicată indică detalii vizuale mai bogate.\n";
    out << "  $$H = -\\sum_{i=0}^{255} p(i) \\log_2 p(i), \\quad p(i) = \\frac{\\text{hist}[i]}{M \\times N}$$\n\n";
    
    out << "### Tehnici de Îmbunătățire a Contrastului\n";
    out << "1. **Linear Contrast Stretching (Întindere Liniară):** Redistribuie liniar valorile pixelilor din intervalul curent $[I_{\\min}, I_{\\max}]$ în intervalul complet $[0, 255]$.\n";
    out << "   $$O(x,y) = \\text{round}\\left( \\frac{I(x,y) - I_{\\min}}{I_{\\max} - I_{\\min}} \\times 255 \\right)$$\n";
    out << "2. **Histogram Equalization (Egalizarea Histogramei - HE):** Flatează histograma prin maparea pixelilor utilizând funcția de distribuție cumulativă (CDF) normalizată. Oferă îmbunătățire globală a contrastului, dar poate amplifica zgomotul.\n";
    out << "   $$LUT[v] = \\text{round}\\left( \\frac{CDF[v]}{\\text{totalPixels}} \\times 255 \\right)$$\n";
    out << "3. **CLAHE (Contrast Limited Adaptive Histogram Equalization):** Îmbunătățește contrastul local împărțind imaginea în sub-regiuni (plăci de dimensiune $8 \\times 8$). Contrastul este limitat prin tăierea histogramelor locale (clipping) și redistribuirea pixelilor peste limită, prevenind zgomotul excesiv. Tranzitul dintre plăci se face prin interpolare biliniară.\n";
    out << "4. **Corecția Gamma (Power-Law):** O metodă neliniară de reglare a luminozității și contrastului.\n";
    out << "   $$S = 255 \\times \\left(\\frac{R}{255}\\right)^\\gamma$$\n";
    out << "5. **Transformarea Logaritmică:** Extinde detaliile din zonele întunecate în detrimentul zonelor foarte luminoase.\n";
    out << "   $$S = c \\times \\log(1 + R), \\quad c = \\frac{255}{\\log(256)}$$\n\n";
    
    out << "## 2. Validarea Implementării Proprii\n";
    out << "Pentru validarea codului implementat manual, rezultatele au fost comparate cu implementările oficiale din biblioteca OpenCV, utilizând metricile **MSE** (Mean Squared Error) și **PSNR** (Peak Signal-to-Noise Ratio). ";
    out << "Valori ale MSE extrem de mici (apropiate de 0) sau valori PSNR mari (peste 50 dB) demonstrează corectitudinea implementării matematice a algoritmilor.\n\n";
    
    out << "## 3. Rezultate Obținute și Metrici Statistice\n\n";
    
    out << "| Imagine | Metodă | Media (Brightness) | Dev. Std (Contrast) | Entropie (Detalii) | MSE vs OpenCV | PSNR vs OpenCV |\n";
    out << "| :--- | :--- | :---: | :---: | :---: | :---: | :---: |\n";
    
    for (const auto& img : allMetrics) {
        bool first = true;
        for (const auto& method : img.methods) {
            out << "| " << (first ? "**" + img.filename + "**" : "") << " | " 
                << method.method << " | "
                << std::fixed << std::setprecision(2) << method.mean << " | "
                << method.stddev << " | "
                << method.entropy << " | ";
            
            if (method.method == "Linear Stretch" || method.method == "Equalization" || method.method == "CLAHE") {
                out << std::scientific << std::setprecision(4) << method.mse << " | "
                    << std::fixed << std::setprecision(2) << (method.psnr > 900 ? "Infinity" : std::to_string(method.psnr) + " dB") << " |\n";
            } else {
                out << "N/A | N/A |\n";
            }
            first = false;
        }
        out << "| --- | --- | --- | --- | --- | --- | --- |\n";
    }
    
    out << "\n## 4. Analiză și Concluzii\n\n";
    out << "În urma rulării metodelor pe imaginile de test, se desprind următoarele observații:\n\n";
    out << "1. **Egalizarea Histogramei (HE)** oferă cea mai mare creștere a deviației standard (contrast global), dar în imagini precum `cameraman.bmp` sau `pout.bmp`, fundalul sau detaliile fine devin nenaturale, iar zgomotul este amplificat vizibil.\n";
    out << "2. **CLAHE** rezolvă excelent această problemă. Prin împărțirea imaginii în ferestre locale și aplicarea limitării de contrast, zgomotul nu este amplificat artificial. De exemplu, în cazul imaginilor microscopice precum `bacteria.bmp` sau `cell.bmp`, CLAHE scoate în evidență elementele structurale într-un mod natural și foarte detaliat (entropia se menține mare, iar detaliile locale devin extrem de clare).\n";
    out << "3. **Linear Stretching** este eficient pentru imagini care au un contrast bun dar nu folosesc întreaga gamă dinamică $[0, 255]$. Pentru imagini care au deja pixeli la extreme (zgomot sau fundal alb/negru curat), stretching-ul liniar clasic nu aduce modificări semnificative (MSE = 0).\n";
    out << "4. **Corecția Gamma (cu $\\gamma = 0.6$)** îmbunătățește considerabil imaginile subexpuse (întunecate) precum `saturn.bmp` sau `moon.bmp`, crescând vizibilitatea elementelor din zonele de umbră fără a satura zonele luminoase, spre deosebire de transformarea logaritmică care distorsionează culorile prea mult.\n";
    out << "5. **Transformarea Logaritmică** comprimă dinamică luminilor și dilată umbrele. Este ideală pentru analiza de detalii în imagini cu contrast foarte mare în zonele întunecate (cum ar fi spectrograme sau imagini din microscop).\n";
    
    out.close();
    std::cout << "Raportul a fost generat cu succes in: " << outputPath << std::endl;
}

void generateCSVMetrics(const std::vector<ImageMetrics>& allMetrics, const std::string& outputPath) {
    std::ofstream out(outputPath);
    if (!out.is_open()) {
        std::cerr << "Nu s-a putut crea fisierul CSV: " << outputPath << std::endl;
        return;
    }
    
    out << "Imagine,Metoda,Mean,StdDev,Entropy,MSE_vs_OpenCV,PSNR_vs_OpenCV\n";
    for (const auto& img : allMetrics) {
        for (const auto& method : img.methods) {
            out << img.filename << ","
                << method.method << ","
                << method.mean << ","
                << method.stddev << ","
                << method.entropy << ","
                << method.mse << ","
                << method.psnr << "\n";
        }
    }
    out.close();
    std::cout << "Fisierul CSV cu metrici a fost salvat in: " << outputPath << std::endl;
}

ImageMetrics processSingleImage(const std::string& inputPath, const std::string& name, bool saveResults) {
    ImageMetrics metrics;
    metrics.filename = name;
    
    cv::Mat original = cv::imread(inputPath, cv::IMREAD_GRAYSCALE);
    if (original.empty()) {
        std::cerr << "Eroare la incarcarea imaginii: " << inputPath << std::endl;
        return metrics;
    }
    
    int totalPixels = original.rows * original.cols;
    
    std::vector<int> origHist = myCalculateHistogram(original);
    std::vector<int> origCDF = myCalculateCDF(origHist);
    metrics.methods.push_back(computeMethodMetrics("Original", original, origHist, totalPixels));
    
    cv::Mat stretched = myLinearStretching(original);
    std::vector<int> stretchHist = myCalculateHistogram(stretched);
    
    cv::Mat cvStretched;
    cv::normalize(original, cvStretched, 0, 255, cv::NORM_MINMAX);
    metrics.methods.push_back(computeMethodMetrics("Linear Stretch", stretched, stretchHist, totalPixels, cvStretched));
    
    cv::Mat equalized = myEqualizeHist(original);
    std::vector<int> eqHist = myCalculateHistogram(equalized);
    
    cv::Mat cvEqualized;
    cv::equalizeHist(original, cvEqualized);
    metrics.methods.push_back(computeMethodMetrics("Equalization", equalized, eqHist, totalPixels, cvEqualized));
    
    cv::Mat claheResult = myCLAHE(original, 4.0, cv::Size(8, 8));
    std::vector<int> claheHist = myCalculateHistogram(claheResult);
    
    cv::Mat cvClahe;
    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
    clahe->setClipLimit(4.0);
    clahe->setTilesGridSize(cv::Size(8, 8));
    clahe->apply(original, cvClahe);
    metrics.methods.push_back(computeMethodMetrics("CLAHE", claheResult, claheHist, totalPixels, cvClahe));
    
    double gamma = 0.6;
    cv::Mat gammaResult = myGammaCorrection(original, gamma);
    std::vector<int> gammaHist = myCalculateHistogram(gammaResult);
    metrics.methods.push_back(computeMethodMetrics("Gamma Correction (g=0.6)", gammaResult, gammaHist, totalPixels));
    
    cv::Mat logResult = myLogTransform(original);
    std::vector<int> logHist = myCalculateHistogram(logResult);
    metrics.methods.push_back(computeMethodMetrics("Logarithmic", logResult, logHist, totalPixels));
    
    if (saveResults) {
        cv::Size gridCellSize(256, 256);
        cv::Mat rOrig, rStretch, rEq, rClahe, rGamma, rLog;
        
        cv::resize(original, rOrig, gridCellSize);
        cv::resize(stretched, rStretch, gridCellSize);
        cv::resize(equalized, rEq, gridCellSize);
        cv::resize(claheResult, rClahe, gridCellSize);
        cv::resize(gammaResult, rGamma, gridCellSize);
        cv::resize(logResult, rLog, gridCellSize);
        
        cv::Mat grid1 = addLabel(rOrig, "Original");
        cv::Mat grid2 = addLabel(rStretch, "Linear Stretch");
        cv::Mat grid3 = addLabel(rEq, "Equalized");
        cv::Mat grid4 = addLabel(rClahe, "CLAHE (8x8)");
        cv::Mat grid5 = addLabel(rGamma, "Gamma (g=0.6)");
        cv::Mat grid6 = addLabel(rLog, "Logarithmic");
        
        cv::Mat row1, row2, finalGrid;
        cv::hconcat(std::vector<cv::Mat>{grid1, grid2, grid3}, row1);
        cv::hconcat(std::vector<cv::Mat>{grid4, grid5, grid6}, row2);
        cv::vconcat(row1, row2, finalGrid);
        
        std::string outImgPath = "../results/" + name + "_comparison.png";
        cv::imwrite(outImgPath, finalGrid);
        
        cv::Mat hOrig = drawHistogramAndCDF(origHist, origCDF, totalPixels);
        cv::Mat hStretch = drawHistogramAndCDF(stretchHist, myCalculateCDF(stretchHist), totalPixels);
        cv::Mat hEq = drawHistogramAndCDF(eqHist, myCalculateCDF(eqHist), totalPixels);
        cv::Mat hClahe = drawHistogramAndCDF(claheHist, myCalculateCDF(claheHist), totalPixels);
        cv::Mat hGamma = drawHistogramAndCDF(gammaHist, myCalculateCDF(gammaHist), totalPixels);
        cv::Mat hLog = drawHistogramAndCDF(logHist, myCalculateCDF(logHist), totalPixels);
        
        cv::Mat hGrid1 = addLabel(hOrig, "Original Hist / CDF");
        cv::Mat hGrid2 = addLabel(hStretch, "Ststretched Hist / CDF");
        cv::Mat hGrid3 = addLabel(hEq, "Equalized Hist / CDF");
        cv::Mat hGrid4 = addLabel(hClahe, "CLAHE Hist / CDF");
        cv::Mat hGrid5 = addLabel(hGamma, "Gamma Hist / CDF");
        cv::Mat hGrid6 = addLabel(hLog, "Log Hist / CDF");
        
        cv::Mat hRow1, hRow2, finalHistGrid;
        cv::hconcat(std::vector<cv::Mat>{hGrid1, hGrid2, hGrid3}, hRow1);
        cv::hconcat(std::vector<cv::Mat>{hGrid4, hGrid5, hGrid6}, hRow2);
        cv::vconcat(hRow1, hRow2, finalHistGrid);
        
        std::string outHistPath = "../results/" + name + "_histograms.png";
        cv::imwrite(outHistPath, finalHistGrid);
    }
    
    return metrics;
}
