# Lista de directorios que coinciden con Components/*/
COMPONENT_DIRS := $(wildcard Components/*/)

# Objetivo principal para ejecutar make en cada directorio
all: $(COMPONENT_DIRS)

$(COMPONENT_DIRS):
	@echo "Ejecutando make en $@"
	$(MAKE) -C $@
	@echo "---------------------------------------------------------"

.PHONY: all $(COMPONENT_DIRS)

