<h1 align="center">⛏️ Królestwo Krasnoludków - ASD 2</h1>

<div align="center">
  <img src="https://img.shields.io/badge/C++-17-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" />
  <img src="https://img.shields.io/badge/Python-3.9+-3776AB?style=for-the-badge&logo=python&logoColor=white" />
  <img src="https://img.shields.io/badge/GUI-PySide6-41CD52?style=for-the-badge&logo=qt&logoColor=white" />
  <img src="https://img.shields.io/badge/Course-Algorytmy_i_Struktury_Danych_2-8A2BE2?style=for-the-badge" />
</div>

<br>

## 📖 O projekcie

Projekt zaliczeniowy stworzony w ramach przedmiotu **Algorytmy i Struktury Danych 2 (ASD2)**. Aplikacja rozwiązuje złożone problemy optymalizacyjne, geometryczne, tekstowe oraz struktur danych w fikcyjnym, rozrastającym się królestwie Królewny Śnieżki i Księcia z bajki. 

Celem projektu było zaprojektowanie od podstaw wydajnego silnika obliczeniowego w języku **C++**, z pominięciem gotowych bibliotek dla kluczowych algorytmów (np. STL dla grafów). System łączy czystą, niskopoziomową wydajność z nowoczesnym interfejsem użytkownika zaimplementowanym w **Pythonie (PySide6)**, co pozwala na pełną wizualizację wyników.

---

