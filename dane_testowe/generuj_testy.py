import os
import random

# USTAWIENIA 
ILE_KOPALNI_WIELKIE =  200      
ILE_KRASNALI_WIELKIE = 1000 
WYMIARY_MAPY = 800             
SUROWCE = ["Zloto", "Srebro", "Wegiel", "Miedz", "Zelazo", "Diamenty"]
# ==============================================

print("Rozpoczynam przygotowywanie danych na obronę projektu...")


# 1. DANE TESTOWE (MAŁE) (Kwadrat)

#EFEKT:
# - Przepływ: Maksymalny przepływ wynosi 8. Wszystkie krasnoludki (8/8) 
#   zostają przypisane do kopalni z odpowiednim surowcem.
# - Graham: Otoczka tworzy idealny kwadrat o wierzchołkach (100,100), (500,100), 
#   (500,500), (100,500). Całkowita długość trasy (obwód) to dokładnie 1600.


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


#EFEKT
# - Graham: Punkty są współliniowe. Algorytm nie powinien się zawiesić. 
#   Trasa patrolu to po prostu odcinek od (100,300) do (500,300) i z powrotem, 
#   więc całkowity dystans to 800.
# - Przepływ: Sieć bez problemu obsługuje mniejszą liczbę chętnych (10) 
#   niż dostępnych miejsc w kopalniach (25). Przepływ = 10.

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

#EFEKT
# - Test wydajnościowy (Stress Test).
# - Przepływ: Część krasnoludków zostanie przypisana, część nie (losowość dopasowań). 
#   Program powinien przetworzyć to w ułamku sekundy, nie wchodząc w nieskończoną pętlę.
# - Graham: Obliczenie otoczki dla 50 punktów powinno wykonać się natychmiast, 
#   bez przepełnienia stosu (Stack Overflow).

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



#nowe 

# 4. DANE BRZEGOWE - Zdublowane pozycje (Dla Grahama)

#EFEKT
# - Graham: Kopalnie ID 1 i ID 2 leżą dokładnie w tym samym punkcie (300,300).
#   Dodatkowo leżą one na jednej prostej z kopalniami 3 i 4 (y = x).
#   Algorytm musi zignorować duplikat i nie rzucić błędem dzielenia przez zero.
#   Otoczka to linia od (100,100) do (500,500), a trasa tam i z powrotem to ok. 1131.37.

with open('kopalnie_duplikaty.csv', 'w', encoding='utf-8') as f:
    f.write("ID,X,Y,Surowiec,IloscMiejsc\n")
    f.write("1,300,300,Zloto,4\n")
    f.write("2,300,300,Srebro,2\n") # Ten sam punkt X,Y!
    f.write("3,100,100,Wegiel,2\n")
    f.write("4,500,500,Miedz,2\n")

# 5. DANE BRZEGOWE - Brak dopasowania surowców (Dla Przepływu)

#EFEKT
# - Przepływ: Podaż (Zelazo, Wegiel) nie zgadza się z popytem (Diamenty).
#   Algorytm poprawnie rozwiązuje sieć zwracając maksymalny przepływ = 0.
#   Wszyscy pracownicy zostają bez przydziału.
# - Graham: Odpali się dla dwóch punktów, zwracając podwojoną odległość między nimi.


with open('kopalnie_brak_dopasowania.csv', 'w', encoding='utf-8') as f:
    f.write("ID,X,Y,Surowiec,IloscMiejsc\n")
    f.write("1,100,100,Wegiel,10\n") 
    f.write("2,500,500,Zelazo,10\n") 

with open('dane_krasnoludkow_brak_dopasowania.csv', 'w', encoding='utf-8') as f:
    f.write("ID,ID_kopalni,Mineraly,X,Y\n")
    for i in range(1, 11):
        # Krasnoludki szukają tylko Diamentów, których nie ma
        f.write(f"{i},0,Diamenty,{random.randint(100, 500)},{random.randint(100, 500)}\n")

print("\nSukces! Wszystkie pliki czekają w folderze data/ i są gotowe do wczytania w GUI.")