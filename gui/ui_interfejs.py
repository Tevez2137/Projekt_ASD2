# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'mainWindow.ui'
##
## Created by: Qt User Interface Compiler version 6.11.0
##
## WARNING! All changes made in this file will be lost when recompiling UI file!
################################################################################

from PySide6.QtCore import (QCoreApplication, QDate, QDateTime, QLocale,
    QMetaObject, QObject, QPoint, QRect,
    QSize, QTime, QUrl, Qt)
from PySide6.QtGui import (QBrush, QColor, QConicalGradient, QCursor,
    QFont, QFontDatabase, QGradient, QIcon,
    QImage, QKeySequence, QLinearGradient, QPainter,
    QPalette, QPixmap, QRadialGradient, QTransform)
from PySide6.QtWidgets import (QApplication, QGraphicsView, QHBoxLayout, QLabel,
    QLineEdit, QMainWindow, QPushButton, QSizePolicy,
    QSpacerItem, QSpinBox, QStatusBar, QTabWidget,
    QTextBrowser, QVBoxLayout, QWidget)

class Ui_mainWindow(object):
    def setupUi(self, mainWindow):
        if not mainWindow.objectName():
            mainWindow.setObjectName(u"mainWindow")
        mainWindow.resize(1100, 750)
        self.centralwidget = QWidget(mainWindow)
        self.centralwidget.setObjectName(u"centralwidget")
        self.horizontalLayout = QHBoxLayout(self.centralwidget)
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.map = QGraphicsView(self.centralwidget)
        self.map.setObjectName(u"map")
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Expanding)
        sizePolicy.setHorizontalStretch(3)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.map.sizePolicy().hasHeightForWidth())
        self.map.setSizePolicy(sizePolicy)

        self.horizontalLayout.addWidget(self.map)

        self.tabWidget = QTabWidget(self.centralwidget)
        self.tabWidget.setObjectName(u"tabWidget")
        sizePolicy1 = QSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Expanding)
        sizePolicy1.setHorizontalStretch(1)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.tabWidget.sizePolicy().hasHeightForWidth())
        self.tabWidget.setSizePolicy(sizePolicy1)
        self.tab_logistyka = QWidget()
        self.tab_logistyka.setObjectName(u"tab_logistyka")
        self.verticalLayout_1 = QVBoxLayout(self.tab_logistyka)
        self.verticalLayout_1.setObjectName(u"verticalLayout_1")
        self.labelLogistykaTytul = QLabel(self.tab_logistyka)
        self.labelLogistykaTytul.setObjectName(u"labelLogistykaTytul")

        self.verticalLayout_1.addWidget(self.labelLogistykaTytul)

        self.btnDodajDomek = QPushButton(self.tab_logistyka)
        self.btnDodajDomek.setObjectName(u"btnDodajDomek")

        self.verticalLayout_1.addWidget(self.btnDodajDomek)

        self.odswiezMape = QPushButton(self.tab_logistyka)
        self.odswiezMape.setObjectName(u"odswiezMape")

        self.verticalLayout_1.addWidget(self.odswiezMape)

        self.btnUruchomMCMF = QPushButton(self.tab_logistyka)
        self.btnUruchomMCMF.setObjectName(u"btnUruchomMCMF")

        self.verticalLayout_1.addWidget(self.btnUruchomMCMF)

        self.panel_statystyk = QLabel(self.tab_logistyka)
        self.panel_statystyk.setObjectName(u"panel_statystyk")
        self.panel_statystyk.setAlignment(Qt.AlignmentFlag.AlignLeading|Qt.AlignmentFlag.AlignLeft|Qt.AlignmentFlag.AlignTop)
        self.panel_statystyk.setWordWrap(True)

        self.verticalLayout_1.addWidget(self.panel_statystyk)

        self.verticalSpacer_1 = QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.verticalLayout_1.addItem(self.verticalSpacer_1)

        self.tabWidget.addTab(self.tab_logistyka, "")
        self.tab_obrona = QWidget()
        self.tab_obrona.setObjectName(u"tab_obrona")
        self.verticalLayout_2 = QVBoxLayout(self.tab_obrona)
        self.verticalLayout_2.setObjectName(u"verticalLayout_2")
        self.labelObronaTytul = QLabel(self.tab_obrona)
        self.labelObronaTytul.setObjectName(u"labelObronaTytul")

        self.verticalLayout_2.addWidget(self.labelObronaTytul)

        self.label_od = QLabel(self.tab_obrona)
        self.label_od.setObjectName(u"label_od")

        self.verticalLayout_2.addWidget(self.label_od)

        self.spin_od = QSpinBox(self.tab_obrona)
        self.spin_od.setObjectName(u"spin_od")

        self.verticalLayout_2.addWidget(self.spin_od)

        self.label_do = QLabel(self.tab_obrona)
        self.label_do.setObjectName(u"label_do")

        self.verticalLayout_2.addWidget(self.label_do)

        self.spin_do = QSpinBox(self.tab_obrona)
        self.spin_do.setObjectName(u"spin_do")

        self.verticalLayout_2.addWidget(self.spin_do)

        self.btn_atak = QPushButton(self.tab_obrona)
        self.btn_atak.setObjectName(u"btn_atak")

        self.verticalLayout_2.addWidget(self.btn_atak)

        self.verticalSpacer_2 = QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.verticalLayout_2.addItem(self.verticalSpacer_2)

        self.tabWidget.addTab(self.tab_obrona, "")
        self.tab_ksiegi = QWidget()
        self.tab_ksiegi.setObjectName(u"tab_ksiegi")
        self.verticalLayout_3 = QVBoxLayout(self.tab_ksiegi)
        self.verticalLayout_3.setObjectName(u"verticalLayout_3")
        self.btn_import = QPushButton(self.tab_ksiegi)
        self.btn_import.setObjectName(u"btn_import")

        self.verticalLayout_3.addWidget(self.btn_import)

        self.btn_eksport = QPushButton(self.tab_ksiegi)
        self.btn_eksport.setObjectName(u"btn_eksport")

        self.verticalLayout_3.addWidget(self.btn_eksport)

        self.input_szukaj = QLineEdit(self.tab_ksiegi)
        self.input_szukaj.setObjectName(u"input_szukaj")

        self.verticalLayout_3.addWidget(self.input_szukaj)

        self.btn_szukaj = QPushButton(self.tab_ksiegi)
        self.btn_szukaj.setObjectName(u"btn_szukaj")

        self.verticalLayout_3.addWidget(self.btn_szukaj)

        self.textBrowser_ksiegi = QTextBrowser(self.tab_ksiegi)
        self.textBrowser_ksiegi.setObjectName(u"textBrowser_ksiegi")

        self.verticalLayout_3.addWidget(self.textBrowser_ksiegi)

        self.tabWidget.addTab(self.tab_ksiegi, "")

        self.horizontalLayout.addWidget(self.tabWidget)

        mainWindow.setCentralWidget(self.centralwidget)
        self.statusbar = QStatusBar(mainWindow)
        self.statusbar.setObjectName(u"statusbar")
        mainWindow.setStatusBar(self.statusbar)

        self.retranslateUi(mainWindow)

        self.tabWidget.setCurrentIndex(0)


        QMetaObject.connectSlotsByName(mainWindow)
    # setupUi

    def retranslateUi(self, mainWindow):
        mainWindow.setWindowTitle(QCoreApplication.translate("mainWindow", u"Kr\u00f3lestwo Krasnoludk\u00f3w - Panel Dowodzenia", None))
        self.labelLogistykaTytul.setText(QCoreApplication.translate("mainWindow", u"<html><head/><body><p align=\"center\"><span style=\" font-size:14pt; font-weight:600; color:#d4af37;\">Zarz\u0105dzanie Owsiank\u0105</span></p></body></html>", None))
        self.btnDodajDomek.setText(QCoreApplication.translate("mainWindow", u"Wznie\u015b Nowy Domek", None))
        self.odswiezMape.setText(QCoreApplication.translate("mainWindow", u"Zbadaj Teren (Od\u015bwie\u017c)", None))
        self.btnUruchomMCMF.setText(QCoreApplication.translate("mainWindow", u"Oblicz Trasy Wydobycia (MCMF)", None))
        self.panel_statystyk.setText(QCoreApplication.translate("mainWindow", u"<h3 style='margin-top: 0; margin-bottom: 10px; color: #f1c40f;'>\U0001f4ca Raport Kr\U000000f3lestwa</h3>Brak danych. Wczytaj map\U00000119.", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab_logistyka), QCoreApplication.translate("mainWindow", u"\u26cf\ufe0f Logistyka", None))
        self.labelObronaTytul.setText(QCoreApplication.translate("mainWindow", u"<html><head/><body><p align=\"center\"><span style=\" font-size:14pt; font-weight:600; color:#d4af37;\">System Wczesnego Ostrzegania</span></p></body></html>", None))
        self.label_od.setText(QCoreApplication.translate("mainWindow", u"Pocz\u0105tek odcinka ataku:", None))
        self.label_do.setText(QCoreApplication.translate("mainWindow", u"Koniec odcinka ataku:", None))
        self.btn_atak.setText(QCoreApplication.translate("mainWindow", u"Wezwij Dekametrowc\u00f3w!", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab_obrona), QCoreApplication.translate("mainWindow", u"\U0001f3f9 Obrona Granic", None))
        self.btn_import.setText(QCoreApplication.translate("mainWindow", u"Importuj Dane", None))
        self.btn_eksport.setText(QCoreApplication.translate("mainWindow", u"Eksportuj Dane", None))
        self.input_szukaj.setPlaceholderText(QCoreApplication.translate("mainWindow", u"Wpisz szukan\u0105 run\u0119/s\u0142owo...", None))
        self.btn_szukaj.setText(QCoreApplication.translate("mainWindow", u"Przeszukaj Ksi\u0119gi (Rabin-Karp)", None))
        self.textBrowser_ksiegi.setHtml(QCoreApplication.translate("mainWindow", u"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><meta charset=\"utf-8\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"hr { height: 1px; border-width: 0; }\n"
"li.unchecked::marker { content: \"\\2610\"; }\n"
"li.checked::marker { content: \"\\2612\"; }\n"
"</style></head><body style=\" font-family:'.AppleSystemUIFont'; font-size:13pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-style:italic; color:#a89f91;\">Ksi\u0119ga jest pusta. Za\u0142aduj tekst, aby go przeczyta\u0107.</span></p></body></html>", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab_ksiegi), QCoreApplication.translate("mainWindow", u"\U0001f4dc Ksi\U00000119gi Kr\U000000f3lestwa", None))
    # retranslateUi

