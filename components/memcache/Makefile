# Directorios de origen y destino
SRC_DIR = src
INCLUDE_DIR = include
COMPILED_DIR = compiled
ROBIN_HOOD_DIR = ../../Libraries/robin-hood-hashing

# Nombre del ejecutable
EXECUTABLE = main

# Compilador y opciones de compilación
CXX = g++
CXXFLAGS = -std=c++20 -Wall -O4 -I$(INCLUDE_DIR) -I$(ROBIN_HOOD_DIR)  # Agrega la ruta a Robin Hood

# Obtener la lista de archivos fuente (excluyendo main.cpp)
SOURCES = $(filter-out main.cpp, $(wildcard $(SRC_DIR)/**/*.cpp))

# Objeto correspondiente a main.cpp
MAIN_OBJECT = main.o

# Objetos generados
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(COMPILED_DIR)/%.o,$(SOURCES)) $(MAIN_OBJECT)

# Regla principal para compilar el ejecutable
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS)
	
# Regla para compilar los objetos del directorio SRC_DIR
$(COMPILED_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Regla para compilar main.cpp
$(MAIN_OBJECT): main.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Limpieza de archivos compilados
clean:
	rm -rf $(COMPILED_DIR)/*.o $(EXECUTABLE)
