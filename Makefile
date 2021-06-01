INCLUDES= -I ./include
FLAGS = -O3 -std=c++11
#FLAGS2 = -g

OBJECTS=./build/utils.o ./build/tasm_opcode.o ./build/sol1_rom.o ./build/sol1_register_8bit.o ./build/sol1_registers.o ./build/sol1_microcode.o ./build/sol1_memory.o ./build/sol1_keyboard.o ./build/sol1_alu_4bit.o ./build/sol1_alu.o ./build/queue_ll.o ./build/linkedlist.o ./build/hw_web.o ./build/hw_uart.o ./build/hw_tty.o ./build/hw_timer.o ./build/hw_rtc.o ./build/hw_ide.o ./build/hashtable.o ./build/debugmenu_roms.o ./build/debugmenu_main.o ./build/sol1_cpu.o ./build/sol1_bus.o ./build/sol1_computer.o

all: ${OBJECTS} 
	gcc ${FLAGS} ${INCLUDES} ./src/main.cpp ${OBJECTS} -L ./lib -lmingw32 -lstdc++ -o ./bin/main.exe	

./build/sol1_computer.o:src/sol1_computer.cpp
	gcc ${FLAGS} ${INCLUDES} ./src/sol1_computer.cpp -c -o ./build/sol1_computer.o

./build/sol1_bus.o:src/sol1_bus.cpp
	gcc ${FLAGS} ${INCLUDES} ./src/sol1_bus.cpp -c -o ./build/sol1_bus.o

./build/sol1_cpu.o:src/sol1_cpu.cpp
	gcc ${FLAGS} ${INCLUDES} ./src/sol1_cpu.cpp -c -o ./build/sol1_cpu.o

./build/debugmenu_main.o:src/debugmenu_main.cpp
	gcc ${FLAGS} ${INCLUDES} ./src/debugmenu_main.cpp -c -o ./build/debugmenu_main.o

./build/debugmenu_roms.o:src/debugmenu_roms.cpp
	gcc ${FLAGS} ${INCLUDES} ./src/debugmenu_roms.cpp -c -o ./build/debugmenu_roms.o

./build/hashtable.o:src/hashtable.cpp
	gcc ${FLAGS} ${INCLUDES} ./src/hashtable.cpp -c -o ./build/hashtable.o

./build/hw_ide.o:src/hw_ide.cpp
	gcc ${FLAGS} ${INCLUDES} ./src/hw_ide.cpp -c -o ./build/hw_ide.o

./build/hw_rtc.o:src/hw_rtc.cpp
	gcc ${FLAGS} ${INCLUDES} ./src/hw_rtc.cpp -c -o ./build/hw_rtc.o

./build/hw_timer.o:src/hw_timer.cpp
	gcc ${FLAGS} ${INCLUDES} ./src/hw_timer.cpp -c -o ./build/hw_timer.o

./build/hw_tty.o:src/hw_tty.cpp
	gcc ${FLAGS} ${INCLUDES} ./src/hw_tty.cpp -c -o ./build/hw_tty.o

./build/hw_uart.o:src/hw_uart.cpp
	gcc ${FLAGS} ${INCLUDES} ./src/hw_uart.cpp -c -o ./build/hw_uart.o

./build/hw_web.o:src/hw_web.cpp
	gcc ${FLAGS} ${INCLUDES} ./src/hw_web.cpp -c -o ./build/hw_web.o

./build/linkedlist.o:src/linkedlist.cpp
	gcc ${FLAGS} ${INCLUDES} ./src/linkedlist.cpp -c -o ./build/linkedlist.o

./build/queue_ll.o:src/queue_ll.cpp
	gcc ${FLAGS} ${INCLUDES} ./src/queue_ll.cpp -c -o ./build/queue_ll.o

./build/sol1_alu.o:src/sol1_alu.cpp
	gcc ${FLAGS} ${INCLUDES} ./src/sol1_alu.cpp -c -o ./build/sol1_alu.o

./build/sol1_alu_4bit.o:src/sol1_alu_4bit.cpp
	gcc ${FLAGS} ${INCLUDES} ./src/sol1_alu_4bit.cpp -c -o ./build/sol1_alu_4bit.o

./build/sol1_keyboard.o:src/sol1_keyboard.cpp
	gcc ${FLAGS} ${INCLUDES} ./src/sol1_keyboard.cpp -c -o ./build/sol1_keyboard.o

./build/sol1_memory.o:src/sol1_memory.cpp
	gcc ${FLAGS} ${INCLUDES} ./src/sol1_memory.cpp -c -o ./build/sol1_memory.o

./build/sol1_microcode.o:src/sol1_microcode.cpp
	gcc ${FLAGS} ${INCLUDES} ./src/sol1_microcode.cpp -c -o ./build/sol1_microcode.o

./build/sol1_registers.o:src/sol1_registers.cpp
	gcc ${FLAGS} ${INCLUDES} ./src/sol1_registers.cpp -c -o ./build/sol1_registers.o

./build/sol1_register_8bit.o:src/sol1_register_8bit.cpp
	gcc ${FLAGS} ${INCLUDES} ./src/sol1_register_8bit.cpp -c -o ./build/sol1_register_8bit.o

./build/sol1_rom.o:src/sol1_rom.cpp
	gcc ${FLAGS} ${INCLUDES} ./src/sol1_rom.cpp -c -o ./build/sol1_rom.o

./build/tasm_opcode.o:src/tasm_opcode.cpp
	gcc ${FLAGS} ${INCLUDES} ./src/tasm_opcode.cpp -c -o ./build/tasm_opcode.o

./build/utils.o:src/utils.cpp
	gcc ${FLAGS} ${INCLUDES} ./src/utils.cpp -c -o ./build/utils.o


clean:
	del build\* /q
	del bin\main.exe /q
	del bin\main2.exe /q
	del bin\test.exe /q