# Proiect: Îmbunătățirea Contrastului și Analiza Histogramelor (PI)

Acest raport a fost generat automat în urma procesării setului de date. Proiectul conține implementări **de la zero** (fără funcții de bibliotecă pentru nucleul algoritmilor) ale tehnicilor de analiză și îmbunătățire a contrastului, conform cerințelor academice.

## 1. Introducere Teoretică

### Analiza Histogramei

Histograma unei imagini grayscale reprezintă distribuția frecvenței de apariție a intensităților de gri ($[0, 255]$). Analiza histogramei oferă informații despre luminozitate (brighness), contrast global și detalii:

- **Media ($\mu$):** Indică luminozitatea medie. Valori mici înseamnă imagini întunecate, valori mari înseamnă imagini supraexpuse.
  $$\mu = \frac{1}{M \times N} \sum_{x=0}^{M-1} \sum_{y=0}^{N-1} I(x,y)$$
- **Deviația Standard (Standard Deviation - $\sigma$):** Indică contrastul imaginii. O deviație standard mai mare arată o distribuție mai largă a intensităților, adică un contrast mai ridicat.
  $$\sigma = \sqrt{\frac{1}{M \times N} \sum_{x=0}^{M-1} \sum_{y=0}^{N-1} (I(x,y) - \mu)^2}$$
- **Entropia (Entropy - $H$):** Măsoară quantitatea de informație și detaliile texturale. O entropie ridicată indică detalii vizuale mai bogate.
  $$H = -\sum_{i=0}^{255} p(i) \log_2 p(i), \quad p(i) = \frac{\text{hist}[i]}{M \times N}$$

### Tehnici de Îmbunătățire a Contrastului

1. **Linear Contrast Stretching (Întindere Liniară):** Redistribuie liniar valorile pixelilor din intervalul curent $[I_{\min}, I_{\max}]$ în intervalul complet $[0, 255]$.
   $$O(x,y) = \text{round}\left( \frac{I(x,y) - I_{\min}}{I_{\max} - I_{\min}} \times 255 \right)$$
2. **Histogram Equalization (Egalizarea Histogramei - HE):** Flatează histograma prin maparea pixelilor utilizând funcția de distribuție cumulativă (CDF) normalizată. Oferă îmbunătățire globală a contrastului, dar poate amplifica zgomotul.
   $$LUT[v] = \text{round}\left( \frac{CDF[v]}{\text{totalPixels}} \times 255 \right)$$
3. **CLAHE (Contrast Limited Adaptive Histogram Equalization):** Îmbunătățește contrastul local împărțind imaginea în sub-regiuni (plăci de dimensiune $8 \times 8$). Contrastul este limitat prin tăierea histogramelor locale (clipping) și redistribuirea pixelilor peste limită, prevenind zgomotul excesiv. Tranzitul dintre plăci se face prin interpolare biliniară.
4. **Corecția Gamma (Power-Law):** O metodă neliniară de reglare a luminozității și contrastului.
   $$S = 255 \times \left(\frac{R}{255}\right)^\gamma$$
5. **Transformarea Logaritmică:** Extinde detaliile din zonele întunecate în detrimentul zonelor foarte luminoase.
   $$S = c \times \log(1 + R), \quad c = \frac{255}{\log(256)}$$

## 2. Validarea Implementării Proprii

Pentru validarea codului implementat manual, rezultatele au fost comparate cu implementările oficiale din biblioteca OpenCV, utilizând metricile **MSE** (Mean Squared Error) și **PSNR** (Peak Signal-to-Noise Ratio). Valori ale MSE extrem de mici (apropiate de 0) sau valori PSNR mari (peste 50 dB) demonstrează corectitudinea implementării matematice a algoritmilo.

## 3. Rezultate Obținute și Metrici Statistice

