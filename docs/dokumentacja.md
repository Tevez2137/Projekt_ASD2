# Dokumentacja Projektowa: Królestwo Krasnoludków (ASD 2)

Projekt realizowany w ramach przedmiotu Algorytmy i Struktury Danych 2. System rozwiązuje złożone problemy logistyczno-geometryczne, wykorzystując autorski silnik w C++ oraz interfejs graficzny w Pythonie (PySide6).

---

## 1. Problem 1: Przydział krasnoludków do kopalni (Minimalizacja Owsianki)

### 1.1 Sformalizowanie problemu
Zadanie polega na znalezieniu optymalnego skojarzenia w grafie dwudzielnym określającym preferencje. Jednym zbiorem wierzchołków są krasnoludki ($N$), a drugim kopalnie ($M$). Każda kopalnia $j$ posiada ograniczoną pojemność $C_j$, a każdy krasnoludek $i$ może pracować tylko w kopalni wydobywającej jeden z akceptowanych przez niego surowców. Celem jest znalezienie takiego przydziału (maksymalizującego sumaryczną liczbę pracujących krasnoludków), który jednocześnie minimalizuje sumaryczną odległość euklidesową (koszt transportu/owsianki) między domkami krasnoludków a wyrobiskami.

### 1.2 Wykorzystane struktury danych
Problem został zamodelowany za pomocą **sieci przepływowej**. 
* **Wierzchołki:** W grafie zdefiniowano $N+M+2$ wierzchołki (krasnoludki, kopalnie, sztuczne źródło $S$ oraz sztuczne ujście $T$).
* **Krawędzie:** Zaimplementowane jako niestandardowa struktura `Edge` przechowująca: wierzchołek docelowy (`to`), pojemność (`capacity`), aktualny przepływ (`flow`), odległość z domku do kopalni jako koszt (`cost`) oraz indeks krawędzi powrotnej (`rev_idx`) niezbędny do obsługi grafu rezydualnego.
* **Reprezentacja grafu:** Wykorzystano listy sąsiedztwa `std::vector<std::vector<Edge>> adj`, zapewniające sprawną iterację po krawędziach wychodzących.

### 1.3 Opis algorytmu (Cycle Canceling / Min-Cost Max-Flow)
Rozwiązanie zaimplementowano w oparciu o algorytm Min-Cost Max-Flow w wariancie Cycle Canceling (niwelowanie cykli). Rozwiązanie podzielono na dwie fazy:
1. **Faza 1 (Algorytm Edmondsa-Karpa):** Znalezienie początkowego, maksymalnego przepływu w sieci grafowej, ignorując początkowo koszty krawędzi. Wykorzystano przeszukiwanie wszerz (BFS), co pozwala na iteracyjne znalezienie ścieżek powiększających i obsadzenie jak największej liczby stanowisk pracy.
2. **Faza 2 (Klasyczny Bellman-Ford):** Minimalizacja kosztów przepływu. Na utworzonym w Fazie 1 grafie rezydualnym iteracyjnie wyszukiwane są cykle o ujemnym koszcie. Do ich detekcji wykorzystano algorytm Bellmana-Forda, dokonujący relaksacji krawędzi $V$ razy. Po wykryciu i zrekonstruowaniu cyklu o ujemnym koszcie, przesyłany jest przez niego przepływ, co globalnie zmniejsza dystans pokonywany przez krasnoludki. Proces powtarza się aż do usunięcia wszystkich ujemnych cykli z sieci rezydualnej.

### 1.4 Analiza poprawności
* Utworzenie krawędzi ze źródła $S$ do każdego krasnoludka o przepustowości równej 1 gwarantuje, że krasnoludek zostanie przydzielony tylko do jednego miejsca pracy.
* Twierdzenie Kleina o niwelowaniu cykli dowodzi, że przepływ ma minimalny koszt wtedy i tylko wtedy, gdy w jego sieci rezydualnej nie ma skierowanych cykli o ujemnym koszcie. Ponieważ Faza 2 z wykorzystaniem algorytmu Bellmana-Forda rygorystycznie eliminuje każdy taki cykl, ostateczny przydział gwarantuje matematyczne minimum pokonywanego dystansu bez spadku wartości produkowanych dóbr.

