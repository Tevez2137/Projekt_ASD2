import sys
import os
import subprocess
import math
from PySide6.QtWidgets import (QApplication, QMainWindow, QGraphicsScene, QMessageBox, QGraphicsView,
                               QDialog, QVBoxLayout, QFormLayout, QLineEdit, QDialogButtonBox, QLabel,
                               QFileDialog, QPushButton)
from PySide6.QtGui import QPen, QBrush, QColor, QPixmap
from PySide6.QtCore import Qt

from ui_interfejs import Ui_mainWindow

class OknoDodawaniaDomku(QDialog):
    def __init__(self, x, y, auto_id, parent=None):
        super().__init__(parent)
        self.setWindowTitle("Zamelduj nowego Krasnoludka")
        self.x, self.y, self.auto_id = int(x), int(y), auto_id

        layout = QVBoxLayout(self)
        form = QFormLayout()
        self.input_min = QLineEdit()
        self.input_min.setPlaceholderText("np. Zloto;Srebro")

        form.addRow("ID Krasnoludka:", QLabel(f"<b>{self.auto_id}</b> (Nadane automatycznie)"))
        form.addRow("Minerały:", self.input_min)
        form.addRow("Współrzędne domku:", QLabel(f"X: {self.x}, Y: {self.y}"))

        layout.addLayout(form)
        buttons = QDialogButtonBox(QDialogButtonBox.Ok | QDialogButtonBox.Cancel)
        buttons.accepted.connect(self.accept)
        buttons.rejected.connect(self.reject)
        layout.addWidget(buttons)

    def pobierz_dane(self):
        return {"id": str(self.auto_id), "id_kop": "0", "mineraly": self.input_min.text(), "x": str(self.x), "y": str(self.y)}

