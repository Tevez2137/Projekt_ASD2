# Kompilator
CXX = g++

# Flagi kompilacji (standard C++17 i włączenie pokazywania błędów/ostrzeżeń)
CXXFLAGS = -std=c++17 -Wall -Wextra

# Nazwa Twojego skompilowanego programu
TARGET = symulacja_krasnoludkow

# Lista wszystkich plików źródłowych w Twoim projekcie
SRCS = main.cpp src/kopalnia.cpp src/krasnoludek.cpp src/mapa.cpp

# Pliki obiektowe (zastąpienie .cpp na .o)
OBJS = $(SRCS:.cpp=.o)

# Domyślna reguła - odpala się po wpisaniu samego 'make'
all: $(TARGET)

# Tworzenie pliku wykonywalnego
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Kompilacja każdego pliku .cpp do pliku .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Szybkie kompilowanie i uruchamianie programu
run: $(TARGET)
	./$(TARGET)

# Sprzątanie - usuwanie plików tymczasowych i samego programu
clean:
	rm -f $(OBJS) $(TARGET)