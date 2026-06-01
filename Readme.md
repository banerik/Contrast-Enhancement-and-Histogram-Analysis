# Îmbunătățirea Contrastului și Analiza Histogramelor (PI)

Acest proiect conține implementări din scratch (de la zero, folosind operații pixel cu pixel) pentru diverse tehnici de analiză a histogramelor și îmbunătățire a contrastului pe imagini grayscale, conform cerințelor disciplinei de **Procesarea Imaginilor (PI)**.

Conform regulamentului, funcțiile OpenCV sunt utilizate exclusiv pentru **verificare, validare și comparare**, demonstrând corectitudinea implementării proprii prin calcularea erorii medii pătratice (**MSE**) și a raportului semnal-zgomot (**PSNR**).

---

## 📖 Ghid Teoretic Simplificat (Pentru Începători)

Dacă nu ai mai lucrat cu procesarea de imagini, iată ce trebuie să știi ca să poți prezenta acest proiect cu încredere:

### 1. Ce este Histograma?
Imaginează-ți o imagine alb-negru ca o tablă de șah uriașă, unde fiecare pătrățel (pixel) are un număr între `0` (negru complet) și `255` (alb complet), reprezentând luminozitatea.
* **Histograma** este pur și simplu un grafic (o diagramă cu bare) care numără câți pixeli de fiecare valoare (de la 0 la 255) există în imagine.
* Dacă graficul are bare înalte doar în stânga, imaginea e prea întunecată.
* Dacă barele sunt doar în dreapta, imaginea e prea luminoasă.
* Dacă toate barele sunt înghesuite la mijloc, imaginea este „ștearsă” sau are **contrast scăzut**.

### 2. Metodele de Îmbunătățire a Contrastului
Contrastul reprezintă diferența dintre cel mai închis și cel mai deschis punct din imagine. Scopul nostru este să facem detaliile mai vizibile folosind următoarele metode:

*   **Linear Contrast Stretching (Întindere Liniară):** 
    Identifică cel mai întunecat pixel ($I_{\min}$) și cel mai deschis pixel ($I_{\max}$) din imagine și îi „întinde” matematic pe tot intervalul $[0, 255]$. De exemplu, dacă pixelii tăi sunt doar între 50 și 150, formula îi va redistribui astfel încât 50 devine 0, 150 devine 255, iar valorile intermediare se împrăștie proporțional.
*   **Histogram Equalization (Egalizarea Histogramei):** 
    Folosește probabilități matematice (funcția cumulativă CDF) pentru a redistribui pixelii astfel încât histograma rezultată să devină cât mai uniformă (plată) posibil. Este excelentă pentru îmbunătățirea contrastului global, dar uneori poate exagera zgomotul de fundal.
*   **CLAHE (Contrast Limited Adaptive Histogram Equalization):** 
    Este o metodă mai inteligentă (adaptivă). În loc să aplice egalizarea pe toată imaginea deodată, împarte imaginea într-o grilă de mini-imagini (plăci de $8 \times 8$ pixeli). Egalizează fiecare placă separat, limitează contrastul maxim (tăind barele prea înalte din histogramă pentru a nu amplifica zgomotul) și folosește o interpolare biliniară la margini pentru a asigura o tranziție lină între plăci.
*   **Corecția Gamma (Power-Law):** 
    O mapare neliniară. Cu un parametru $\gamma < 1$, luminează detaliile ascunse în umbră fără să strice zonele care sunt deja destul de iluminate.
*   **Transformarea Logaritmică:** 
    Extinde foarte mult intensitățile mici (zonele întunecate) și comprimă intensitățile mari. Este folosită mult în imagistică medicală sau microscopie.

### 3. Cum măsurăm performanța? (Metrici)
Pentru a analiza științific efectul metodelor, calculăm:
*   **Media ($\mu$):** Valoarea medie a luminozității (0 = negru, 255 = alb).
*   **Deviația Standard ($\sigma$):** Indică variația luminozității. **Este măsura contrastului global** - o deviație mai mare înseamnă contrast mai bun!
*   **Entropia ($H$):** Măsoară cantitatea de detaliu și textură din imagine. O entropie mai mare înseamnă că imaginea conține detalii mai clar definite.
*   **MSE & PSNR:** Comparația matematică între algoritmul nostru scris de mână și algoritmul OpenCV. Un MSE de 0.00 (sau foarte mic) arată că algoritmul implementat de noi funcționează perfect corect.

---

## 🛠️ Cerințe și Instalare

### Premize
*   **Sistem de operare:** macOS / Linux / Windows
*   **Compilator:** C++11 compatible (GCC / Clang / MSVC)
*   **Utilitare:** CMake 3.10+ și biblioteca **OpenCV 4.x** instalată.

### Construirea proiectului (Build)
Deschide terminalul în folderul rădăcină al proiectului și rulează:
```bash
mkdir -p build
cd build
cmake ..
make
```

---

## 🚀 Cum se rulează (Run)

Aplicația are două moduri distincte de funcționare:

### 1. Modul Batch (Procesare Completă Automată) - Recomandat pentru temă
Procesează toate cele 15 imagini din folderul `data/`, aplică toate cele 6 metode, salvează rezultatele vizuale și generează raportul academic:
```bash
./ImageEnhancement --batch
```
**Ce se generează în directorul `results/`:**
*   `Report.md`: Raportul academic complet în limba română cu introducere teoretică, tabele cu metricile rezultate pentru toate cele 15 imagini și concluzii (gata de prezentat!).
*   `metrics.csv`: Fișier Excel/CSV cu toate valorile numerice (Mean, StdDev, Entropy, MSE, PSNR).
*   `[nume_imagine]_comparison.png`: O grilă comparativă $2 \times 3$ care arată rezultatul fiecărei metode pe imaginea respectivă (cu etichete descriptive).
*   `[nume_imagine]_histograms.png`: O grilă comparativă $2 \times 3$ cu histogramele și curbele CDF (reprezentate cu roșu) pentru fiecare metodă.

### 2. Modul Interactiv (Imagine unică)
Rulează pe o singură imagine specificată, printează metricile în consolă și deschide ferestre grafice OpenCV pentru a vedea rezultatele live:
```bash
./ImageEnhancement ../data/cameraman.bmp
```
*Apasă orice tastă în timp ce ai focusul pe o fereastră de imagine pentru a închide programul.*
