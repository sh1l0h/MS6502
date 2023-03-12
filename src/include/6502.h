#ifndef MS_6502_H
#define MS_6502_H

#include "c.h"

// SR flags
#define C 1   // Carry
#define Z 2   // Zero
#define I 4   // IRQ disable
#define D 8   // Decimalj
#define B 16  // Break
#define V 64  // Overflow
#define N 128 // Negative

typedef struct MS6502 {
	u8  (*read)(u16);
	u8  (*write)(u16, u8);

	u16 PC; // Program counter
	u8  AC; // Accumulator
	u8  X;  // X register
	u8  Y;  // Y register
	u8  SR; // Status register
	u8  SP; // Stack pointer
	u8  IR; // Instruction register

	i8  cycle;
	i8  op1;
	i16 op2;

} MS6502;

MS6502 *6502_create(u8 (*read)(u16), u8 (*write)(u16, u8));

void 6502_reset(MS6502 *mp);
void 6502_clock(MS6502 *mp);

#endif
