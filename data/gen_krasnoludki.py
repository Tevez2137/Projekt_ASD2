import csv
import random
import sys

# Stała określająca maksymalny zasięg mapy
C = 1000 

MINERALY_POOL = ["Zloto", "Srebro", "Wegiel", "Miedz", "Zelazo", "Diamenty", "Rubiny"]

def generate_csv(n, filename="dane_krasnoludkow.csv", m_kopalni=10):
    header = ['ID', 'ID_kopalni', 'Mineraly', 'X', 'Y']
    
    try:
        with open(filename, mode='w', newline='', encoding='utf-8') as f:
            writer = csv.writer(f)
            writer.writerow(header) # Zapisujemy nagłówek
            
            for i in range(1, n + 1):
                k_id = i
                kopalnia_id = random.randint(1, m_kopalni)
                
                # Losujemy minerały i łączymy je średnikiem, np. "Zloto;Srebro"
                ile_min = random.randint(1, 3)
                wybrane_min = ";".join(random.sample(MINERALY_POOL, ile_min))
                
                x = random.randint(0, C)
                y = random.randint(0, C)
                
                writer.writerow([k_id, kopalnia_id, wybrane_min, x, y])
                
        print(f"Sukces! Wygenerowano {n} krasnoludków w pliku: {filename}")
        
    except IOError as e:
        print(f"Błąd zapisu pliku: {e}")

if __name__ == "__main__":
    n_danych = int(sys.argv[1]) if len(sys.argv) > 1 else 10
    generate_csv(n_danych)