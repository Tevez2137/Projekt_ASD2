# Kompilator i flagi
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

# Nazwa programu i nowy folder na pliki skompilowane
TARGET = symulacja_krasnoludkow
BUILD_DIR = build

# Pliki źródłowe
SRCS = main.cpp src/kopalnia.cpp src/krasnoludek.cpp src/mapa.cpp

# Magia nr 1: Automatycznie zamienia listę "src/plik.cpp" na "build/src/plik.o"
OBJS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SRCS))

# Domyślna reguła
all: $(BUILD_DIR)/$(TARGET)

# Tworzenie finalnego pliku wykonywalnego w folderze build/
$(BUILD_DIR)/$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

# Kompilacja plików .cpp do .o wewnątrz folderu build/
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Szybkie budowanie i uruchamianie z nowego folderu
run: all
	./$(BUILD_DIR)/$(TARGET)

# Sprzątanie - teraz jest banalne, po prostu usuwamy cały folder build!
clean:
	rm -rf $(BUILD_DIR)