import sys
import os
import subprocess
import shutil
from PySide6.QtWidgets import (QApplication, QMainWindow, QGraphicsScene, QMessageBox, QGraphicsView,
                               QDialog, QVBoxLayout, QFormLayout, QLineEdit, QDialogButtonBox, QLabel, QSpinBox,
                               QFileDialog)
from PySide6.QtGui import QPen, QBrush, QColor
from PySide6.QtCore import Qt

from ui_interfejs import Ui_mainWindow

# =========================================================
# OKNO DIALOGOWE (MODAL) DO WPROWADZANIA DANYCH KRASNALA
# =========================================================
class OknoDodawaniaDomku(QDialog):
    def __init__(self, x, y, auto_id, parent=None):
        super().__init__(parent)
        self.setWindowTitle("Zamelduj nowego Krasnoludka")
        
        self.x = int(x)
        self.y = int(y)
        self.auto_id = auto_id

        layout = QVBoxLayout(self)
        form = QFormLayout()

        self.input_min = QLineEdit()
        self.input_min.setPlaceholderText("np. Zloto;Srebro")

        # Okienko wyświetla automatyczne ID i nie pyta o Kopalnię
        form.addRow("ID Krasnoludka:", QLabel(f"<b>{self.auto_id}</b> (Nadane automatycznie)"))
        form.addRow("Minerały:", self.input_min)
        form.addRow("Współrzędne domku:", QLabel(f"X: {self.x}, Y: {self.y}"))

        layout.addLayout(form)

        buttons = QDialogButtonBox(QDialogButtonBox.Ok | QDialogButtonBox.Cancel)
        buttons.accepted.connect(self.accept)
        buttons.rejected.connect(self.reject)
        layout.addWidget(buttons)

    def pobierz_dane(self):
        return {
            "id": str(self.auto_id),
            "id_kop": "0", # Domyślnie 0. Zostanie zaktualizowane przez C++ MCMF.
            "mineraly": self.input_min.text(),
            "x": str(self.x),
            "y": str(self.y)
        }

