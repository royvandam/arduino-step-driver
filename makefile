TARGET          = firmware
TARGET_PLATFORM = Atmel AVR
TARGET_MCU      = atmega328p
TARGET_MCU_TAG  = m328p
TARGET_CLOCK    = 16000000
TARGET_LOADER   = avrdude
TARGET_PORT     = /dev/ttyUSB0
TARGET_BOARD    = arduino

TOOLCHAIN = avr-

SOURCES = main.cpp gpio.cpp assert.cpp
OBJECTS = $(SOURCES:.cpp=.o)

INCLUDES = -I.
OPTIONS  = -DF_CPU=$(TARGET_CLOCK) -DWITH_DEBUG=1 -DWITH_ASSERT=1

EFLAGS  = -Wall -Wextra -Werror

CXX =  $(TOOLCHAIN)g++
CXXFLAGS = -mmcu=$(TARGET_MCU) $(EFLAGS) $(OPTIONS) $(INCLUDES) -Os -std=c++11 -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics 

LD_LIBS  =
LDFLAGS = $(MMCU) $(CXXFLAGS) -Wl,--gc-sections $(LDLIBS)

LOADER_FLAGS = -C arduino.conf -v -p $(TARGET_MCU_TAG) -c $(TARGET_BOARD) -P $(TARGET_PORT) -b57600

OBJCOPY = $(TOOLCHAIN)objcopy
OBJSIZE = $(TOOLCHAIN)size

# Implicit suffix rules
# ---------------------
.SUFFIXES:
.SUFFIXES: .cpp .o .elf .hex

.cpp.o:
	@echo CXX $@
	$(CXX) -c $(CXXFLAGS) -o $@ $<

# Build rules
# ---------------------
.PHONY: all clean install
.PHONY: checkfuses burnfuses terminal

all: $(SOURCES) $(TARGET).hex

$(TARGET).hex: $(TARGET).elf
	@echo OBJCOPY $@
	@$(OBJCOPY) -O ihex -R .eephex $(TARGET).elf $(TARGET).hex
	@echo OBJSIZE $@
	@$(OBJSIZE) -t $(OBJECTS) $(TARGET).elf $(TARGET).hex

$(TARGET).elf: $(OBJECTS)
	@echo LD $@
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $@

clean:
	rm -rf $(OBJECTS) $(TARGET).*

# Install: Write hex to the target device
install: $(TARGET).hex
	$(TARGET_LOADER) $(LOADER_FLAGS) -e -U flash:w:$(TARGET).hex

# Terminal: Open a connection with the STK500 terminal.
terminal:
	$(TARGET_LOADER) $(LOADER_FLAGS) -t $(FLAGS)

# Fuses: make sure to uncomment the extended fuse byte when needed by the microcontroller.
checkfuses:
	$(TARGET_LOADER) $(LOADER_FLAGS) \
	-U hfuse:v:$(TARGET_HFUSE):m -U lfuse:v:$(TARGET_LFUSE):m #-U efuse:v:$(TARGET_EFUSE):m

burnfuses:
	$(TARGET_LOADER) $(LOADER_FLAGS) \
	-U hfuse:w:$(TARGET_HFUSE):m -U lfuse:w:$(TARGET_LFUSE):m #-U efuse:w:$(TARGET_EFUSE):m
