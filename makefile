# Kompilator i flagi
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

# Pliki źródłowe
SRCS = main.cpp src/mapa.cpp src/kopalnia.cpp src/krasnoludek.cpp src/otoczka.cpp src/dekametrowcy.cpp src/ksiegi.cpp
BUILD_DIR = build

# Magia zamiany ścieżek
OBJS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SRCS))

# ==========================================
# WYKRYWANIE SYSTEMU OPERACYJNEGO
# ==========================================
ifeq ($(OS),Windows_NT)
	# Na Windowsie dodajemy tylko .exe
	TARGET = symulacja_krasnoludkow.exe
else
	# Na Linux / Mac bez .exe
	TARGET = symulacja_krasnoludkow
endif

CLEAN_CMD = rm -rf $(BUILD_DIR)
MKDIR_CMD = mkdir -p $(dir $@)
RUN_CMD = ./$(BUILD_DIR)/$(TARGET)

# Domyślna reguła
all: $(BUILD_DIR)/$(TARGET)

# Tworzenie finalnego pliku
$(BUILD_DIR)/$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

# Kompilacja plików .cpp do .o w folderze build
$(BUILD_DIR)/%.o: %.cpp
	@$(MKDIR_CMD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Odpalanie
run: all
	$(RUN_CMD)

# Sprzątanie
clean:
	$(CLEAN_CMD)