# =========================================================
# GŁÓWNA KLASA APLIKACJI (INTERFEJS GRAFICZNY)
# =========================================================
class GlowneOkno(QMainWindow):
    def __init__(self):
        super().__init__()
        
        self.ui = Ui_mainWindow()
        self.ui.setupUi(self)
        self.setWindowTitle("System Logistyki Krasnoludków")

        # === SYSTEM WYBORU DOWOLNYCH PLIKÓW ===
        base_dir = os.path.dirname(__file__)
        data_dir = os.path.join(base_dir, "..", "data")

        msg = QMessageBox()
        msg.setWindowTitle("Wybór danych wejściowych")
        msg.setText("Czy chcesz wczytać domyślne pliki testowe, czy wybrać własne z dysku?")
        btn_domyslne = msg.addButton("Użyj domyślnych", QMessageBox.AcceptRole)
        btn_wlasne = msg.addButton("Wybierz własne z dysku...", QMessageBox.ActionRole)
        msg.exec()

        kopalnie_path = ""
        krasnale_path = ""

        if msg.clickedButton() == btn_wlasne:
            # Otwiera systemowe okno wyboru dowolnego pliku!
            kopalnie_path, _ = QFileDialog.getOpenFileName(self, "Wybierz plik Kopalni (.csv)", data_dir, "CSV Files (*.csv);;All Files (*)")
            krasnale_path, _ = QFileDialog.getOpenFileName(self, "Wybierz plik Krasnoludków (.csv)", data_dir, "CSV Files (*.csv);;All Files (*)")

        # Jeśli użytkownik nie wybrał plików lub kliknął "Domyślne", ładujemy pliki testowe
        if not kopalnie_path or not krasnale_path:
            kopalnie_path = os.path.join(data_dir, "kopalnie_test.csv")
            krasnale_path = os.path.join(data_dir, "dane_krasnoludkow_test.csv")

        # Kopiujemy wybrane pliki jako pliki "aktywne" (dzięki temu C++ zawsze wie, co czytać)
        try:
            shutil.copy(kopalnie_path, os.path.join(data_dir, "kopalnie_aktywne.csv"))
            shutil.copy(krasnale_path, os.path.join(data_dir, "dane_krasnoludkow_aktywne.csv"))
        except Exception as e:
            print(f"Błąd kopiowania plików aktywnych: {e}")
        # ======================================
        
        self.ui.pushButton_2.setText("Uruchom Algorytm MCMF")
        
        style_path = os.path.join(os.path.dirname(__file__), "style.qss")
        try:
            with open(style_path, "r", encoding="utf-8") as f:
                self.setStyleSheet(f.read())
        except FileNotFoundError:
            pass
        
        self.scene = QGraphicsScene()
        self.ui.map.setScene(self.scene)
        self.ui.map.setDragMode(QGraphicsView.ScrollHandDrag)
        self.ui.map.wheelEvent = self.obsluga_zooma
        
        self.ui.pushButton_2.clicked.connect(self.uruchom_silnik_cpp)
        self.ui.btnDodajDomek.clicked.connect(self.aktywuj_tryb_dodawania)
        self.ui.odswiezMape.clicked.connect(self.wczytaj_i_rysuj)

        self.tryb_dodawania = False
        self.oryginalny_mousePressEvent = self.ui.map.mousePressEvent
        self.ui.map.mousePressEvent = self.klikniecie_w_mape

        # --- PROBLEM 3: SALWA ---
        self.ui.pushButton_2.setText("Symuluj Atak")
        self.spin_od = QSpinBox()
        self.spin_do = QSpinBox()
        self.spin_od.setRange(0, 9)
        self.spin_do.setRange(0, 9)
        self.spin_od.setValue(1)
        self.spin_do.setValue(4)
        
        self.ui.navbar.addWidget(QLabel("Atak od:"), 4, 0)
        self.ui.navbar.addWidget(self.spin_od, 5, 0)
        self.ui.navbar.addWidget(QLabel("Atak do:"), 6, 0)
        self.ui.navbar.addWidget(self.spin_do, 7, 0)
        self.ui.pushButton_2.clicked.connect(self.obsluga_ataku_salwa)
        # ------------------------

        # Czyścimy stary wynik ataku (jeśli został z poprzedniego uruchomienia aplikacji)
        path_salwa = os.path.join(os.path.dirname(__file__), "..", "data", "wyniki_salwa.txt")
        if os.path.exists(path_salwa):
            os.remove(path_salwa)

        # Od razu ładujemy to, co jest obecnie w plikach
        self.wczytaj_i_rysuj()

    def obsluga_zooma(self, event):
        zoom_in_factor = 1.15
        zoom_out_factor = 1 / zoom_in_factor

        if event.angleDelta().y() > 0:
            zoom_factor = zoom_in_factor
        else:
            zoom_factor = zoom_out_factor

        self.ui.map.scale(zoom_factor, zoom_factor)

    def obsluga_ataku_salwa(self):
        lewy, prawy = self.spin_od.value(), self.spin_do.value()
        if lewy > prawy: return
        
        sciezka = os.path.join(os.path.dirname(__file__), "..", "data", "atak.txt")
        with open(sciezka, "w") as f:
            f.write(f"{lewy} {prawy}\n")
            
        self.uruchom_silnik_cpp()

    def uruchom_silnik_cpp(self):
        self.ui.pushButton_2.setText("Przetwarzam...")
        self.ui.pushButton_2.setEnabled(False)
        QApplication.processEvents()

        try:
            base_dir = os.path.dirname(__file__)
            project_dir = os.path.join(base_dir, "..")
            
            subprocess.run(
                ["make", "run"], 
                cwd=project_dir, 
                check=True, 
                capture_output=True, 
                text=True
            )
            
            self.wczytaj_i_rysuj()
            self.ui.statusbar.showMessage("Obliczenia zakończone sukcesem!", 4000)

        except subprocess.CalledProcessError as e:
            QMessageBox.critical(self, "Błąd", f"Algorytm C++ napotkał błąd:\n{e.stderr}")
        except FileNotFoundError:
            QMessageBox.critical(self, "Błąd", "Nie znaleziono polecenia 'make'.")
        finally:
            self.ui.pushButton_2.setText("Uruchom Algorytm MCMF")
            self.ui.pushButton_2.setEnabled(True)

    def aktywuj_tryb_dodawania(self):
        self.tryb_dodawania = True
        self.ui.map.setCursor(Qt.CrossCursor)
        self.ui.btnDodajDomek.setText("Wybierz miejsce...")
        self.ui.btnDodajDomek.setEnabled(False)
        self.ui.statusbar.showMessage("Kliknij lewym przyciskiem myszy na mapie, aby wskazać pozycję domku.", 6000)

    def pobierz_nastepne_id(self):
        """Przeszukuje plik CSV w poszukiwaniu najwyższego nadanego ID i zwraca ID + 1."""
        base_dir = os.path.dirname(__file__)
        path = os.path.join(base_dir, "..", "data", "dane_krasnoludkow.csv")
        max_id = 0
        try:
            with open(path, 'r', encoding='utf-8') as f:
                next(f) # Pomiń nagłówek
                for linia in f:
                    dane = linia.strip().split(',')
                    if len(dane) > 0 and dane[0].isdigit():
                        max_id = max(max_id, int(dane[0]))
        except FileNotFoundError:
            pass
        return max_id + 1

    def klikniecie_w_mape(self, event):
        if self.tryb_dodawania and event.button() == Qt.LeftButton:
            pozycja_sceny = self.ui.map.mapToScene(event.pos())
            x, y = int(pozycja_sceny.x()), int(pozycja_sceny.y())

            self.tryb_dodawania = False
            self.ui.map.unsetCursor()
            self.ui.btnDodajDomek.setText("Dodaj domek")
            self.ui.btnDodajDomek.setEnabled(True)

            auto_id = self.pobierz_nastepne_id()
            self.otworz_modal_domku(x, y, auto_id)
        else:
            self.oryginalny_mousePressEvent(event)

    def otworz_modal_domku(self, x, y, auto_id):
        okno = OknoDodawaniaDomku(x, y, auto_id, self)
        if okno.exec() == QDialog.Accepted:
            dane = okno.pobierz_dane()
            if dane["mineraly"]:
                self.zapisz_krasnala_do_csv(dane)
            else:
                QMessageBox.warning(self, "Błąd walidacji", "Pole 'Minerały' musi zostać wypełnione!")

    def zapisz_krasnala_do_csv(self, dane):
        base_dir = os.path.dirname(__file__)
        path = os.path.join(base_dir, "..", "data", "dane_krasnoludkow.csv")
        try:
            with open(path, 'a', encoding='utf-8') as f:
                linia = f"{dane['id']},{dane['id_kop']},{dane['mineraly']},{dane['x']},{dane['y']}\n"
                f.write(linia)
            
            self.ui.statusbar.showMessage(f"Zarejestrowano krasnoludka o ID {dane['id']}. Przeliczam trasy...", 4000)
            self.uruchom_silnik_cpp()

        except Exception as e:
            QMessageBox.critical(self, "Błąd zapisu", f"Nie udało się dopisać krasnoludka do bazy CSV:\n{e}")

    def wczytaj_i_rysuj(self):
        base_dir = os.path.dirname(__file__)
        path_kopalnie = os.path.join(base_dir, "..", "data", "kopalnie_aktywne.csv")
        path_krasnoludki = os.path.join(base_dir, "..", "data", "dane_krasnoludkow_aktywne.csv")
        path_otoczka = os.path.join(base_dir, "..", "data", "otoczka.txt")

        kopalnie = {}     
        krasnoludki = {}  

        # Wczytywanie kopalń
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
            print(f"Błąd ładowania kopalń: {e}")

        # Wczytywanie krasnoludków
        try:
            with open(path_krasnoludki, 'r', encoding='utf-8') as f:
                next(f)
                for linia in f:
                    dane = linia.strip().split(',')
                    if len(dane) >= 5:
                        id_krasnala = int(dane[0])
                        id_kop = int(dane[1]) if dane[1].isdigit() else 0
                        krasnoludki[id_krasnala] = {
                            "id_kop": id_kop, 
                            "mineraly": dane[2],
                            "x": int(dane[3]), 
                            "y": int(dane[4])
                        }
        except Exception as e:
            print(f"Błąd ładowania krasnoludków: {e}")

        self.scene.clear()

        # 1. RYSOWANIE LINII PRZYDZIAŁÓW Bezpośrednio z CSV!
        for k_id, k_data in krasnoludki.items():
            id_kop = k_data["id_kop"]
            if id_kop > 0 and id_kop in kopalnie:
                kx, ky = k_data["x"], k_data["y"]
                mx, my = kopalnie[id_kop]["x"], kopalnie[id_kop]["y"]
                self.scene.addLine(kx, ky, mx, my, QPen(QColor(46, 204, 113), 2))

        # 2. RYSOWANIE TRASY PATROLOWEJ KSIĘCIA
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

        # 3. RYSOWANIE KOPALŃ (Czerwone kwadraty)
        for m_id, data in kopalnie.items():
            rect = self.scene.addRect(data["x"] - 10, data["y"] - 10, 20, 20, QPen(Qt.black), QBrush(QColor(231, 76, 60)))
            rect.setToolTip(f"<b>Kopalnia ID:</b> {m_id}<br><b>Surowiec:</b> {data['surowiec']}<br><b>Miejsca:</b> {data['miejsca']}")

        # 4. RYSOWANIE DOMKÓW KRASNOLUDKÓW (Niebieskie okręgi)
        for k_id, data in krasnoludki.items():
            ellipse = self.scene.addEllipse(data["x"] - 5, data["y"] - 5, 10, 10, QPen(Qt.black), QBrush(QColor(52, 152, 219)))
            mineraly_format = data['mineraly'].replace(";", ", ")
            
            # W tooltipie podajemy też aktualny przydział
            tooltip = f"<b>Krasnoludek ID:</b> {k_id}<br><b>Lubi:</b> {mineraly_format}"
            if data['id_kop'] > 0:
                tooltip += f"<br><b>Pracuje w:</b> Kopalnia nr {data['id_kop']}"
            else:
                tooltip += "<br><i>Brak przypisania!</i>"
            ellipse.setToolTip(tooltip)

    # --- PROBLEM 3: WYNIK SALWY ---
        path_salwa = os.path.join(base_dir, "..", "data", "wyniki_salwa.txt")
        if os.path.exists(path_salwa):
            with open(path_salwa, 'r') as f:
                dane = f.readline().strip().split()
                if len(dane) == 3:
                    QMessageBox.warning(self, "Atak!", f"Atak na odcinek {dane[0]}-{dane[1]}!\nRozkaz wydaje Dekametrowiec ID: {dane[2]}")
            os.remove(path_salwa)

if __name__ == "__main__":
    app = QApplication(sys.argv)
    okno = GlowneOkno()
    okno.show()
    sys.exit(app.exec())