### 1.5 Złożoność
* **Złożoność pamięciowa:** $\mathcal{O}(V + E)$, gdzie $V = N+M+2$, a $E$ to liczba możliwych krawędzi. Dzięki zastosowaniu wektorów i list sąsiedztwa pamięć rośnie liniowo, co w pełni wystarcza dla zadanego problemu. Pamięć w poszczególnych fazach została zoptymalizowana przez deklarację pomocniczych wektorów przed główną pętlą.
* **Złożoność czasowa:** Faza 1 (Edmonds-Karp) działa w optymalnym czasie $\mathcal{O}(V \cdot E^2)$. W Fazie 2 pojedyncze wywołanie algorytmu Bellmana-Forda wymaga czasu $\mathcal{O}(V \cdot E)$. Ze względu na metodę Cycle Canceling w najgorszym przypadku pętla może wykonać się proporcjonalnie do maksymalnego kosztu i maksymalnego przepływu. W praktycznych testach algorytm działa jednak niezwykle sprawnie.

---

## 2. Problem 2: Trasa Patrolowa Księcia (Obrona przed Jabłkami)

### 2.1 Sformalizowanie problemu
Zadanie polega na wyznaczeniu najkrótszej zamkniętej ścieżki, która otacza wszystkie punkty reprezentujące aktywne kopalnie (do których w Problemie 1 zostali przydzieleni pracownicy). W ujęciu matematycznym i geometrycznym problem ten sprowadza się do znalezienia **otoczki wypukłej** dla podanego zbioru punktów $P$ w przestrzeni dwuwymiarowej, a następnie obliczenia jej obwodu.

### 2.2 Wykorzystane struktury danych
* **Współrzędne:** Struktura `Wspolrzedne` przechowująca liczby całkowite `x` i `y`.
* **Kolekcja punktów:** Kontener `std::vector<Wspolrzedne>` wykorzystany do przechowywania wszystkich aktywnych kopalni oraz stosu punktów tworzących docelową otoczkę wypukłą.
* **Integracja z przepływem:** Przed uruchomieniem algorytmu geometrycznego, graf rezydualny z Problemu 1 jest analizowany. Kopalnia jest uznawana za aktywną tylko wtedy, gdy na krawędzi łączącej ją z super-ujściem przepływ jest dodatni (`e.flow > 0`).

### 2.3 Opis algorytmu (Algorytm Grahama)
Do wyznaczenia otoczki wypukłej zaimplementowano **Algorytm Grahama**. Działanie zrealizowano w krokach:
1. **Wybór punktu startowego ($P_0$):** Znalezienie punktu o najmniejszej współrzędnej $Y$ (w przypadku remisów najmniejszy $X$).
2. **Sortowanie kątowe:** Pozostałe punkty są sortowane rosnąco według kąta biegunowego względem $P_0$. Uniknięto trygonometrii na rzecz weryfikacji kierunku za pomocą szybkiego **iloczynu wektorowego (funkcja `skret`)**. 
3. **Konstrukcja otoczki (Skanowanie):** Przechodzimy po posortowanych punktach, utrzymując je na wirtualnym stosie. Dla każdego nowego punktu sprawdzamy iloczyn wektorowy z dwoma poprzednimi. Jeśli następuje "skręt w prawo" (wklęsłość), ostatni punkt jest zrzucany ze stosu.
4. **Kalkulacja obwodu:** Sumowanie odległości euklidesowych pomiędzy ostatecznymi wierzchołkami otoczki, korzystając z funkcji `std::hypot`.

### 2.4 Analiza poprawności
Algorytm Grahama gwarantuje odnalezienie poprawnej otoczki wypukłej. Relacja iloczynu wektorowego dokładnie określa, czy dodanie nowego punktu zaburza wypukłość wielokąta. Przypadki brzegowe (np. współliniowość) zostały obsłużone za pomocą predykatu sprawdzającego podniesiony do kwadratu dystans euklidesowy. Dodatkowo, dla mniej niż 3 kopalni algorytm bezpiecznie zwraca odpowiednio dystans zerowy lub podwojony dystans euklidesowy (droga patrolowa tam i z powrotem).

