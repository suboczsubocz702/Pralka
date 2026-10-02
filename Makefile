# Nazwa pliku wykonywalnego
TARGET = PralniaCPU

# Kompilator i flagi
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread

# Automatyczne wyszukiwanie wszystkich plików .cpp w katalogu
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

# Domyślna reguła (budowanie programu)
all: $(TARGET)

# Łączenie obiektów w plik wykonywalny
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

# Kompilacja plików .cpp do .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Czyszczenie plików kompilacji
clean:
	@if exist *.o del /f /q *.o 2>nul || rm -f *.o
	@if exist $(TARGET) del /f /q $(TARGET) 2>nul || if exist $(TARGET).exe del /f /q $(TARGET).exe 2>nul || rm -f $(TARGET)
	@echo Czyszczenie zakończone.

# Ponowna pełna kompilacja
rebuild: clean all

.PHONY: all clean rebuild
