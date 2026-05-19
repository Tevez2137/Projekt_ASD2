# Kompilator i flagi
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

# Pliki źródłowe
SRCS = main.cpp src/mapa.cpp src/kopalnia.cpp src/krasnoludek.cpp src/otoczka.cpp src/dekametrowcy.cpp src/ksiegi.cpp
BUILD_DIR = build

# Magia zamiany ścieżek
OBJS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SRCS))

# ==========================================
# SUPER WYKRYWANIE SYSTEMU I KONSOLI
# ==========================================
ifeq ($(OS),Windows_NT)
	TARGET = symulacja_krasnoludkow.exe
	
	# Sprawdzamy, czy powłoka to uniksowe 'sh' (Twoje MSYS2) czy cmd.exe (Kumpel)
	ifneq (,$(findstring sh,$(SHELL)))
		# ---------- USTAWIENIA DLA CIEBIE (MSYS2) ----------
		MKDIR_CMD = mkdir -p $(dir $@)
		CLEAN_CMD = rm -rf $(BUILD_DIR)
		RUN_CMD = ./$(BUILD_DIR)/$(TARGET)
	else
		# ---------- USTAWIENIA DLA KUMPLA (CMD) ----------
		# Usuwamy końcowy ukośnik, żeby nie psuć cudzysłowów w Windowsie
		DIR_NO_SLASH = $(patsubst %/,%,$(dir $@))
		MKDIR_CMD = if not exist "$(subst /,\,$(DIR_NO_SLASH))" mkdir "$(subst /,\,$(DIR_NO_SLASH))"
		CLEAN_CMD = if exist $(BUILD_DIR) rmdir /S /Q $(BUILD_DIR)
		RUN_CMD = $(BUILD_DIR)\$(TARGET)
	endif
else
	# ---------- USTAWIENIA DLA LINUX / MAC ----------
	TARGET = symulacja_krasnoludkow
	MKDIR_CMD = mkdir -p $(dir $@)
	CLEAN_CMD = rm -rf $(BUILD_DIR)
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