import sys
import os
import subprocess
from PySide6.QtWidgets import QApplication, QMainWindow, QGraphicsScene, QMessageBox, QGraphicsView
from PySide6.QtGui import QPen, QBrush, QColor
from PySide6.QtCore import Qt

from ui_interfejs import Ui_mainWindow

class GlowneOkno(QMainWindow):
    def __init__(self):
        super().__init__()
        
        self.ui = Ui_mainWindow()
        self.ui.setupUi(self)
        self.setWindowTitle("System Logistyki Krasnoludków")
        
        # 1. Inicjalizacja sceny
        self.scene = QGraphicsScene()
        self.ui.map.setScene(self.scene)
        
        # 2. Włączenie interakcji z mapą (łapka do przesuwania)
        self.ui.map.setDragMode(QGraphicsView.ScrollHandDrag)
        
        # 3. Dodanie obsługi przybliżania (Zoom) za pomocą kółka myszy
        self.ui.map.wheelEvent = self.obsluga_zooma
        
        # 4. Podpięcie przycisku do funkcji odpalającej C++
        # Upewnij się, że przycisk w UI nazywa się "btn_start_algorytm"
        #self.ui.btn_start_algorytm.clicked.connect(self.uruchom_silnik_cpp)

        # Od razu ładujemy to, co jest obecnie w plikach
        self.wczytaj_i_rysuj()

    def obsluga_zooma(self, event):
        """Funkcja pozwalająca na przybliżanie/oddalanie mapy kółkiem myszy."""
        zoom_in_factor = 1.15
        zoom_out_factor = 1 / zoom_in_factor

        # Sprawdzamy kierunek kręcenia kółkiem
        if event.angleDelta().y() > 0:
            zoom_factor = zoom_in_factor
        else:
            zoom_factor = zoom_out_factor

        self.ui.map.scale(zoom_factor, zoom_factor)

    def uruchom_silnik_cpp(self):
        """Funkcja odpalająca make run i rysująca nową mapę."""
        self.ui.btn_start_algorytm.setText("Przetwarzam...")
        self.ui.btn_start_algorytm.setEnabled(False)
        QApplication.processEvents() # Odświeża GUI, żeby pokazać zmianę tekstu na przycisku

        try:
            # Cofamy się z folderu gui do głównego folderu projektu i odpalamy make run
            base_dir = os.path.dirname(__file__)
            project_dir = os.path.join(base_dir, "..")
            
            # Subprocess odpala terminal w tle
            subprocess.run(
                ["make", "run"], 
                cwd=project_dir, 
                check=True, 
                capture_output=True, 
                text=True
            )
            
            # Po udanym obliczeniu, odświeżamy mapę
            self.wczytaj_i_rysuj()
            QMessageBox.information(self, "Sukces", "Optymalny przydział został obliczony i narysowany!")

        except subprocess.CalledProcessError as e:
            QMessageBox.critical(self, "Błąd", f"Algorytm C++ napotkał błąd:\n{e.stderr}")
        except FileNotFoundError:
            QMessageBox.critical(self, "Błąd", "Nie znaleziono polecenia 'make'. Upewnij się, że jesteś w środowisku, które je obsługuje (np. Git Bash).")
        finally:
            self.ui.btn_start_algorytm.setText("Uruchom Algorytm MCMF")
            self.ui.btn_start_algorytm.setEnabled(True)

    def wczytaj_i_rysuj(self):
        base_dir = os.path.dirname(__file__)
        path_kopalnie = os.path.join(base_dir, "..", "data", "kopalnie.csv")
        path_krasnoludki = os.path.join(base_dir, "..", "data", "dane_krasnoludkow.csv")
        path_przydzialy = os.path.join(base_dir, "..", "data", "przydzialy.txt")

        kopalnie = {}     # Słownik: ID -> {"x": x, "y": y, "surowiec": s, "miejsca": m}
        krasnoludki = {}  # Słownik: ID -> {"x": x, "y": y, "mineraly": m, "id_kop": id}

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
            print("BRAK PLIKU przydzialy.txt! Kliknij przycisk 'Uruchom Algorytm MCMF'.")

        for m_id, data in kopalnie.items():
            rect = self.scene.addRect(data["x"] - 10, data["y"] - 10, 20, 20, QPen(Qt.black), QBrush(QColor(231, 76, 60)))
            # Ustawienie Tooltipa
            rect.setToolTip(f"<b>Kopalnia ID:</b> {m_id}<br><b>Surowiec:</b> {data['surowiec']}<br><b>Miejsca:</b> {data['miejsca']}")

        for k_id, data in krasnoludki.items():
            ellipse = self.scene.addEllipse(data["x"] - 5, data["y"] - 5, 10, 10, QPen(Qt.black), QBrush(QColor(52, 152, 219)))
            # Ustawienie Tooltipa
            mineraly_format = data['mineraly'].replace(";", ", ")
            ellipse.setToolTip(f"<b>Krasnoludek ID:</b> {k_id}<br><b>Lubi:</b> {mineraly_format}")

if __name__ == "__main__":
    app = QApplication(sys.argv)
    okno = GlowneOkno()
    okno.show()
    sys.exit(app.exec())