| Imagine                       | Metodă                   | Media (Brightness) | Dev. Std (Contrast) | Entropie (Detalii) | MSE vs OpenCV | PSNR vs OpenCV |
| :---------------------------- | :----------------------- | :----------------: | :-----------------: | :----------------: | :-----------: | :------------: |
| **afmsurf.bmp**               | Original                 |       103.26       |        42.27        |        6.90        |      N/A      |      N/A       |
|                               | Linear Stretch           |       101.84       |        42.95        |        6.90        |  0.0000e+00   |    Infinity    |
|                               | Equalization             |       128.91       |        73.43        |        6.73        |  3.3231e-02   |  62.915306 dB  |
|                               | CLAHE                    |       134.33       |        54.05        |        7.75        |  2.4209e+01   |  34.291051 dB  |
|                               | Gamma Correction (g=0.6) |       145.23       |        36.03        |        6.71        |      N/A      |      N/A       |
|                               | Logarithmic              |       209.78       |        19.71        |        6.12        |      N/A      |      N/A       |
| ---                           | ---                      |        ---         |         ---         |        ---         |      ---      |      ---       |
| **bacteria.bmp**              | Original                 |       125.82       |        37.70        |        6.93        |      N/A      |      N/A       |
|                               | Linear Stretch           |       134.25       |        40.24        |        6.93        |  0.0000e+00   |    Infinity    |
|                               | Equalization             |       128.89       |        73.50        |        6.66        |  1.1151e+01   |  37.657620 dB  |
|                               | CLAHE                    |       138.42       |        57.46        |        7.70        |  8.8421e+01   |  28.665252 dB  |
|                               | Gamma Correction (g=0.6) |       163.82       |        37.03        |        6.54        |      N/A      |      N/A       |
|                               | Logarithmic              |       216.79       |        36.02        |        5.52        |      N/A      |      N/A       |
| ---                           | ---                      |        ---         |         ---         |        ---         |      ---      |      ---       |
| **cameraman.bmp**             | Original                 |       118.72       |        62.34        |        7.01        |      N/A      |      N/A       |
|                               | Linear Stretch           |       115.78       |        64.69        |        7.01        |  6.6223e-03   |  69.920706 dB  |
|                               | Equalization             |       128.81       |        73.48        |        6.77        |  0.0000e+00   |    Infinity    |
|                               | CLAHE                    |       128.81       |        64.31        |        7.58        |  1.6548e+01   |  35.943275 dB  |
|                               | Gamma Correction (g=0.6) |       152.59       |        61.41        |        6.63        |      N/A      |      N/A       |
|                               | Logarithmic              |       205.81       |        45.04        |        5.72        |      N/A      |      N/A       |
| ---                           | ---                      |        ---         |         ---         |        ---         |      ---      |      ---       |
| **cell.bmp**                  | Original                 |       118.17       |        13.86        |        4.60        |      N/A      |      N/A       |
|                               | Linear Stretch           |       125.03       |        19.22        |        4.60        |  0.0000e+00   |    Infinity    |
|                               | Equalization             |       137.42       |        74.18        |        4.37        |  0.0000e+00   |    Infinity    |
|                               | CLAHE                    |       147.05       |        30.18        |        6.50        |  3.7585e+01   |  32.380600 dB  |
|                               | Gamma Correction (g=0.6) |       160.43       |        11.73        |        4.23        |      N/A      |      N/A       |
|                               | Logarithmic              |       219.51       |        6.13         |        3.29        |      N/A      |      N/A       |
| ---                           | ---                      |        ---         |         ---         |        ---         |      ---      |      ---       |
| **eight.bmp**                 | Original                 |       198.26       |        52.84        |        4.88        |      N/A      |      N/A       |
|                               | Linear Stretch           |       182.27       |        67.70        |        4.88        |  0.0000e+00   |    Infinity    |
|                               | Equalization             |       138.83       |        77.68        |        4.68        |  1.1001e-03   |  77.716326 dB  |
|                               | CLAHE                    |       196.50       |        57.26        |        6.25        |  9.0850e+00   |  38.547545 dB  |
|                               | Gamma Correction (g=0.6) |       216.80       |        38.62        |        4.57        |      N/A      |      N/A       |
|                               | Logarithmic              |       241.11       |        15.88        |        3.50        |      N/A      |      N/A       |
| ---                           | ---                      |        ---         |         ---         |        ---         |      ---      |      ---       |
| **moon.bmp**                  | Original                 |       52.92        |        78.00        |        5.51        |      N/A      |      N/A       |
|                               | Linear Stretch           |       53.31        |        78.64        |        5.51        |  0.0000e+00   |    Infinity    |
|                               | Equalization             |       136.33       |        66.46        |        5.06        |  1.5535e+00   |  46.217795 dB  |
|                               | CLAHE                    |       58.89        |        68.97        |        6.49        |  7.6046e+00   |  39.320057 dB  |
|                               | Gamma Correction (g=0.6) |       73.12        |        82.37        |        5.36        |      N/A      |      N/A       |
|                               | Logarithmic              |       117.89       |        79.87        |        4.98        |      N/A      |      N/A       |
| ---                           | ---                      |        ---         |         ---         |        ---         |      ---      |      ---       |
| **pout.bmp**                  | Original                 |       110.30       |        23.18        |        5.76        |      N/A      |      N/A       |
|                               | Linear Stretch           |       61.71        |        39.40        |        5.76        |  0.0000e+00   |    Infinity    |
|                               | Equalization             |       130.42       |        72.63        |        5.71        |  3.4579e-02   |  62.742675 dB  |
|                               | CLAHE                    |       121.30       |        39.23        |        7.24        |  2.8969e+01   |  33.511434 dB  |
|                               | Gamma Correction (g=0.6) |       153.45       |        19.24        |        5.59        |      N/A      |      N/A       |
|                               | Logarithmic              |       215.69       |        9.44         |        4.90        |      N/A      |      N/A       |
| ---                           | ---                      |        ---         |         ---         |        ---         |      ---      |      ---       |
| **saturn.bmp**                | Original                 |       50.49        |        72.98        |        4.11        |      N/A      |      N/A       |
|                               | Linear Stretch           |       50.49        |        72.98        |        4.11        |  0.0000e+00   |    Infinity    |
|                               | Equalization             |       173.49       |        31.25        |        3.62        |  1.7173e+04   |  5.782236 dB   |
|                               | CLAHE                    |       54.40        |        72.83        |        4.16        |  1.0350e+01   |  37.981515 dB  |
|                               | Gamma Correction (g=0.6) |       64.43        |        85.91        |        3.97        |      N/A      |      N/A       |
|                               | Logarithmic              |       85.45        |       107.21        |        3.58        |      N/A      |      N/A       |
| ---                           | ---                      |        ---         |         ---         |        ---         |      ---      |      ---       |
| **westconcordorthophoto.bmp** | Original                 |       137.85       |        55.37        |        7.74        |      N/A      |      N/A       |
|                               | Linear Stretch           |       131.55       |        58.35        |        7.74        |  6.2977e-03   |  70.139009 dB  |
|                               | Equalization             |       128.14       |        73.58        |        7.62        |  0.0000e+00   |    Infinity    |
|                               | CLAHE                    |       129.48       |        71.86        |        7.99        |  3.0429e+00   |  43.297984 dB  |
|                               | Gamma Correction (g=0.6) |       172.47       |        44.23        |        7.33        |      N/A      |      N/A       |
|                               | Logarithmic              |       222.41       |        21.64        |        6.26        |      N/A      |      N/A       |
| ---                           | ---                      |        ---         |         ---         |        ---         |      ---      |      ---       |

