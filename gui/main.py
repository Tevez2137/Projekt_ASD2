import sys
import os
from PySide6.QtWidgets import QApplication, QMainWindow, QGraphicsScene
from PySide6.QtGui import QPen, QBrush, QColor
from PySide6.QtCore import Qt

# Importujemy wygenerowany interfejs z Qt Designera
from ui_interfejs import Ui_MainWindow 

class GlowneOkno(QMainWindow):
    def __init__(self):
        super().__init__()
        
        # 1. Inicjalizacja interfejsu z Designera
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        
        # Opcjonalnie: ustawienie tytułu okna, jeśli nie ustawiłeś w Designerze
        self.setWindowTitle("Symulator Logistyki Krasnoludków")
        
        # 2. Tworzymy scenę (wirtualne płótno)
        self.scene = QGraphicsScene()
        
        # 3. PODPINAMY SCENĘ POD WIDŻET O NAZWIE "map" z Qt Designera
        self.ui.map.setScene(self.scene)
        
        # 4. Odpalamy wczytywanie danych i rysowanie
        self.wczytaj_i_rysuj()

    def wczytaj_i_rysuj(self):
        # Definiowanie ścieżek do plików (szukamy w folderze data/)
        base_dir = os.path.dirname(__file__)
        path_kopalnie = os.path.join(base_dir, "..", "data", "kopalnie.csv")
        path_krasnoludki = os.path.join(base_dir, "..", "data", "dane_krasnoludkow.csv")
        path_przydzialy = os.path.join(base_dir, "..", "data", "przydzialy.txt")

        kopalnie = {}     # Słownik: ID -> (X, Y)
        krasnoludki = {}  # Słownik: ID -> (X, Y)

        # Wczytywanie Kopalni
        try:
            with open(path_kopalnie, 'r', encoding='utf-8') as f:
                next(f) # Pomijamy nagłówek
                for linia in f:
                    dane = linia.strip().split(',')
                    if len(dane) >= 3:
                        # Zapisujemy ID jako klucz, a (X, Y) jako wartość
                        kopalnie[int(dane[0])] = (int(dane[1]), int(dane[2]))
        except Exception as e:
            print(f"Błąd podczas ładowania kopalnie.csv: {e}")

        # Wczytywanie Krasnoludków
        try:
            with open(path_krasnoludki, 'r', encoding='utf-8') as f:
                next(f) # Pomijamy nagłówek
                for linia in f:
                    dane = linia.strip().split(',')
                    if len(dane) >= 5:
                        # Zapisujemy ID jako klucz, a współrzędne domku (X, Y) jako wartość
                        krasnoludki[int(dane[0])] = (int(dane[3]), int(dane[4]))
        except Exception as e:
            print(f"Błąd podczas ładowania dane_krasnoludkow.csv: {e}")

        # Czyścimy scenę (przydatne, gdybyś chciał podpiąć tę funkcję pod przycisk "Odśwież")
        self.scene.clear()

        # RYSOWANIE TRAS (Zielone Linie) na podstawie pliku z C++
        try:
            with open(path_przydzialy, 'r') as f:
                for linia in f:
                    dane = linia.strip().split()
                    if len(dane) == 2:
                        id_krasnala = int(dane[0])
                        id_kopalni = int(dane[1])
                        
                        # Sprawdzamy czy mamy współrzędne obu punktów
                        if id_krasnala in krasnoludki and id_kopalni in kopalnie:
                            kx, ky = krasnoludki[id_krasnala]
                            mx, my = kopalnie[id_kopalni]
                            
                            # Rysujemy linię łączącą (kolor zielony, grubość 2px)
                            self.scene.addLine(kx, ky, mx, my, QPen(QColor(46, 204, 113), 2))
        except FileNotFoundError:
            print("BRAK PLIKU przydzialy.txt! Zrób najpierw 'make run' w C++, żeby wygenerować trasy.")

        # RYSOWANIE OBIEKTÓW NA WIERZCHU (żeby linie ich nie zasłaniały)
        
        # Rysuj Kopalnie (Czerwone Kwadraty)
        for m_id, (x, y) in kopalnie.items():
            # Rysujemy prostokąt: -10 centruje go względem współrzędnych
            self.scene.addRect(x - 10, y - 10, 20, 20, QPen(Qt.black), QBrush(QColor(231, 76, 60)))

        # Rysuj Domki Krasnoludków (Niebieskie Kółka)
        for k_id, (x, y) in krasnoludki.items():
            # Rysujemy kółko: -5 to połowa średnicy, by wycentrować
            self.scene.addEllipse(x - 5, y - 5, 10, 10, QPen(Qt.black), QBrush(QColor(52, 152, 219)))


if __name__ == "__main__":
    # Tworzymy główną aplikację
    app = QApplication(sys.argv)
    
    # Tworzymy i wyświetlamy nasze okno
    okno = GlowneOkno()
    okno.show()
    
    # Uruchamiamy główną pętlę zdarzeń
    sys.exit(app.exec())