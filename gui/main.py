import sys
import os
import subprocess
from PySide6.QtWidgets import (QApplication, QMainWindow, QGraphicsScene, QMessageBox, QGraphicsView,
                               QDialog, QVBoxLayout, QFormLayout, QLineEdit, QDialogButtonBox, QLabel)
from PySide6.QtGui import QPen, QBrush, QColor
from PySide6.QtCore import Qt

from ui_interfejs import Ui_mainWindow

# =========================================================
# NOWA KLASA: Okienko (Modal) do wpisywania danych krasnala
# =========================================================
class OknoDodawaniaDomku(QDialog):
    def __init__(self, x, y, parent=None):
        super().__init__(parent)
        self.setWindowTitle("Zamelduj nowego Krasnoludka")
        
        # Zapisujemy odebrane współrzędne
        self.x = int(x)
        self.y = int(y)

        layout = QVBoxLayout(self)
        form = QFormLayout()

        # Pola tekstowe
        self.input_id = QLineEdit()
        self.input_id.setPlaceholderText("np. 101")
        
        self.input_kop = QLineEdit()
        self.input_kop.setPlaceholderText("ID kopalni (opcjonalne, domyślnie 0)")
        
        self.input_min = QLineEdit()
        self.input_min.setPlaceholderText("np. Zloto;Srebro")

        # Układamy etykiety i pola
        form.addRow("ID Krasnoludka:", self.input_id)
        form.addRow("Minerały:", self.input_min)
        form.addRow("ID Kopalni:", self.input_kop)
        form.addRow("Współrzędne na mapie:", QLabel(f"X: {self.x}, Y: {self.y}"))

        layout.addLayout(form)

        # Przyciski Zapisz / Anuluj
        buttons = QDialogButtonBox(QDialogButtonBox.Ok | QDialogButtonBox.Cancel)
        buttons.accepted.connect(self.accept)
        buttons.rejected.connect(self.reject)
        layout.addWidget(buttons)

    def pobierz_dane(self):
        """Zwraca słownik z danymi wpisanymi przez użytkownika."""
        return {
            "id": self.input_id.text(),
            "id_kop": self.input_kop.text() if self.input_kop.text() else "0",
            "mineraly": self.input_min.text(),
            "x": str(self.x),
            "y": str(self.y)
        }

