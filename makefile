# Kompilator i flagi
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

# Pliki źródłowe
SRCS = main.cpp src/mapa.cpp src/kopalnia.cpp src/krasnoludek.cpp src/otoczka.cpp src/dekametrowcy.cpp
BUILD_DIR = build

# Magia zamiany ścieżek
OBJS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SRCS))

# ==========================================
# WYKRYWANIE SYSTEMU OPERACYJNEGO
# ==========================================
ifeq ($(OS),Windows_NT)
	# Ustawienia dla Windows (cmd.exe)
	TARGET = symulacja_krasnoludkow.exe
	CLEAN_CMD = if exist $(BUILD_DIR) rmdir /S /Q $(BUILD_DIR)
	# Hack na tworzenie folderów w locie pod CMD:
	MKDIR_CMD = if not exist "$(dir $@)" mkdir "$(subst /,\,$(dir $@))"
	RUN_CMD = $(BUILD_DIR)\$(TARGET)
else
	# Ustawienia dla Linux / Mac
	TARGET = symulacja_krasnoludkow
	CLEAN_CMD = rm -rf $(BUILD_DIR)
	MKDIR_CMD = mkdir -p $(dir $@)
	RUN_CMD = ./$(BUILD_DIR)/$(TARGET)
endif
# ==========================================

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