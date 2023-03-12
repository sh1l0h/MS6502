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

#include <stdlib.h>
#include "include/6502.h"

MS6502 *MS6502_create(u8 (*read)(u16), u8 (*write)(u16, u8))
{
	MS6502 *result = malloc(sizeof(*result));
	result->read   = read;
	result->write  = write;
	
	result->PC = 0x0000;
	result->AC = 0xAA;
	result->X  = 0x00;
	result->Y  = 0x00;
	result->SR = 0x00;
	result->SP = 0xFF;
	result->IR = 0x00;

	result->cycle = 0;
}

void MS6502_reset(MS6502 *mp)
{
	mp->cycle = -7;
}

static inline void fetch(MS6502 *mp)
{
	mp->IR = mp->read(mp->PC++);
}

static void execute_opcode(MS6502 *mp)
{
	if(mp->IR & 3 == 1){ // group one

		switch(mp->IR & 0xE0 >> 5){

		case 0: // ORA
		case 1: // AND
		case 2: // EOR
		case 3: // ADC
		case 4: // STA
		case 5: // LDA
			mp->AC = mp->op1;
			break;
		case 6: // CMP
		case 7: // SBC
			break;
		}

	}
}

static void addressing_mode(MS6502 *mp)
{
	if((mp->IR & 3) == 1){ // group one
		switch(mp->IR & 0x1C >> 2){
		case 1: // Zero page

			switch(mp->cycle){
			case 1:
				mp->op1 = mp->read(mp->PC++);
				break;
			case 2:
				mp->op1 = mp->read((u16)mp->op1);
				mp->cycle = -1;
			}
			break;

		case 2: // Immediate
			mp->op1 = mp->read(mp->PC++);
			mp->cycle = -1;
			break;

		case 3: // Absolute
			switch(mp->cycle){
			case 1:
				mp->op2 = (u16)mp->read(mp->PC++);
				break;
			case 2:
				mp->op2 |= (u16)mp->read(mp->PC++) << 8;
				break;
			case 3:
				mp->op1 = mp->read(mp->op2);
				mp->cycle = -1;
				break;
			}
			break;
		}
	}
}

void MS6502_clock(MS6502 *mp)
{

	if(mp->cycle < 0){
		switch(mp->cycle){
		case -2:
			mp->PC = mp->read(0xFFFC);
			break;
		case -1:
			mp->PC |= (u16)mp->read(0xFFFD) << 8;
			break;
		case -7:
			mp->PC = 0x0000;
			mp->AC = 0xAA;
			mp->X  = 0x00;
			mp->Y  = 0x00;
			mp->SR = 0x00;
			mp->SP = 0xFF;
			mp->IR = 0x00;
			break;


		default: break;
		}
	}
	else if(mp->cycle == 0){
		execute_opcode(mp);
		fetch(mp);
	}
	else addressing_mode(mp);

	mp->cycle++;
}
