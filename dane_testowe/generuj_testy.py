import os
import random
from pathlib import Path

# USTAWIENIA 
SUROWCE = ["Zloto", "Srebro", "Wegiel", "Miedz", "Zelazo", "Diamenty"]
MIN_ODSTEP = 150
WYMIARY_MAPY = 8000

# Definicje zestawów testowych (nazwa, ilość kopalni, ilość krasnoludków)
# Rozmiary dobrane aby było 5 zestawów, w tym jeden szybki i jeden bardzo duży (~10-15 minut)
ZESTAWY_TESTOWE = {
    "przyklad_bardzo_maly": (30, 150),     # lekki, szybki test
    "przyklad_maly": (70, 350),           # niewielki test
    "przyklad_sredni": (150, 750),        # średnia skala
    "przyklad_duzy": (300, 1500),         # test ciężki (~3-4 minuty)
    "przyklad_mamut": (700, 3500),        # bardzo duży test (~10-15 minut)
}

OUTPUT_DIR = Path(__file__).resolve().parent / "przykladowe"
OUTPUT_DIR.mkdir(exist_ok=True)

# ==============================================

print("Rozpoczynam przygotowywanie danych na obronę projektu...")

def losuj_punkt(wymiary, *_args, **_kwargs):
    x = random.randint(50, wymiary - 50)
    y = random.randint(50, wymiary - 50)
    return x, y

def generuj_zestaw(nazwa, ile_kopalni, ile_krasnoludkow):
    """Generuje zestaw testowy o zadanym rozmiarze"""
    print(f"\nGeneruję {nazwa}: {ile_kopalni} kopalni, {ile_krasnoludkow} krasnoludków...")
    
    # Plik kopalni
    kopalnie_path = OUTPUT_DIR / f"kopalnie_{nazwa}.csv"
    with open(kopalnie_path, 'w', encoding='utf-8') as f:
        f.write("ID,X,Y,Surowiec,IloscMiejsc\n")
        for i in range(1, ile_kopalni + 1):
            x, y = losuj_punkt(WYMIARY_MAPY)
            surowiec = random.choice(SUROWCE)
            miejsca = random.randint(2, 6)
            f.write(f"{i},{x},{y},{surowiec},{miejsca}\n")
    
    # Plik krasnoludków
    krasnoludki_path = OUTPUT_DIR / f"krasnoludki_{nazwa}.csv"
    with open(krasnoludki_path, 'w', encoding='utf-8') as f:
        f.write("ID,ID_kopalni,Mineraly,X,Y\n")
        for i in range(1, ile_krasnoludkow + 1):
            x, y = losuj_punkt(WYMIARY_MAPY)
            lubiane = ";".join(random.sample(SUROWCE, random.randint(1, 3)))
            f.write(f"{i},0,{lubiane},{x},{y}\n")
    
    print(f"  ✓ {kopalnie_path}")
    print(f"  ✓ {krasnoludki_path}")



# Generuj testy o różnych rozmiarach
for nazwa, (ile_kopalni, ile_krasnoludkow) in ZESTAWY_TESTOWE.items():
    generuj_zestaw(nazwa, ile_kopalni, ile_krasnoludkow)

print(f"\n✅ Sukces! Wszystkie pliki testowe zostały wygenerowane w: {OUTPUT_DIR}")