### 2.5 Złożoność
Niech $K$ oznacza liczbę *aktywnych* kopalni ($K \le M$).
* **Złożoność pamięciowa:** $\mathcal{O}(K)$ – algorytm operuje wyłącznie na kopiach wektorów w pamięci na stosie, nie tworząc dodatkowych skomplikowanych struktur.
* **Złożoność czasowa:** $\mathcal{O}(K \log K)$ – zdominowana przez etap sortowania punktów (funkcja `std::sort`). Samo skanowanie Grahama to operacja liniowa $\mathcal{O}(K)$. Czas przygotowania (wyłowienie aktywnych kopalni z grafu) to $\mathcal{O}(M)$.

---

## 3. Problem 3: Salwa Dekametrowców (Obrona Przeciwlotnicza)

### 3.1 Sformalizowanie problemu
Problem polega na błyskawicznym wyłonieniu dowódcy obrony (dekametrowca o największej "głośności") na konkretnym, zaatakowanym odcinku granicy. Granicę państwa reprezentuje zbiór $N$ dekametrowców rozstawionych wzdłuż otoczki wypukłej wyznaczonej w Problemie 2. System musi sprawnie odpowiadać na zapytania o maksimum w zadanym przedziale indeksów $[L, R]$. Ponieważ granica jest krzywą zamkniętą (pętlą), ataki mogą "przechodzić" przez punkt zszycia tablicy, co oznacza, że indeks początkowy może być matematycznie większy od końcowego ($L > R$).

### 3.2 Wykorzystane struktury danych
Dla uzyskania logarytmicznego czasu odpowiedzi wykorzystano strukturę **Drzewa Przedziałowego (Segment Tree)** zaimplementowaną na tablicy.
* **Reprezentacja drzewa:** Wektor `std::vector<WezelDrzewa> tree` o rozmiarze $4N$, gdzie każdy węzeł przechowuje strukturę z polami `maxGlosnosc` oraz `krasnoludekID`.
* **Element neutralny:** Do obsługi pustych wywołań w liściach zdefiniowano stałą `MINUS_NIESKONCZONOSC` równą $-1$. Gwarantuje to, że zapytania wychodzące poza zakres nie zaburzają wyników funkcji `max()`.
* **Dane wejściowe:** Wektor bazowy `std::vector<Dekametrowiec> A`, generowany dynamicznie na podstawie współrzędnych geograficznych otoczki wypukłej.

### 3.3 Opis algorytmu (Range Maximum Query - RMQ)
Działanie modułu opiera się na klasycznym rozwiązaniu problemu RMQ:
1. **Budowa drzewa (Funkcja `BUILD`):** Konstrukcja przebiega w sposób rekurencyjny (Top-Down). Przestrzeń tablicy dzielona jest na połowy aż do osiągnięcia liści, do których wpisywane są początkowe głośności dekametrowców. Przy powrocie z rekurencji, wartość węzła wewnętrznego jest wyznaczana jako maksimum z jego dwojga dzieci (`tree[v] = max(tree[2v], tree[2v+1])`).
2. **Przeszukiwanie (Funkcja `QUERY`):** Aby znaleźć maksimum na przedziale $[L, R]$, drzewo jest przeszukiwane rekurencyjnie. 
   * Jeśli rozpatrywany węzeł jest całkowicie rozłączny z zadanym zapytaniem, algorytm natychmiast zwraca element neutralny.
   * Jeśli węzeł w całości zawiera się w przedziale zapytania, zwracana jest jego wartość (bez dalszego schodzenia w dół, co gwarantuje wysoką wydajność).
   * W przypadku częściowego pokrycia, przedział jest dzielony i wynik jest maksimum z lewego i prawego poddrzewa.
