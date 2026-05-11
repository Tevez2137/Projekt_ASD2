# ⛏️ Algorytmy i Struktury Danych 2 - Projekt 2026: Królestwo Krasnoludków

Projekt zaliczeniowy z przedmiotu ASD2, rozwiązujący złożone problemy logistyczne, geometryczne i tekstowe w rozrastającym się królestwie Królewny Śnieżki i Księcia z bajki. 

System łączy potężny silnik obliczeniowy napisany w **C++** z interaktywną wizualizacją i panelem sterowania w **Pythonie (PySide6)**. Punktem wejścia do aplikacji jest interfejs graficzny, który automatycznie zarządza kompilacją i uruchamianiem algorytmów w tle.

---

## 📖 Opis Problemów i Zastosowane Algorytmy

Projekt został podzielony na cztery główne moduły odpowiadające na potrzeby mieszkańców królestwa:

### 1. Optymalizacja Wydobycia i Logistyki (Problem Owsianki)
Królewna Śnieżka musi przydzielić krasnoludków do odpowiednich kopalni, biorąc pod uwagę ich preferencje surowcowe oraz limit miejsc w wyrobiskach. Z kolei Książę chce zminimalizować sumaryczną odległość pokonywaną przez górników, aby zaoszczędzić na ilości gotowanej owsianki.
* **Rozwiązanie:** Algorytm **Min-Cost Max-Flow** (Edmonds-Karp). Zapewnia maksymalne obsadzenie stanowisk przy jednoczesnej minimalizacji dystansu.

### 2. Trasa Patrolowa Księcia (Obrona przed Jabłkami)
Po incydencie ze złą królową, w królestwie obowiązuje bezwzględny zakaz wwożenia i spożywania jabłek. Książę musi codziennie patrolować teren królestwa. Trasa musi być jak najkrótsza, ale jednocześnie odgradzać wszystkie *aktualnie użytkowane* kopalnie.
* **Rozwiązanie:** **Algorytm Grahama (Otoczka Wypukła)**. Skonstruowany na podstawie współrzędnych tylko tych kopalni, do których w module 1 zostali przydzieleni pracownicy.

### 3. Salwa Dekametrowców (Obrona Granic)
Wzdłuż trasy patrolowej rozstawieni są łucznicy (dekametrowcy), broniący granic przed przemytem owoców. W przypadku ataku na dany odcinek granicy, dowodzenie musi przejąć najgłośniejszy krasnoludek w danym przedziale.
* **Rozwiązanie:** **Drzewo Przedziałowe (Segment Tree)** lub struktura **RMQ**, pozwalająca na błyskawiczne znajdowanie najgłośniejszego dekametrowca dla zadanego zakresu granicy.

### 4. Elektroniczne Księgi (Kompresja i Wyszukiwanie)
Aby ogromna wiedza Śnieżki i Księcia nie zaginęła, musi zostać zarchiwizowana w sposób oszczędzający "elektroniczny papier", z możliwością szybkiego wyszukiwania.
* **Rozwiązanie:** Algorytmy kompresji (np. **Huffman**) połączone z szybkimi algorytmami wyszukiwania wzorców w tekście (np. **Rabin-Karp**).

---

## 🛠️ Technologie i Uruchomienie

* **Frontend (Aplikacja GUI):** `Python 3`, `PySide6`
* **Backend (Silnik algorytmiczny):** `C++`
* **Zarządzanie kompilacją:** `Makefile` (wywoływany automatycznie przez aplikację)

### 🚀 Uruchomienie Aplikacji
Cała logika i silnik C++ są zautomatyzowane. Aby uruchomić projekt, wystarczy włączyć interfejs graficzny:

1. Upewnij się, że masz zainstalowane biblioteki Pythona (PySide6) oraz kompilator `g++` z programem `make`.
2. Otwórz terminal w głównym katalogu projektu i wpisz:
   ```bash
   python gui/main.py
   ```
3. W oknie aplikacji użyj przycisku **"Uruchom Algorytm"** – system sam skompiluje pliki C++, wyliczy optymalne trasy i narysuje wynik na mapie!

---

## 📂 Struktura Repozytorium

* `src/` – Pliki z kodem źródłowym C++ implementujące główne algorytmy.
* `gui/` – Skrypty Pythona obsługujące interfejs i komunikację z backendem.
* `data/` – Baza danych królestwa (`.csv`), skrypty generujące testy oraz pliki wynikowe.
* `docs/` – Dokumentacja, testy wydajnościowe i opis poprawności algorytmów.

---

## 👥 Zespół Projektowy
* Karol Gorlik
* Wiktor Góralski
* Kacper Narodzonek
* Kacper Wilkanowski