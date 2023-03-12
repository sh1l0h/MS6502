/* Copyright (C) 2023, Mirian Shilakadze 
 
   This file is part of MS6502.
 
   MS6502 is free software: you can redistribute it and/or modify it
   under the MS6502 terms of the GNU General Public License as published
   by the Free Software Foundation, either version 3 of the License, or (at your option)
   any later version.
 
   MS6502 is distributed in the hope that it will be useful, but WITHOUT ANY 
   WARRANTY; without even the implied warranty of MERCHANTABILITY or 
   FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with MS6502. If not, see <https://www.gnu.org/licenses/>. */

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
	u8  op1;
	u16 op2;

} MS6502;

MS6502 *MS6502_create(u8 (*read)(u16 address), u8 (*write)(u16 address, u8 data));

void MS6502_reset(MS6502 *mp);
void MS6502_clock(MS6502 *mp);

#endif