3. **Zapytania cykliczne (Atak dookoła granicy):** Zaimplementowano specjalną logikę odpytywania. Jeżeli $L \le R$, wykonywane jest jedno standardowe zapytanie. Jeżeli $L > R$, zapytanie rozbijane jest na dwa niezależne wywołania: na przedział od $L$ do końca tablicy ($N-1$) oraz od początku tablicy ($0$) do $R$. Ostatecznym dowódcą zostaje krasnoludek wyłoniony z porównania maksimów tych dwóch zapytań.

### 3.4 Analiza poprawności
Poprawność drzewa przedziałowego opiera się na własności łączności funkcji statystycznej – funkcja wyznaczania maksimum jest w pełni łączna, tj. $\max(a, b, c) = \max(\max(a, b), c)$. Rozbicie przedziału na potęgi dwójki w wewnętrznych węzłach drzewa pozwala na bezbłędną agregację danych. Logika rozbijania zapytań cyklicznych również jest matematycznie i logicznie poprawna, opierając się na tożsamości $\max(A \cup B) = \max(\max(A), \max(B))$, co idealnie modeluje domkniętą granicę Otoczki Wypukłej.

### 3.5 Złożoność
Niech $N$ oznacza liczbę dekametrowców na granicy (liczbę punktów na otoczce wypukłej).
* **Złożoność pamięciowa:** $\mathcal{O}(N)$ – reprezentacja tablicowa pełnego drzewa binarnego wymaga alokacji tablicy o wielkości najwyżej $4N$, co jest optymalne pod względem narzutu pamięciowego.
* **Złożoność czasowa budowy:** $\mathcal{O}(N)$ – każdy z około $2N$ węzłów drzewa jest odwiedzany dokładnie raz podczas inicjalizacji.
* **Złożoność czasowa zapytania:** $\mathcal{O}(\log N)$ – dzięki agregacji danych w węzłach, maksymalna liczba odwiedzonych wierzchołków na każdym poziomie drzewa podczas działania funkcji `QUERY` wynosi $4$. W przypadku zapytania rozbitego (cyklicznego) wykonywane są dwa takie zapytania, co asymptotycznie utrzymuje czas wyszukiwania w doskonałej złożoności logarytmicznej $2 \times \mathcal{O}(\log N) = \mathcal{O}(\log N)$.

---

## 4. Problem 4: Elektroniczne Księgi (Kompresja i Wyszukiwanie)

### 4.1 Sformalizowanie problemu
Zadanie polega na optymalizacji przechowywania obszernych archiwów królestwa (zmniejszenie objętości danych tekstowych - "elektronicznego papieru") oraz umożliwieniu błyskawicznego odnajdywania konkretnych fraz i słów w tekstach ksiąg. Problem algorytmiczny sprowadza się do implementacji bezstratnej kompresji tekstu na poziomie bitowym oraz szybkiego algorytmu wyszukiwania wzorców w długich łańcuchach znaków.

### 4.2 Wykorzystane struktury danych
Dla realizacji tego problemu zastosowano dwie odrębne rodziny struktur:
* **Kompresja:** **Drzewo binarne** (reprezentujące Drzewo Huffmana, węzły `HuffmanNode`), **Kolejka priorytetowa** (`std::priority_queue` typu Min-Heap) do budowy drzewa z optymalnym czasem dostępu do najmniejszych elementów, oraz **Tablice asocjacyjne** (`std::map`) do zliczania częstotliwości wystąpień znaków i przechowywania przypisanych im kodów binarnych.
* **Wyszukiwanie:** Struktury wbudowane języka (łańcuchy `std::string`) oraz zmienne całkowitoliczbowe służące jako rejestry dla **wartości funkcji haszujących** (z wykorzystaniem operacji matematycznych modulo do uniknięcia przepełnienia typu).

### 4.3 Opis algorytmu (Huffman i Rabin-Karp)
Moduł został podzielony na dwa współpracujące, lecz niezależne algorytmy tekstowe:
1. **Oszczędzanie papieru (Algorytm Huffmana):**
   * Pierwszym krokiem jest budowa słownika częstotliwości wszystkich znaków występujących w tekście.
   * Następuje inicjalizacja kolejki priorytetowej, w której każdy unikalny znak staje się pojedynczym węzłem.
   * Algorytm iteracyjnie pobiera z kolejki dwa węzły o najmniejszej częstotliwości i łączy je w nowy węzeł nadrzędny. Proces powtarza się aż do utworzenia jednego wspólnego korzenia – pełnego Drzewa Huffmana.
   * Zwieńczeniem jest głębokie przejście drzewa i wygenerowanie unikalnych kodów prefiksowych (ciągów `0` i `1`) dla każdego znaku, co pozwala zamienić tekst na wysoce skompresowany ciąg symboli.