## 📑 Spis Treści
1. [Szczegółowy Opis Modułów i Algorytmów](#1-szczegółowy-opis-modułów-i-algorytmów)
2. [Złożoność Obliczeniowa i Benchmarki](#2-złożoność-obliczeniowa-i-benchmarki)
3. [Architektura Systemu i Przepływ Danych](#3-architektura-systemu-i-przepływ-danych)
4. [Format Danych (Wejście/Wyjście)](#4-format-danych-wejściewyjście)
5. [Wymagania Systemowe](#5-wymagania-systemowe)
6. [Instalacja i Uruchomienie](#6-instalacja-i-uruchomienie)
7. [Wizualizacja (GUI)](#7-wizualizacja-gui)
8. [Struktura Katalogów](#8-struktura-katalogów)
9. [Zespół Projektowy](#9-zespół-projektowy)

---

## 1. Szczegółowy Opis Modułów i Algorytmów

Całość systemu podzielona jest na 4 niezależne moduły biznesowe, z których każdy odpowiada za rozwiązanie konkretnej klasy problemu algorytmicznego.

### Moduł I: Problem Owsianki (Optymalizacja Sieci Przepływowej)
**Domena:** Optymalizacja logistyki i przydziału zasobów (Task Assignment).
**Problem:** Śnieżka musi przydzielić krasnoludków do odpowiednich kopalni na podstawie ich preferencji wydobywczych (złoto, diamenty, srebro). Kopalnie mają limitowaną liczbę wakatów. Książę wymaga zminimalizowania łącznego dystansu dojazdów do pracy, aby oszczędzić na cateringu (owsianka).
**Implementacja:**
* Wykorzystano model **Min-Cost Max-Flow** (Przepływ o minimalnym koszcie i maksymalnej wartości).
* Zaimplementowano wariant algorytmu **Edmondsa-Karpa**. Ponieważ graf posiada wagi na krawędziach reprezentujące koszty dystansu, do znajdowania ścieżek powiększających wykorzystano algorytm Bellmana-Forda.

### Moduł II: Trasa Patrolowa Księcia (Geometria Obliczeniowa)
**Domena:** Obliczanie powłok geometrycznych.
**Problem:** Książę patroluje terytorium wokół kopalni, w których w danym dniu pracują krasnoludki. Trasa musi tworzyć zamknięty wielokąt, zamykając obszar wewnątrz i ogradzając królestwo przed przemytnikami zatrutych jabłek.
**Implementacja:**
* System w pierwszej kolejności filtruje dane z Modułu I, pozostawiając tylko współrzędne *aktywnych* kopalni.
* Następnie uruchamiany jest **Algorytm Grahama (Graham Scan)** wyliczający Otoczkę Wypukłą (Convex Hull). Punkty są sortowane kątowo względem punktu startowego (najniższego, najbardziej wysuniętego na lewo), a następnie obwiednia jest budowana iteracyjnie z wykorzystaniem struktury stosu.

### Moduł III: Salwa Dekametrowców (Zaawansowane Struktury Danych)
**Domena:** Szybkie zapytania na przedziałach (Range Queries).
**Problem:** Granice królestwa chronione są przez łuczników. W przypadku nagłego ataku wroga na dany odcinek granicy, dowodzenie ma przejąć krasnoludek o najdonośniejszym głosie znajdujący się dokładnie na atakowanym fragmencie.
**Implementacja:**
* Problem sprowadzono do klasycznego **RMQ (Range Maximum Query)**.
* Skonstruowano statyczne **Drzewo Przedziałowe (Segment Tree)** pozwalające na błyskawiczne znajdowanie ekstremum na dowolnym podprzedziale ciągłej tablicy łuczników.

### Moduł IV: Elektroniczne Księgi (Kompresja i Algorytmy Tekstowe)
**Domena:** Kompresja bezstratna oraz wyszukiwanie wzorców (String Matching).
**Problem:** Zarchiwizowanie setek ksiąg i traktatów w sposób oszczędzający miejsce na dyskach w królestwie, przy zachowaniu możliwości błyskawicznego przeszukiwania zarchiwizowanych tekstów.
**Implementacja:**
* Zastosowano **Algorytm Huffmana** budujący drzewo prefiksowe, przypisujący krótsze ciągi bitowe znakom występującym częściej w badanym korpusie językowym.
* Do szybkiego wyszukiwania wzorców zaimplementowano algorytm **Rabina-Karpa** z wykorzystaniem rolling hash (haszowania kroczącego), co minimalizuje liczbę pustych porównań znaków.

---

## 2. Złożoność Obliczeniowa i Benchmarki

Wszystkie struktury zaimplementowano ze szczególnym naciskiem na optymalne wykorzystanie zasobów pamięciowych i obliczeniowych. Gdzie: $V$ oznacza liczbę wierzchołków, $E$ liczbę krawędzi, $N$ liczbę elementów, a $M$ długość wzorca tekstowego.

### Tabela Złożoności Teoretycznej

| Algorytm / Struktura | Złożoność Czasowa | Złożoność Pamięciowa | Uwagi |
|----------------------|-------------------|----------------------|-------|
| **Min-Cost Max-Flow** | $O(E \cdot V \cdot f)$ | $O(V + E)$ | $f$ to maksymalny przepływ w sieci przydziałów. |
| **Graham Scan** (Otoczka) | $O(N \log N)$ | $O(N)$ | Sortowanie kątowe stanowi główny czynnik złożoności. |
| **Segment Tree** (Budowa)| $O(C \cdot N)$ | $O(N)$ | Gdzie stała $C$ wynika z operacji alokacji i łączenia węzłów. |
| **Segment Tree** (RMQ) | $O(\log N)$ | $O(1)$ | Wysokość drzewa wynosi $\log N$. |
| **Huffman** (Kompresja)| $O(N \log K)$ | $O(K)$ | $K$ to rozmiar alfabetu. |
| **Rabin-Karp** | $O(N + M)$ | $O(1)$ | Złożoność oczekiwana (przy braku masowych kolizji hashy). |

### 📈 Wyniki Testów Obciążeniowych

Wykonaliśmy 5 stopniowych testów skalowalności, aby sprawdzić wytrzymałość sieci przepływowej (`Min-Cost Max-Flow`) oraz czas reakcji struktur zapytań (`Drzewa Przedziałowego`) w naszym królestwie.

| Zestaw Testowy | Rozmiar (Kopalnie / Krasnoludki) | Czas Inicjalizacji (MCMF) | Czas Reakcji (Salwa / RMQ) |
| :--- | :--- | :--- | :--- |
| **Bardzo Mały** | 30 / 150 | ~1.00 s | 0.014 s |
| **Mały** | 70 / 350 | ~1.00 s | 0.015 s |
| **Średni** | 150 / 750 | ~20.01 s | 0.018 s |
| **Duży** | 300 / 1500 | ~268.09 s | 0.021 s |
| **MAMUT** | 700 / 3500 | ~15541.44 s | 0.035 s |

**⚡ Wnioski z Tabeli:**
* **Wykładniczy koszt budowy:** Czas alokacji i wyliczania ścieżek dla Problemu Owsianki rośnie drastycznie przy powiększaniu grafu (od 1 sekundy dla małych zbiorów, aż do ponad 4 godzin dla zestawu typu "MAMUT"). Potwierdza to pesymistyczną asymptotykę problemów przepływowych na potężnych grafach z wagami.
* **Niezachwiana wydajność zapytań:** Mimo aż 23-krotnego wzrostu liczby danych wejściowych, czas wyszukiwania zagrożeń w drzewie przedziałowym (Salwa) wzrósł zaledwie o ułamki sekund (z 0.014 s do 0.035 s). Jest to idealne, praktyczne zobrazowanie potęgi logarytmicznej złożoności zapytań $O(\log N)$ na strukturze *Segment Tree*.

---

## 3. Architektura Systemu i Przepływ Danych

Projekt wykorzystuje nowoczesne podejście hybrydowe (Backend/Frontend rozdzielony na poziomie procesów):

1. **Warstwa GUI (Python + PySide6):** Zbiera parametry od użytkownika i uruchamia podproces (`subprocess`).
2. **Warstwa Automatyzacji (Makefile):** Automatycznie kompiluje pliki C++ jeśli nastąpiły w nich zmiany (`make build`).
3. **Warstwa Obliczeniowa (C++):** Odczytuje dane wejściowe z plików w katalogu `data/` lub `dane_testowe/`, przeprowadza alokację pamięci, wykonuje algorytmy i zapisuje ustrukturyzowane wyniki bezpośrednio w katalogu wymiany danych.
4. **Warstwa Prezentacji (Python):** Skrypt główny wczytuje wygenerowane pliki wynikowe i używa ich do narysowania wykresów, ścieżek patrolowych oraz tabel na interaktywnym Canvasie w GUI.

---

## 4. Format Danych (Wejście/Wyjście)

System został zaprojektowany z myślą o uniwersalności. Dane wejściowe oraz struktury wynikowe przetwarzane przez silnik C++ opierają się na ujednoliconych formatach tekstowych i binarnych, co ułatwia bezpieczne parsowanie i eliminuje narzut pamięciowy.

* **Dane dla Owsianki:** Lista krasnoludków (ID, preferencje, obecna pozycja $X, Y$) oraz lista kopalń (ID, pozycja $X, Y$, max pojemność).
* **Dane Geometrii i Struktur:** Mapy współrzędnych aktywnych kopalni eksportowane przez silnik w celu wyznaczenia otoczki wypukłej oraz uporządkowane tablice wartowników łuczniczych z przypisanymi poziomami donośności głosu dla zapytań przedziałowych.

---

## 5. Wymagania Systemowe

System został przetestowany na środowiskach Windows, Linux (Ubuntu) oraz macOS.
* **Kompilator C++:** Obsługujący standard C++17 (np. `g++`, `clang++`, lub MinGW-w64 na Windows)
* **Make:** Z narzędzi `build-essential`
* **Python:** Wersja 3.9+
* **Zależności Python:**
  ```bash
  pip install PySide6 matplotlib pandas
  ```

---

## 6. Instalacja i Uruchomienie

### Sposób A: Przez Interfejs Graficzny (Rekomendowane)
Jest to najprostsza metoda, nie wymagająca ręcznej obsługi Makefile.
1. Sklonuj repozytorium:
   ```bash
   git clone [https://github.com/Tevez2137/Projekt_ASD2.git](https://github.com/Tevez2137/Projekt_ASD2.git)
   cd Projekt_ASD2
   ```
2. Uruchom skrypt główny:
   ```bash
   python gui/main.py
   ```
3. W GUI kliknij **"Uruchom Algorytmy"**. Aplikacja wywoła odpowiednie komendy shellowe pod spodem.

### Sposób B: Uruchomienie Głównego Silnika z Konsoli
Jeśli chcesz pominąć GUI i testować czystą wydajność algorytmów w terminalu:
1. Przejdź do głównego katalogu z plikiem `makefile`.
2. Skompiluj projekt poleceniem:
   ```bash
   make
   ```
3. Uruchom wygenerowany plik binarny:
   ```bash
   ./main
   ```
4. Wyczyszczenie plików binarnych (.o):
   ```bash
   make clean
   ```

---

## 7. Wizualizacja (GUI)

Interfejs zaimplementowany w **PySide6** umożliwia wygodne korzystanie z systemu. Panel dzieli się na:
* **Mapę Królestwa (Canvas):** Miejsce, w którym na żywo renderowane są kopalnie (jako wierzchołek) oraz przydziały krasnoludków. Tam również rysowana jest finalna trasa patrolowa (wielokąt z otoczki wypukłej).
* **Panel Sterowania:** Przyciski uruchamiające konkretne moduły, przełączanie między zestawami `danych testowych`.
* **Konsolę Zdarzeń:** Okno tekstowe wyświetlające status kompresji plików (Huffman), komunikaty o najgłośniejszym dekametrowcu oraz łącznym koszcie logistycznym (wynik Min-Cost Max-Flow).

---

## 8. Struktura Katalogów

```text
Projekt_ASD2/
├── .gitignore          # Ignorowane pliki binarne (.o, .exe)
├── Makefile            # Skrypt budujący aplikację C++
├── main.cpp            # Główny entry point dla aplikacji backendowej
│
├── src/                # Folder klas algorytmicznych (C++)
│   ├── Graph.cpp       # Implementacja algorytmów sieciowych i przepływu
│   ├── Geometry.cpp    # Convex Hull i operacje na wektorach
│   └── DataStruct.cpp  # Segment Tree, Huffman, Rabin-Karp
│
├── gui/                # Skrypty i interfejs PySide6 (Python)
│   ├── main.py         # Entry point dla GUI
│   └── views/          # Komponenty okien i map
│
├── data/               # Podstawowe słowniki wejściowe
├── dane_testowe/       # Duże zbiory do testów obciążeniowych i asymptotyki
└── docs/               # Raporty wydajności i obrony projektu
```

---

## 9. Zespół Projektowy

Opracowanie analityczne, implementacja niskopoziomowa, integracja architektoniczna i design UX zostały stworzone przez zespół inżynierski:
* **Karol Gorlik**
* **Wiktor Góralski**
* **Kacper Narodzonek**
* **Kacper Wilkanowski**

<br>
<div align="center">
  <i>"Z optymalnym algorytmem nawet królestwo wielkości kontynentu można obronić w stałym czasie."</i>
</div>