class GlowneOkno(QMainWindow):
    def __init__(self):
        super().__init__()
        self.ui = Ui_mainWindow()
        self.ui.setupUi(self)
        self.setWindowTitle("System Logistyki Krasnoludków")
        
        self.ostatni_atak = None 

        base_dir = os.path.dirname(__file__)
        data_dir = os.path.join(base_dir, "..", "data")
        project_dir = os.path.join(base_dir, "..")
        exe_name = "symulacja_krasnoludkow.exe" if sys.platform == "win32" else "symulacja_krasnoludkow"
        self.exe_path = os.path.join(project_dir, "build", exe_name)

        try:
            path_bin_kop = os.path.join(data_dir, "kopalnie.bin")
            if not os.path.exists(path_bin_kop) and os.path.exists(self.exe_path):
                test_kop = os.path.join(data_dir, "kopalnie_test.csv")
                test_kras = os.path.join(data_dir, "dane_krasnoludkow_test.csv")
                subprocess.run([self.exe_path, "IMPORT", test_kop, test_kras], cwd=project_dir, check=True)
        except Exception as e:
            print(f"Błąd inicjalizacji bazy BIN: {e}")

        style_path = os.path.join(base_dir, "style.qss")
        try:
            with open(style_path, "r", encoding="utf-8") as f: self.setStyleSheet(f.read())
        except FileNotFoundError: pass

        self.scene = QGraphicsScene()
        self.ui.map.setScene(self.scene)
        self.ui.map.setDragMode(QGraphicsView.ScrollHandDrag)
        self.ui.map.wheelEvent = self.obsluga_zooma
        
        if hasattr(self.ui, 'btnUruchomMCMF'): self.ui.btnUruchomMCMF.clicked.connect(self.uruchom_tylko_mcmf)
        if hasattr(self.ui, 'btnDodajDomek'): self.ui.btnDodajDomek.clicked.connect(self.aktywuj_tryb_dodawania)
        if hasattr(self.ui, 'odswiezMape'): self.ui.odswiezMape.clicked.connect(self.wczytaj_i_rysuj)
        if hasattr(self.ui, 'btn_atak'): self.ui.btn_atak.clicked.connect(self.obsluga_ataku_salwa)
        if hasattr(self.ui, 'btn_kompresja'): self.ui.btn_kompresja.clicked.connect(self.obsluga_kompresji_ksiegi)
        if hasattr(self.ui, 'btn_szukaj'): self.ui.btn_szukaj.clicked.connect(self.obsluga_wyszukiwania_ksiegi)
        
        if hasattr(self.ui, 'btn_import'): self.ui.btn_import.clicked.connect(self.import_danych)
        if hasattr(self.ui, 'btn_eksport'): self.ui.btn_eksport.clicked.connect(self.eksport_danych)
        if hasattr(self.ui, 'actionImport'): self.ui.actionImport.triggered.connect(self.import_danych)
        if hasattr(self.ui, 'actionEksport'): self.ui.actionEksport.triggered.connect(self.eksport_danych)

        self.tryb_dodawania = False
        self.oryginalny_mousePressEvent = self.ui.map.mousePressEvent
        self.ui.map.mousePressEvent = self.klikniecie_w_mape

        self.wczytaj_i_rysuj()

    def import_danych(self):
        data_dir = os.path.join(os.path.dirname(__file__), "..", "data")
        kopalnie_path, _ = QFileDialog.getOpenFileName(self, "Wybierz plik Kopalni (.csv)", data_dir, "CSV Files (*.csv)")
        krasnale_path, _ = QFileDialog.getOpenFileName(self, "Wybierz plik Krasnoludków (.csv)", data_dir, "CSV Files (*.csv)")
        
        if kopalnie_path and krasnale_path:
            try:
                subprocess.run([self.exe_path, "IMPORT", kopalnie_path, krasnale_path], cwd=os.path.join(os.path.dirname(__file__), ".."), check=True)
                self.wczytaj_i_rysuj()
                QMessageBox.information(self, "Import", "Dane CSV zostały pomyślnie zaimportowane do bazy BIN!")
            except Exception as e:
                QMessageBox.critical(self, "Błąd", f"Nie udało się zaimportować plików:\n{e}")

    def eksport_danych(self):
        katalog = QFileDialog.getExistingDirectory(self, "Wybierz folder do zapisu eksportu")
        if katalog:
            sciezka_kras = os.path.join(katalog, "wyeksportowane_krasnoludki.csv")
            sciezka_kop = os.path.join(katalog, "wyeksportowane_kopalnie.csv")
            try:
                subprocess.run([self.exe_path, "EKSPORT", sciezka_kras, sciezka_kop], cwd=os.path.join(os.path.dirname(__file__), ".."), check=True)
                QMessageBox.information(self, "Eksport", f"Baza BIN została wyeksportowana pomyślnie!\n\nZapisano pliki:\n- wyeksportowane_krasnoludki.csv\n- wyeksportowane_kopalnie.csv\nw folderze:\n{katalog}")
            except Exception as e:
                QMessageBox.critical(self, "Błąd", f"Eksport bazy nieudany:\n{e}")

    def obsluga_zooma(self, event):
        zoom = 1.15 if event.angleDelta().y() > 0 else (1 / 1.15)
        self.ui.map.scale(zoom, zoom)

    def obsluga_ataku_salwa(self):
        if hasattr(self.ui, 'spin_od') and hasattr(self.ui, 'spin_do') and hasattr(self.ui, 'spin_spacing'):
            self.ostatni_atak = (self.ui.spin_od.value(), self.ui.spin_do.value(), self.ui.spin_spacing.value())
            self.wczytaj_i_rysuj()

    def uruchom_tylko_mcmf(self):
        self.ostatni_atak = None
        path_akcja = os.path.join(os.path.dirname(__file__), "..", "data", "akcja_ksiegi.txt")
        if os.path.exists(path_akcja): os.remove(path_akcja)
        self.uruchom_silnik_cpp()

    def obsluga_kompresji_ksiegi(self):
        with open(os.path.join(os.path.dirname(__file__), "..", "data", "akcja_ksiegi.txt"), "w", encoding="utf-8") as f:
            f.write("KOMPRESJA")
        self.uruchom_tylko_ksiegi()

    def obsluga_wyszukiwania_ksiegi(self):  
        if hasattr(self.ui, 'input_szukaj'):
            fraza = self.ui.input_szukaj.text().strip()
            if not fraza:
                QMessageBox.warning(self, "Błąd", "Wpisz słowo, którego szukasz w księgach!")
                return
            base_dir = os.path.dirname(__file__)
            with open(os.path.join(base_dir, "..", "data", "akcja_ksiegi.txt"), "w", encoding="utf-8") as f: f.write("SZUKAJ")
            with open(os.path.join(base_dir, "..", "data", "wzorzec.txt"), "w", encoding="utf-8") as f: f.write(fraza)
            self.uruchom_tylko_ksiegi()

    def uruchom_tylko_ksiegi(self):
        try:
            if not os.path.exists(self.exe_path):
                QMessageBox.warning(self, "Brak silnika!", "Skompiluj projekt wpisując 'make' w terminalu.")
                return
            subprocess.run([self.exe_path, "KSIEGI"], cwd=os.path.join(os.path.dirname(__file__), ".."), check=True, capture_output=True, text=True)
            self.wczytaj_i_rysuj()
        except subprocess.CalledProcessError as e:
            QMessageBox.critical(self, "Błąd", f"Błąd przetwarzania ksiąg:\n{e.stderr}")

    def uruchom_silnik_cpp(self):
        if hasattr(self.ui, 'btnUruchomMCMF'):
            self.ui.btnUruchomMCMF.setText("Przetwarzam baze BIN...")
            self.ui.btnUruchomMCMF.setEnabled(False)
        QApplication.processEvents()

        try:
            if not os.path.exists(self.exe_path):
                QMessageBox.warning(self, "Brak silnika!", "Skompiluj projekt wpisując 'make' w terminalu.")
                return
            
            # WYWALONE MAKE RUN - ODPALAMY BEZPOSREDNIO ZBUDOWANA BINKE BEZ ARGUMENTOW (zeby wykonalo Graph::init())
            subprocess.run([self.exe_path], cwd=os.path.join(os.path.dirname(__file__), ".."), check=True, capture_output=True, text=True)
            
            self.wczytaj_i_rysuj()
            if hasattr(self.ui, 'statusbar'): self.ui.statusbar.showMessage("Obliczenia na bazie BIN zakończone!", 4000)
        except subprocess.CalledProcessError as e:
            QMessageBox.critical(self, "Błąd", f"Algorytm C++ napotkał błąd:\n{e.stderr}")
        finally:
            if hasattr(self.ui, 'btnUruchomMCMF'):
                self.ui.btnUruchomMCMF.setText("Oblicz Trasy Wydobycia (MCMF)")
                self.ui.btnUruchomMCMF.setEnabled(True)

    def aktywuj_tryb_dodawania(self):
        self.tryb_dodawania = True
        self.ui.map.setCursor(Qt.CrossCursor)
        if hasattr(self.ui, 'btnDodajDomek'):
            self.ui.btnDodajDomek.setText("Wybierz miejsce...")
            self.ui.btnDodajDomek.setEnabled(False)
        if hasattr(self.ui, 'statusbar'):
            self.ui.statusbar.showMessage("Kliknij na mapie, aby wskazać pozycję domku.", 6000)

    def pobierz_nastepne_id(self):
        max_id = 0
        try:
            result = subprocess.run([self.exe_path, "GUI_DATA_DUMP"], cwd=os.path.join(os.path.dirname(__file__), ".."), capture_output=True, text=True, check=True)
            kras_sekcja = False
            for linia in result.stdout.split("\n"):
                linia = linia.strip()
                if linia == "---KRASNOLUDKI---": kras_sekcja = True; continue
                if linia == "---OTOCZKA---": break
                if kras_sekcja and linia and not linia.startswith("ID"):
                    dane = linia.split(',')
                    if len(dane) > 0 and dane[0].lstrip('-').isdigit(): max_id = max(max_id, int(dane[0]))
        except Exception: pass
        return max_id + 1

    def klikniecie_w_mape(self, event):
        if self.tryb_dodawania and event.button() == Qt.LeftButton:
            pozycja_sceny = self.ui.map.mapToScene(event.position().toPoint())
            x, y = int(pozycja_sceny.x()), int(pozycja_sceny.y())

            self.tryb_dodawania = False
            self.ui.map.unsetCursor()
            if hasattr(self.ui, 'btnDodajDomek'):
                self.ui.btnDodajDomek.setText("Dodaj domek")
                self.ui.btnDodajDomek.setEnabled(True)

            self.otworz_modal_domku(x, y, self.pobierz_nastepne_id())
        else:
            self.oryginalny_mousePressEvent(event)

    def otworz_modal_domku(self, x, y, auto_id):
        okno = OknoDodawaniaDomku(x, y, auto_id, self)
        if okno.exec() == QDialog.Accepted:
            dane = okno.pobierz_dane()
            if dane["mineraly"]: self.zapisz_krasnala_do_csv(dane)
            else: QMessageBox.warning(self, "Błąd walidacji", "Pole 'Minerały' musi zostać wypełnione!")

    def zapisz_krasnala_do_csv(self, dane):
        try:
            subprocess.run([self.exe_path, "ADD_DWARF", str(dane['id']), str(dane['id_kop']), dane['mineraly'], str(dane['x']), str(dane['y'])],
                cwd=os.path.join(os.path.dirname(__file__), ".."), check=True, capture_output=True, text=True)
            if hasattr(self.ui, 'statusbar'): self.ui.statusbar.showMessage(f"Zarejestrowano w BIN (ID {dane['id']})", 4000)
            self.uruchom_silnik_cpp()
        except Exception as e:
            QMessageBox.critical(self, "Błąd zapisu", f"Nie udało się dopisać krasnoludka do bazy BIN:\n{e}")

    def wczytaj_i_rysuj(self):
        kopalnie = {}     
        krasnoludki = {}  
        straznicy = {}
        self.punkty_otoczki = []
        wynik_salwy = None
        highlight = None

        img_kop = {
            "Zloto": "kopalnia_zlota.png", "Diament": "kopalnia_diamentow.png", "Diamenty": "kopalnia_diamentow.png",
            "Srebro": "kopalnia_srebra.png", "Zelazo": "kopalnia_zelaza.png", "Wegiel": "kopalnia_wegla.png",
            "Miedz": "kopalnia_miedzi.png", "Rubin": "kopalnia_rubinow.png", "Rubiny": "kopalnia_rubinow.png"
        }
        img_kras = {
            "Zloto": "krasnoludek_zloto.png", "Diament": "krasnoludek_diament.png", "Diamenty": "krasnoludek_diament.png",
            "Srebro": "krasnoludek_srebro.png", "Zelazo": "krasnoludek_zelazny.png", "Wegiel": "krasnoludek_wegielny.png",
            "Miedz": "krasnoludek_miedziany.png", "Rubin": "krasnoludek_rubinowy.png", "Rubiny": "krasnoludek_rubinowy.png"
        }

        base_dir = os.path.dirname(__file__)
        img_dir = os.path.join(base_dir, "img")

        try:
            args = [self.exe_path, "GUI_DATA_DUMP"]
            if getattr(self, 'ostatni_atak', None):
                args.extend(["SALWA", str(self.ostatni_atak[0]), str(self.ostatni_atak[1]), str(self.ostatni_atak[2])])

            result = subprocess.run(args, cwd=os.path.join(base_dir, ".."), capture_output=True, text=True, check=True)
            
            kop_sekcja = kras_sekcja = otoczka_sekcja = salwa_sekcja = False
            straznicy_sekcja = False
            highlight_sekcja = False
            for linia in result.stdout.split("\n"):
                linia = linia.strip()
                if not linia: continue
                if linia == "---KOPALNIE---": kop_sekcja = True; kras_sekcja = otoczka_sekcja = salwa_sekcja = highlight_sekcja = straznicy_sekcja = False; continue
                if linia == "---KRASNOLUDKI---": kras_sekcja = True; kop_sekcja = otoczka_sekcja = salwa_sekcja = highlight_sekcja = straznicy_sekcja = False; continue
                if linia == "---OTOCZKA---": otoczka_sekcja = True; kop_sekcja = kras_sekcja = salwa_sekcja = highlight_sekcja = straznicy_sekcja = False; continue
                if linia == "---STRAZNICY---": straznicy_sekcja = True; kop_sekcja = kras_sekcja = otoczka_sekcja = salwa_sekcja = highlight_sekcja = False; continue
                if linia == "---SALWA---": salwa_sekcja = True; kop_sekcja = kras_sekcja = otoczka_sekcja = highlight_sekcja = straznicy_sekcja = False; continue
                if linia == "---HIGHLIGHT---": highlight_sekcja = True; kop_sekcja = kras_sekcja = otoczka_sekcja = salwa_sekcja = straznicy_sekcja = False; continue
                
                if kop_sekcja and not linia.startswith("ID"):
                    dane = linia.split(',')
                    if len(dane) >= 4 and dane[0].lstrip('-').isdigit():
                        kopalnie[int(dane[0])] = {"x": int(dane[1]), "y": int(dane[2]), "surowiec": dane[3], "miejsca": dane[4]}
                elif kras_sekcja and not linia.startswith("ID"):
                    dane = linia.split(',')
                    if len(dane) >= 5 and dane[0].lstrip('-').isdigit():
                        id_kop = int(dane[1]) if dane[1].lstrip('-').isdigit() else 0
                        krasnoludki[int(dane[0])] = {"id_kop": id_kop, "mineraly": dane[2], "x": int(dane[3]), "y": int(dane[4])}
                elif otoczka_sekcja:
                    coords = linia.split(',')
                    if len(coords) == 2 and coords[0].lstrip('-').isdigit():
                        self.punkty_otoczki.append((int(coords[0]), int(coords[1])))
                elif straznicy_sekcja:
                    dane = linia.split(',')
                    if len(dane) == 4 and dane[0].lstrip('-').isdigit():
                        straznicy[int(dane[0])] = {"x": int(dane[1]), "y": int(dane[2]), "glosnosc": int(dane[3])}
                elif salwa_sekcja:
                    dane = linia.split()
                    if len(dane) == 3: wynik_salwy = (int(dane[0]), int(dane[1]), dane[2])
                elif highlight_sekcja:
                    parts = linia.split(',')
                    if len(parts) >= 4 and parts[0].lstrip('-').isdigit():
                        try:
                            hid = int(parts[0]); hx = int(parts[1]); hy = int(parts[2]); hcol = parts[3]
                            highlight = (hid, hx, hy, hcol)
                        except: pass
        except Exception as e: print(f"Błąd parsera mapy z C++: {e}")

        self.scene.clear()

        for k_id, k_data in krasnoludki.items():
            id_kop = k_data["id_kop"]
            if id_kop > 0 and id_kop in kopalnie:
                self.scene.addLine(k_data["x"], k_data["y"], kopalnie[id_kop]["x"], kopalnie[id_kop]["y"], QPen(QColor(46, 204, 113), 2))

        if len(self.punkty_otoczki) > 0:
            max_indeks = len(self.punkty_otoczki) - 1
        else:
            max_indeks = 0

        if hasattr(self.ui, 'spin_od'): self.ui.spin_od.setRange(0, max_indeks)
        if hasattr(self.ui, 'spin_do'): self.ui.spin_do.setRange(0, max_indeks)

        if len(self.punkty_otoczki) > 1:
            pen_otoczka = QPen(QColor(155, 89, 182), 3)
            pen_otoczka.setStyle(Qt.DashLine)
            for i in range(len(self.punkty_otoczki)):
                p1 = self.punkty_otoczki[i]
                p2 = self.punkty_otoczki[(i + 1) % len(self.punkty_otoczki)]
                self.scene.addLine(p1[0], p1[1], p2[0], p2[1], pen_otoczka)

        # 3. Rysowanie Strażników / Dekametrowców na trasie patrolowej
        for s_id, data in straznicy.items():
            możliwe_ikony = ["dekametrowiec.png", "dekametrowiecc.png"]
            pixmap_straznik = QPixmap()
            for nazwa in możliwe_ikony:
                sciezka = os.path.join(img_dir, nazwa)
                if os.path.exists(sciezka):
                    pix = QPixmap(sciezka)
                    if not pix.isNull():
                        pixmap_straznik = pix
                        break

            tooltip_straznik = f"<b>Dekametrowiec ID:</b> {s_id}<br><b>Głośność:</b> {data['glosnosc']}"

            if not pixmap_straznik.isNull():
                pixmap_straznik = pixmap_straznik.scaled(50, 50, Qt.KeepAspectRatio, Qt.SmoothTransformation)
                item = self.scene.addPixmap(pixmap_straznik)
                item.setPos(data['x'] - pixmap_straznik.width() / 2, data['y'] - pixmap_straznik.height() / 2)
                item.setToolTip(tooltip_straznik)
                item.setZValue(10)
                item.setAcceptHoverEvents(True)
                item.setAcceptedMouseButtons(Qt.NoButton)
            else:
                pen_straznik = QPen(QColor(142, 68, 173), 2)
                brush_straznik = QBrush(QColor(142, 68, 173, 100))
                ellipse = self.scene.addEllipse(data['x'] - 10, data['y'] - 10, 20, 20, pen_straznik, brush_straznik)
                ellipse.setToolTip(tooltip_straznik)
                ellipse.setZValue(10)
                ellipse.setAcceptHoverEvents(True)
                ellipse.setAcceptedMouseButtons(Qt.NoButton)

            label = self.scene.addText(f"D{s_id - 1000}")
            label.setDefaultTextColor(QColor(142, 68, 173))
            font = label.font(); font.setBold(True); font.setPointSize(8); label.setFont(font)
            label.setPos(data['x'] + 18, data['y'] - 18)
            label.setZValue(12)

        # 4. Rysowanie Kopalń (ikony PNG)
        for m_id, data in kopalnie.items():
            surowiec = data['surowiec']
            nazwa_pliku = img_kop.get(surowiec, "kopalnia_pusta.png")
            sciezka_img = os.path.join(img_dir, nazwa_pliku)
            
            pixmap = QPixmap(sciezka_img)
            tooltip_tekst = f"<b>Kopalnia ID:</b> {m_id}<br><b>Surowiec:</b> {surowiec}<br><b>Miejsca:</b> {data['miejsca']}"
            
            if not pixmap.isNull():
                pixmap = pixmap.scaled(100, 100, Qt.KeepAspectRatio, Qt.SmoothTransformation)
                item = self.scene.addPixmap(pixmap)
                item.setPos(data["x"] - pixmap.width() / 2, data["y"] - pixmap.height() / 2)
                item.setToolTip(tooltip_tekst)
                hover_rect = self.scene.addRect(data["x"] - 48, data["y"] - 48, 96, 96, QPen(Qt.NoPen), QBrush(Qt.transparent))
                hover_rect.setToolTip(tooltip_tekst)
                hover_rect.setZValue(1)
                hover_rect.setAcceptHoverEvents(True)
                hover_rect.setAcceptedMouseButtons(Qt.NoButton)
            else:
                rect = self.scene.addRect(data["x"] - 10, data["y"] - 10, 20, 20, QPen(Qt.black), QBrush(QColor(231, 76, 60)))
                rect.setToolTip(tooltip_tekst)

        for k_id, data in krasnoludki.items():
            id_kop = data['id_kop']
            nazwa_pliku = "kransoludek_nieaktywny.png" 
            
            if id_kop > 0 and id_kop in kopalnie:
                surowiec_kopalni = kopalnie[id_kop]['surowiec']
                nazwa_pliku = img_kras.get(surowiec_kopalni, "kransoludek_nieaktywny.png")
                
            sciezka_img = os.path.join(img_dir, nazwa_pliku)
            pixmap = QPixmap(sciezka_img)
            
            mineraly_format = data['mineraly'].replace(";", ", ")
            tooltip = f"<b>Krasnoludek ID:</b> {k_id}<br><b>Lubi:</b> {mineraly_format}"
            tooltip += f"<br><b>Pracuje w:</b> Kopalnia nr {id_kop}" if id_kop > 0 else "<br><i>Brak przypisania!</i>"
            
            if not pixmap.isNull():
                pixmap = pixmap.scaled(100, 100, Qt.KeepAspectRatio, Qt.SmoothTransformation)
                item = self.scene.addPixmap(pixmap)
                item.setPos(data["x"] - pixmap.width() / 2, data["y"] - pixmap.height() / 2)
                item.setToolTip(tooltip)
                hover_rect = self.scene.addRect(data["x"] - 48, data["y"] - 48, 96, 96, QPen(Qt.NoPen), QBrush(Qt.transparent))
                hover_rect.setToolTip(tooltip)
                hover_rect.setZValue(1)
                hover_rect.setAcceptHoverEvents(True)
                hover_rect.setAcceptedMouseButtons(Qt.NoButton)
            else:
                ellipse = self.scene.addEllipse(data["x"] - 5, data["y"] - 5, 10, 10, QPen(Qt.black), QBrush(QColor(52, 152, 219)))
                ellipse.setToolTip(tooltip)

        if wynik_salwy:
            l_idx, r_idx, dowodca = wynik_salwy
            if len(self.punkty_otoczki) > 0:
                n = len(self.punkty_otoczki)
                pen_atak = QPen(QColor(255, 0, 0), 6)
                kroki = r_idx - l_idx if r_idx >= l_idx else (n - l_idx + r_idx)
                for step in range(kroki):
                    idx_obecny = (l_idx + step) % n
                    p1, p2 = self.punkty_otoczki[idx_obecny], self.punkty_otoczki[(idx_obecny + 1) % n]
                    self.scene.addLine(p1[0], p1[1], p2[0], p2[1], pen_atak)
                
                dow_pt = self.punkty_otoczki[l_idx % n]
                self.scene.addEllipse(dow_pt[0] - 20, dow_pt[1] - 20, 40, 40, QPen(QColor(255,0,0), 3))
                tekst = self.scene.addText(f"DOWÓDCA ID: {dowodca}")
                tekst.setDefaultTextColor(QColor(255, 0, 0))
                font = tekst.font(); font.setBold(True); font.setPointSize(12); tekst.setFont(font)
                tekst.setPos(dow_pt[0] + 15, dow_pt[1] - 30)
            
            QMessageBox.warning(self, "Atak!", f"Atak na odcinek {l_idx}-{r_idx}!\nRozkaz wydaje Dekametrowiec ID: {dowodca}")
            self.ostatni_atak = None 

        if highlight:
            hid, hx, hy, hcol = highlight
            try: col = QColor(hcol)
            except: col = QColor(255, 105, 180)
            pen_h = QPen(col, 4)
            brush_h = QBrush(QColor(col.red(), col.green(), col.blue(), 40))
            circ = self.scene.addEllipse(hx - 18, hy - 18, 36, 36, pen_h, brush_h)
            circ.setZValue(100)
            label = self.scene.addText(f"#{hid}")
            label.setDefaultTextColor(col)
            font = label.font(); font.setBold(True); font.setPointSize(10); label.setFont(font)
            label.setPos(hx + 20, hy - 10)

        # --- STATYSTYKI OBLICZANE Z DANYCH BIEŻĄCYCH ---
        aktywne_kopalnie_id = set()
        pracujacy_krasnale = 0
        wydobycie_surowcow = {}
        wymagana_owsianka = 0  

        for k_id, k_data in krasnoludki.items():
            id_kop = k_data["id_kop"]
            if id_kop > 0 and id_kop in kopalnie:
                aktywne_kopalnie_id.add(id_kop)
                pracujacy_krasnale += 1
                sur = kopalnie[id_kop]["surowiec"]
                wydobycie_surowcow[sur] = wydobycie_surowcow.get(sur, 0) + 1
                dx = k_data["x"] - kopalnie[id_kop]["x"]
                dy = k_data["y"] - kopalnie[id_kop]["y"]
                wymagana_owsianka += round(math.hypot(dx, dy))

        dystans_otoczki = 0
        n_ot = len(self.punkty_otoczki)
        if n_ot > 1:
            for i in range(n_ot):
                p1 = self.punkty_otoczki[i]
                p2 = self.punkty_otoczki[(i + 1) % n_ot]
                dystans_otoczki += math.hypot(p1[0] - p2[0], p1[1] - p2[1])
            if n_ot == 2: dystans_otoczki *= 2 

        html_statystyki = f"""
        <h3 style='margin-top: 0; margin-bottom: 5px; color: #f1c40f;'>📊 Raport Królestwa</h3>
        <b>Pracujący krasnale:</b> {pracujacy_krasnale}<br>
        <b>Aktywne kopalnie:</b> {len(aktywne_kopalnie_id)}<br>
        <b>Zużycie Owsianki:</b> {wymagana_owsianka} miseczek<br>
        <b>Trasa Patrolu (Mur):</b> {round(dystans_otoczki)} km<br>
        <hr style='border: 1px solid #7f8c8d; margin: 5px 0;'>
        <b>Wydobycie surowców:</b><br>
        """
        if wydobycie_surowcow:
            for sur, ilosc in sorted(wydobycie_surowcow.items()): html_statystyki += f"• {sur}: {ilosc} krasnali<br>"
        else: html_statystyki += "<i>Brak aktywnego wydobycia</i>"

        if hasattr(self.ui, 'panel_statystyk'): self.ui.panel_statystyk.setText(html_statystyki)

        # 6. ODCZYT KSIĄG
        path_wyniki_ksiegi = os.path.join(base_dir, "..", "data", "wyniki_ksiegi.txt")
        path_akcja = os.path.join(base_dir, "..", "data", "akcja_ksiegi.txt")
        if os.path.exists(path_wyniki_ksiegi):
            try:
                with open(path_wyniki_ksiegi, 'r', encoding='utf-8') as f:
                    akcja = f.readline().strip()
                    if akcja == "KOMPRESJA":
                        stats = f.readline().strip().split()
                        if len(stats) == 3:
                            msg_text = f"<h3 style='color:#d4af37;'>📝 KOMPRESJA ZAKOŃCZONA</h3><p>Oryginał: <b>{stats[0]} bitów</b><br>Po kompresji: <b>{stats[1]} bitów</b><br>Zaoszczędzono: <b><span style='color:#2ecc71;'>{float(stats[2]):.2f}%</span></b></p>"
                            if hasattr(self.ui, 'textBrowser_ksiegi'): self.ui.textBrowser_ksiegi.setHtml(msg_text)
                    elif akcja == "SZUKAJ":
                        liczba_znalezien = f.readline().strip()
                        pozycje = f.readline().strip()
                        msg_text = f"<h3 style='color:#d4af37;'>🔍 WYSZUKIWANIE ZAKOŃCZONE</h3><p>Znaleziono <b>{liczba_znalezien}</b> pasujących fragmentów.</p>"
                        if int(liczba_znalezien) > 0: msg_text += f"<p>Indeksy: {pozycje}</p>"
                        if hasattr(self.ui, 'textBrowser_ksiegi'): self.ui.textBrowser_ksiegi.setHtml(msg_text)
                os.remove(path_wyniki_ksiegi)
                if os.path.exists(path_akcja): os.remove(path_akcja)
            except Exception as e: print(f"Błąd odczytu księgi: {e}")     

if __name__ == "__main__":
    app = QApplication(sys.argv)
    okno = GlowneOkno()
    okno.show()
    sys.exit(app.exec())