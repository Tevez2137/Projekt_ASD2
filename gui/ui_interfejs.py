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
from PySide6.QtGui import (QAction, QBrush, QColor, QConicalGradient,
    QCursor, QFont, QFontDatabase, QGradient,
    QIcon, QImage, QKeySequence, QLinearGradient,
    QPainter, QPalette, QPixmap, QRadialGradient,
    QTransform)
from PySide6.QtWidgets import (QApplication, QGraphicsView, QGridLayout, QLabel,
    QMainWindow, QMenu, QMenuBar, QPushButton,
    QSizePolicy, QStatusBar, QWidget)

class Ui_mainWindow(object):
    def setupUi(self, mainWindow):
        if not mainWindow.objectName():
            mainWindow.setObjectName(u"mainWindow")
        mainWindow.setWindowModality(Qt.WindowModality.NonModal)
        mainWindow.resize(898, 729)
        mainWindow.setAutoFillBackground(False)
        self.centralwidget = QWidget(mainWindow)
        self.centralwidget.setObjectName(u"centralwidget")
        self.gridLayout = QGridLayout(self.centralwidget)
        self.gridLayout.setObjectName(u"gridLayout")
        self.map = QGraphicsView(self.centralwidget)
        self.map.setObjectName(u"map")

        self.gridLayout.addWidget(self.map, 4, 6, 1, 1)

        self.navbar = QGridLayout()
        self.navbar.setObjectName(u"navbar")
        self.pushButton_2 = QPushButton(self.centralwidget)
        self.pushButton_2.setObjectName(u"pushButton_2")

        self.navbar.addWidget(self.pushButton_2, 3, 0, 1, 1)

        self.odswiezMape = QPushButton(self.centralwidget)
        self.odswiezMape.setObjectName(u"odswiezMape")

        self.navbar.addWidget(self.odswiezMape, 2, 0, 1, 1)

        self.btnDodajDomek = QPushButton(self.centralwidget)
        self.btnDodajDomek.setObjectName(u"btnDodajDomek")

        self.navbar.addWidget(self.btnDodajDomek, 1, 0, 1, 1)


        self.gridLayout.addLayout(self.navbar, 4, 3, 1, 1)

        self.statusBar = QLabel(self.centralwidget)
        self.statusBar.setObjectName(u"statusBar")

        self.gridLayout.addWidget(self.statusBar, 5, 5, 1, 2)

        mainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QMenuBar(mainWindow)
        self.menubar.setObjectName(u"menubar")
        self.menubar.setGeometry(QRect(0, 0, 898, 30))
        self.menu = QMenu(self.menubar)
        self.menu.setObjectName(u"menu")
        mainWindow.setMenuBar(self.menubar)
        self.statusbar = QStatusBar(mainWindow)
        self.statusbar.setObjectName(u"statusbar")
        mainWindow.setStatusBar(self.statusbar)

        self.menubar.addAction(self.menu.menuAction())

        self.retranslateUi(mainWindow)

        QMetaObject.connectSlotsByName(mainWindow)
    # setupUi

    def retranslateUi(self, mainWindow):
        mainWindow.setWindowTitle(QCoreApplication.translate("mainWindow", u"Statystyki Kr\u00f3lestwa", None))
        self.pushButton_2.setText(QCoreApplication.translate("mainWindow", u"PushButton", None))
        self.odswiezMape.setText(QCoreApplication.translate("mainWindow", u"Od\u015bwie\u017c Map\u0119", None))
        self.btnDodajDomek.setText(QCoreApplication.translate("mainWindow", u"Dodaj domek", None))
        self.statusBar.setText("")
        self.menu.setTitle(QCoreApplication.translate("mainWindow", u"Mapa", None))
    # retranslateUi