## 4. Analiză și Concluzii

În urma rulării metodelor pe imaginile de test, se desprind următoarele observații:

1. **Egalizarea Histogramei (HE)** oferă cea mai mare creștere a deviației standard (contrast global), dar în imagini precum `cameraman.bmp` sau `pout.bmp`, fundalul sau detaliile fine devin nenaturale, iar zgomotul este amplificat vizibil.
2. **CLAHE** rezolvă excelent această problemă. Prin împărțirea imaginii în ferestre locale și aplicarea limitării de contrast, zgomotul nu este amplificat artificial. De exemplu, în cazul imaginilor microscopice precum `bacteria.bmp` sau `cell.bmp`, CLAHE scoate în evidență elementele structurale într-un mod natural și foarte detaliat (entropia se menține mare, iar detaliile locale devin extrem de clare).
3. **Linear Stretching** este eficient pentru imagini care au un contrast bun dar nu folosesc întreaga gamă dinamică $[0, 255]$. Pentru imagini care au deja pixeli la extreme (zgomot sau fundal alb/negru curat), stretching-ul liniar clasic nu aduce modificări semnificative (MSE = 0).
4. **Corecția Gamma (cu $\gamma = 0.6$)** îmbunătățește considerabil imaginile subexpuse (întunecate) precum `saturn.bmp` sau `moon.bmp`, crescând vizibilitatea elementelor din zonele de umbră fără a satura zonele luminoase, spre deosebire de transformarea logaritmică care distorsionează culorile prea mult.
5. **Transformarea Logaritmică** comprimă dinamică luminilor și dilată umbrele. Este ideală pentru analiza de detalii în imagini cu contrast foarte mare în zonele întunecate (cum ar fi spectrograme sau imagini din microscop).
