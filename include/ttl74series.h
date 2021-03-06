#ifndef TTL74SERIES_H
#define TTL74SERIES_H

#include "config.h"
#include "sol1_register_8bit.h"


// 2-NOR
SOL1_BYTE IC_74LS02(SOL1_BYTE A, SOL1_BYTE B);

// INVERTER
SOL1_BYTE IC_74LS04(SOL1_BYTE A);


// 2-AND
SOL1_BYTE IC_74LS08(SOL1_BYTE A, SOL1_BYTE B);


// 3-NAND
SOL1_BYTE IC_74LS10(SOL1_BYTE A, SOL1_BYTE B, SOL1_BYTE C);

// 3-AND
SOL1_BYTE IC_74LS11(SOL1_BYTE A, SOL1_BYTE B, SOL1_BYTE C);

// 3-NOR
SOL1_BYTE IC_74LS27(SOL1_BYTE A, SOL1_BYTE B, SOL1_BYTE C);

// 2-OR
SOL1_BYTE IC_74LS32(SOL1_BYTE A, SOL1_BYTE B);

// 2-XOR
SOL1_BYTE IC_74LS86(SOL1_BYTE A, SOL1_BYTE B);

//74LS138     3 - line to 8 - line decoder / demultiplexer 16 - PDIP 0 to 70
//74LS148     10 - LINE TO 4 - LINE AND 8 - LINE TO 3 - LINE PRIORITY ENCODERS

//74LS151     1 - of - 8 Line Data Selector / Multiplexer257
SOL1_BYTE IC_74LS151(SOL1_BYTE D,
	SOL1_BYTE ABC,
	SOL1_BYTE G);

//74LS153     Dual 1 - of - 4 Line Data Selectors / Multiplexer
SOL1_BYTE IC_74LS153(SOL1_BYTE X1,
	SOL1_BYTE X2,
	SOL1_BYTE AB,
	SOL1_BYTE G);

//74LS157     QUADRUPLE 2 - LINE TO 1 - LINE DATA SELECTORS / MULTIPLEXERS
SOL1_BYTE IC_74LS157(SOL1_BYTE A,
	SOL1_BYTE B,
	SOL1_BYTE OEAB);

//74LS181     4 - Bit Arithmetic Logic Unit

//74LS244     Octal 3 - STATE Buffer / Line Driver / Line Receiver
SOL1_BYTE IC_74LS244(SOL1_BYTE Y,
	SOL1_BYTE OE);

//74LS253     3 - STATE Data Selector / Multiplexer
SOL1_BYTE IC_74LS253(SOL1_BYTE A,
	SOL1_BYTE B,
	SOL1_BYTE AB, 
	SOL1_BYTE G);

//74LS257     3-STATE Quad 2-Data Selectors/Multiplexers
SOL1_BYTE IC_74LS257(SOL1_BYTE A,
	SOL1_BYTE B,
	SOL1_BYTE OEAB);

//74LS273     8-Bit Register with Clear
SOL1_BYTE IC_74LS273(SOL1_REGISTER_8BIT *reg, SOL1_BYTE D, SOL1_BYTE CLK, SOL1_BYTE MR);


//74LS283     4 - Bit Binary Full Adders With Fast Carry 16 - PDIP 0 to 70


//74LS377     Octal D - Type Flip - Flop with Common Enable and Clock
SOL1_BYTE IC_74LS377(SOL1_REGISTER_8BIT *reg, SOL1_BYTE D, SOL1_BYTE CLK, SOL1_BYTE G);






SOL1_BYTE IC74LSL181(SOL1_BYTE nA, SOL1_BYTE nB, SOL1_BYTE S, SOL1_BYTE M, SOL1_BYTE Cn,
	SOL1_BYTE *AeqB, SOL1_BYTE *Cn4, SOL1_BYTE *nP , SOL1_BYTE *nG); //nF



#endif