2. **Szybkie wyszukiwanie (Algorytm Rabina-Karpa):**
   * Zamiast porównywać szukany wzorzec naiwnie (znak po znaku, dla każdej możliwej pozycji w księdze), algorytm korzysta z matematycznej funkcji haszującej.
   * Na starcie obliczany jest hasz dla szukanego wzorca oraz dla początkowego "okna" tekstu o tej samej długości.
   * Podczas przesuwania okna wzdłuż tekstu, nowy hasz jest wyliczany w czasie stałym (*Rolling Hash*). Realizowane jest to poprzez matematyczne odjęcie wartości pierwszego znaku opuszczającego okno i dodanie wartości nowego znaku, który się w nim pojawia.
   * Tylko w przypadku pełnej zgodności obu haszy, algorytm dokonuje faktycznego porównania znaków, by ostatecznie zatwierdzić znalezisko.

### 4.4 Analiza poprawności
* **Algorytm Huffmana:** Gwarantuje stworzenie kodów prefiksowych, co oznacza, że żaden kod litery nie jest prefiksem (początkiem) kodu innej litery. Dzięki temu kompresja jest w 100% bezstratna, a dekodowanie z perspektywy maszyny jest absolutnie jednoznaczne. Drzewo gwarantuje wyznaczenie matematycznie optymalnej (najkrótszej) średniej długości kodu dla danego tekstu.
* **Algorytm Rabina-Karpa:** Użycie manualnego sprawdzenia "znak-po-znaku" w momencie, gdy zrównają się sumy kontrolne (hasze), jest kluczowe dla poprawności. Taki mechanizm gwarantuje brak wyników fałszywie pozytywnych (*False Positives*), które mogłyby się pojawić w wyniku naturalnych kolizji funkcji modulo dla różnych, ale matematycznie podobnych słów.

### 4.5 Złożoność
Niech $N$ oznacza długość tekstu w księdze, $M$ długość szukanego wzorca, a $A$ wielkość alfabetu (liczbę unikalnych znaków w tekście).
* **Złożoność pamięciowa:** Drzewo Huffmana oraz pomocnicze mapy wymagają rozmiaru $\mathcal{O}(A)$. Ponieważ operujemy na ograniczonym alfabecie ASCII, $A \le 256$, co w praktyce oznacza narzut pamięciowy rzędu stałego $\mathcal{O}(1)$. Algorytm Rabina-Karpa również operuje w miejscu, wymagając pamięci $\mathcal{O}(1)$, gdyż przechowuje jedynie zmienne dla haszy.
* **Złożoność czasowa kompresji (Huffman):** Jednokrotne przejście w celu zliczenia znaków zajmuje $\mathcal{O}(N)$. Zbudowanie kolejki i wyciąganie z niej elementów to czas $\mathcal{O}(A \log A)$. Sumarycznie czas wykonania kompresji to optymalne $\mathcal{O}(N + A \log A)$.
* **Złożoność czasowa wyszukiwania (Rabin-Karp):** Obliczenie początkowego hasza trwa $\mathcal{O}(M)$. Przesunięcie "Rolling Hasha" trwa $\mathcal{O}(1)$ dla każdego znaku tekstu. W przypadku optymistycznym i średnim (niewielka liczba kolizji), czas wyszukiwania jest wysoce zoptymalizowany i wynosi $\mathcal{O}(N + M)$. Tylko w absurdalnie pesymistycznym przypadku (gdzie niemal wszystkie okna generują tę samą sumę kontrolną) czas wyszukiwania degraduje się do liniowo-kwadratowego $\mathcal{O}(N \cdot M)$.

---

## 5. Metodologia Testowania