# =========================================================
# GŁÓWNA KLASA APLIKACJI
# =========================================================
class GlowneOkno(QMainWindow):
    def __init__(self):
        super().__init__()
        
        self.ui = Ui_mainWindow()
        self.ui.setupUi(self)
        self.setWindowTitle("System Logistyki Krasnoludków")
        
        # Opcjonalne: ładowanie stylów QSS
        style_path = os.path.join(os.path.dirname(__file__), "style.qss")
        try:
            with open(style_path, "r", encoding="utf-8") as f:
                self.setStyleSheet(f.read())
        except FileNotFoundError:
            pass
        
        # 1. Inicjalizacja sceny
        self.scene = QGraphicsScene()
        self.ui.map.setScene(self.scene)
        self.ui.map.setDragMode(QGraphicsView.ScrollHandDrag)
        self.ui.map.wheelEvent = self.obsluga_zooma
        
        # 2. Podpięcie przycisków
        # self.ui.btn_start_algorytm.clicked.connect(self.uruchom_silnik_cpp) # Zakładam, że tu masz przycisk do C++
        self.ui.btnDodajDomek.clicked.connect(self.aktywuj_tryb_dodawania)
        self.ui.odswiezMape.clicked.connect(self.wczytaj_i_rysuj)

        # 3. Zmienna do śledzenia "Trybu budowania" i podpięcie własnego klikania myszką
        self.tryb_dodawania = False
        self.oryginalny_mousePressEvent = self.ui.map.mousePressEvent
        self.ui.map.mousePressEvent = self.klikniecie_w_mape

        # Ładujemy mapę na start
        self.wczytaj_i_rysuj()

    # --- NOWE FUNKCJE DO WSTAWIANIA DOMKU ---

    def aktywuj_tryb_dodawania(self):
        """Włącza tryb celownika - oczekuje na kliknięcie w mapę."""
        self.tryb_dodawania = True
        self.ui.map.setCursor(Qt.CrossCursor) # Zmienia kursor na celownik
        self.ui.btnDodajDomek.setText("Wybierz miejsce...")
        self.ui.btnDodajDomek.setEnabled(False)
        self.ui.statusbar.showMessage("Tryb budowania: Kliknij w dowolne miejsce na mapie!", 5000)

    def klikniecie_w_mape(self, event):
        """Przechwytuje kliknięcie na mapie."""
        if self.tryb_dodawania and event.button() == Qt.LeftButton:
            # Mapujemy koordynaty z okienka na wirtualne koordynaty "sceny" (mapy)
            pozycja_sceny = self.ui.map.mapToScene(event.pos())
            x, y = pozycja_sceny.x(), pozycja_sceny.y()

            # Wyłączamy tryb budowania i przywracamy domyślny kursor (łapkę)
            self.tryb_dodawania = False
            self.ui.map.unsetCursor()
            self.ui.btnDodajDomek.setText("Dodaj domek")
            self.ui.btnDodajDomek.setEnabled(True)

            # Odpalamy okienko z przekazanymi współrzędnymi
            self.otworz_modal_domku(x, y)
        else:
            # Jeśli nie jesteśmy w trybie dodawania, oddajemy sterowanie do normalnej łapki
            self.oryginalny_mousePressEvent(event)

    def otworz_modal_domku(self, x, y):
        """Uruchamia okienko do wpisania danych i wywołuje zapis, jeśli użytkownik kliknie OK."""
        okno = OknoDodawaniaDomku(x, y, self)
        if okno.exec() == QDialog.Accepted: # Jeśli użytkownik kliknął OK
            dane = okno.pobierz_dane()
            if dane["id"] and dane["mineraly"]: # Prosta walidacja czy nie są puste
                self.zapisz_krasnala_do_csv(dane)
            else:
                QMessageBox.warning(self, "Błąd", "ID i Minerały nie mogą być puste!")

    def zapisz_krasnala_do_csv(self, dane):
        """Otwiera plik i dopisuje linijkę."""
        base_dir = os.path.dirname(__file__)
        path = os.path.join(base_dir, "..", "data", "dane_krasnoludkow.csv")
        try:
            with open(path, 'a', encoding='utf-8') as f:
                # Zapisujemy w formacie: ID,ID_kopalni,Mineraly,X,Y
                linia = f"{dane['id']},{dane['id_kop']},{dane['mineraly']},{dane['x']},{dane['y']}\n"
                f.write(linia)
            
            # Odświeżamy mapę, żeby domek od razu się pojawił!
            self.wczytaj_i_rysuj() 
            self.ui.statusbar.showMessage("Pomyślnie dodano krasnoludka!", 3000)
            QMessageBox.information(self, "Zbudowano!", "Domek stoi! Uruchom ponownie algorytm C++, aby wyliczyć mu trasę i nową otoczkę.")
        except Exception as e:
            QMessageBox.critical(self, "Błąd", f"Nie udało się zapisać: {e}")

    # --- STARE FUNKCJE ---

    def obsluga_zooma(self, event):
        zoom_in_factor = 1.15
        zoom_out_factor = 1 / zoom_in_factor
        if event.angleDelta().y() > 0:
            zoom_factor = zoom_in_factor
        else:
            zoom_factor = zoom_out_factor
        self.ui.map.scale(zoom_factor, zoom_factor)

    def wczytaj_i_rysuj(self):
        base_dir = os.path.dirname(__file__)
        path_kopalnie = os.path.join(base_dir, "..", "data", "kopalnie.csv")
        path_krasnoludki = os.path.join(base_dir, "..", "data", "dane_krasnoludkow.csv")
        path_przydzialy = os.path.join(base_dir, "..", "data", "przydzialy.txt")
        path_otoczka = os.path.join(base_dir, "..", "data", "otoczka.txt")

        kopalnie = {}     
        krasnoludki = {}  

        try:
            with open(path_kopalnie, 'r', encoding='utf-8') as f:
                next(f)
                for linia in f:
                    dane = linia.strip().split(',')
                    if len(dane) >= 4:
                        kopalnie[int(dane[0])] = {
                            "x": int(dane[1]), "y": int(dane[2]),
                            "surowiec": dane[3], "miejsca": dane[4]
                        }
        except Exception as e:
            print(f"Błąd kopalnie: {e}")

        try:
            with open(path_krasnoludki, 'r', encoding='utf-8') as f:
                next(f)
                for linia in f:
                    dane = linia.strip().split(',')
                    if len(dane) >= 5:
                        krasnoludki[int(dane[0])] = {
                            "id_kop": dane[1], "mineraly": dane[2],
                            "x": int(dane[3]), "y": int(dane[4])
                        }
        except Exception as e:
            print(f"Błąd krasnoludki: {e}")

        self.scene.clear()

        # Rysowanie przydziałów (Zielone)
        try:
            with open(path_przydzialy, 'r') as f:
                for linia in f:
                    dane = linia.strip().split()
                    if len(dane) == 2:
                        id_krasnala = int(dane[0])
                        id_kopalni = int(dane[1])
                        
                        if id_krasnala in krasnoludki and id_kopalni in kopalnie:
                            kx, ky = krasnoludki[id_krasnala]["x"], krasnoludki[id_krasnala]["y"]
                            mx, my = kopalnie[id_kopalni]["x"], kopalnie[id_kopalni]["y"]
                            self.scene.addLine(kx, ky, mx, my, QPen(QColor(46, 204, 113), 2))
        except FileNotFoundError:
            pass

        # Rysowanie Otoczki (Fioletowa)
        try:
            punkty_otoczki = []
            with open(path_otoczka, 'r') as f:
                for linia in f:
                    dane = linia.strip().split()
                    if len(dane) == 2:
                        punkty_otoczki.append((int(dane[0]), int(dane[1])))

            if len(punkty_otoczki) > 1:
                pen_otoczka = QPen(QColor(155, 89, 182), 3) 
                pen_otoczka.setStyle(Qt.DashLine)
                for i in range(len(punkty_otoczki)):
                    p1 = punkty_otoczki[i]
                    p2 = punkty_otoczki[(i + 1) % len(punkty_otoczki)]
                    self.scene.addLine(p1[0], p1[1], p2[0], p2[1], pen_otoczka)
        except FileNotFoundError:
            pass

        # Rysowanie obiektów
        for m_id, data in kopalnie.items():
            rect = self.scene.addRect(data["x"] - 10, data["y"] - 10, 20, 20, QPen(Qt.black), QBrush(QColor(231, 76, 60)))
            rect.setToolTip(f"<b>Kopalnia ID:</b> {m_id}<br><b>Surowiec:</b> {data['surowiec']}<br><b>Miejsca:</b> {data['miejsca']}")

        for k_id, data in krasnoludki.items():
            ellipse = self.scene.addEllipse(data["x"] - 5, data["y"] - 5, 10, 10, QPen(Qt.black), QBrush(QColor(52, 152, 219)))
            mineraly_format = data['mineraly'].replace(";", ", ")
            ellipse.setToolTip(f"<b>Krasnoludek ID:</b> {k_id}<br><b>Lubi:</b> {mineraly_format}")

if __name__ == "__main__":
    app = QApplication(sys.argv)
    okno = GlowneOkno()
    okno.show()
    sys.exit(app.exec())