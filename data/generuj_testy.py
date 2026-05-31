import os
import random

# USTAWIENIA 
ILE_KOPALNI_WIELKIE = 50       
ILE_KRASNALI_WIELKIE = 200     
WYMIARY_MAPY = 800             
SUROWCE = ["Zloto", "Srebro", "Wegiel", "Miedz", "Zelazo", "Diamenty"]
# ==============================================

print("Rozpoczynam przygotowywanie danych na obronę projektu...")


# 1. DANE TESTOWE (MAŁE) (Kwadrat)

with open('kopalnie_test.csv', 'w', encoding='utf-8') as f:
    f.write("ID,X,Y,Surowiec,IloscMiejsc\n")
    f.write("1,100,100,Zloto,2\n")
    f.write("2,100,500,Srebro,2\n")
    f.write("3,500,500,Wegiel,2\n")
    f.write("4,500,100,Miedz,2\n")

with open('dane_krasnoludkow_test.csv', 'w', encoding='utf-8') as f:
    f.write("ID,ID_kopalni,Mineraly,X,Y\n")
    f.write("1,0,Zloto,80,80\n")
    f.write("2,0,Zloto,120,120\n")
    f.write("3,0,Srebro,80,520\n")
    f.write("4,0,Srebro,120,480\n")
    f.write("5,0,Wegiel,520,520\n")
    f.write("6,0,Wegiel,480,480\n")
    f.write("7,0,Miedz,520,80\n")
    f.write("8,0,Miedz,480,120\n")
print("- Wygenerowano pliki testowe (Idealny kwadrat)")

# 2. DANE BRZEGOWE - Wszystko w jednej linii

with open('kopalnie_edge.csv', 'w', encoding='utf-8') as f:
    f.write("ID,X,Y,Surowiec,IloscMiejsc\n")
    f.write("1,100,300,Zelazo,5\n")
    f.write("2,200,300,Zelazo,5\n")
    f.write("3,300,300,Zelazo,5\n")
    f.write("4,400,300,Zelazo,5\n")
    f.write("5,500,300,Zelazo,5\n")

with open('dane_krasnoludkow_edge.csv', 'w', encoding='utf-8') as f:
    f.write("ID,ID_kopalni,Mineraly,X,Y\n")
    for i in range(1, 11):
        f.write(f"{i},0,Zelazo,{random.randint(100, 500)},{random.randint(280, 320)}\n")
print("- Wygenerowano pliki brzegowe (Płaska linia Y=300)")

# 3. DANE PRODUKCYJNE (Wielkie dane)

with open('kopalnie_wielkie.csv', 'w', encoding='utf-8') as f:
    f.write("ID,X,Y,Surowiec,IloscMiejsc\n")
    for i in range(1, ILE_KOPALNI_WIELKIE + 1):
        x = random.randint(50, WYMIARY_MAPY - 50)
        y = random.randint(50, WYMIARY_MAPY - 50)
        surowiec = random.choice(SUROWCE)
        miejsca = random.randint(2, 6)
        f.write(f"{i},{x},{y},{surowiec},{miejsca}\n")

with open('dane_krasnoludkow_wielkie.csv', 'w', encoding='utf-8') as f:
    f.write("ID,ID_kopalni,Mineraly,X,Y\n")
    for i in range(1, ILE_KRASNALI_WIELKIE + 1):
        x = random.randint(50, WYMIARY_MAPY - 50)
        y = random.randint(50, WYMIARY_MAPY - 50)
        lubiane = ";".join(random.sample(SUROWCE, random.randint(1, 3)))
        f.write(f"{i},0,{lubiane},{x},{y}\n")
print(f"- Wygenerowano wielką bazę: {ILE_KOPALNI_WIELKIE} kopalni i {ILE_KRASNALI_WIELKIE} krasnoludków")

print("\nSukces! Wszystkie pliki czekają w folderze data/ i są gotowe do wczytania w GUI.")