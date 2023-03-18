/* Tester for MS6502 project.
   Copyright (C) 2023, Mirian Shilakadze 
 
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
#include <stdio.h>
#include <string.h>
#include "../src/include/6502.h"

u8 ram[0x10000];

u8 read(u16 address)
{
    return ram[address];
}

u8 write(u16 address, u8 data)
{
	ram[address] = data;
    return 0;
}

int main(i32 argc, char **argv)
{
	if(argc == 1){
		fprintf(stderr, "%s: error: no input file\n", argv[0]);
		return 1;
	}

    FILE *f = fopen(argv[1], "r");
	if(!f){
		fprintf(stderr, "%s: error: %s: no such file or directory\n", argv[0], argv[1]);
		return 1;
	}

    u32 error_count = 0;

	MS6502 *mp = MS6502_create(read, write);

    char curr_file_name[128];
    curr_file_name[0] = '\0';
	u64 size = 0;
	char *line = NULL;

	u32 nline = 0;
	while(getline(&line, &size, f) != -1){
		nline++;
		if(*line != ':') continue;

		char *temp = strtok(line, ":"); 

        u8 preserve_state = *temp == '+';

		u64 clock = (u64)strtol(temp, NULL, 0) + (preserve_state ? 7 : 0);

		temp = strtok(temp, ":");
		if(strcmp(temp, "_")){
            strcpy(curr_file_name, temp);
			FILE *input_file = fopen(curr_file_name, "r");
			if(!input_file) {
				fprintf(stderr, "%s:%u: error: %s: no such file or directory\n", argv[1], nline, curr_file_name);
				continue;
			}
			fread((void *) ram, 1, 0x10000, input_file);
			fclose(input_file);
		}
        else if(curr_file_name[0] == '\0'){
            fprintf(stderr, "%s: RAM is empty\n", argv[0]);
            continue;
        }

		/* 
		 * Indexes 0-6 correspond to the registers as listed below.
		 * Indexes above 6 correspond to cells in RAM, 
		 *
		 * 0 -> PC
		 * 1 -> AC
		 * 2 -> X
		 * 3 -> Y
		 * 4 -> SR
		 * 5 -> SP
		 * 6 -> IR
		 */
		union {
			struct{
				u8 should_check;
				u16 value;
			};
			struct{
				u16 address;
				u8 data;
			};
		} expected_values[64];
		u32 values_end = 7;
		for(u8 i = 0; i < 7; i++) expected_values[i].should_check = 0;

		while((temp = strtok(NULL, ", "))){
			if(*temp == '\n') break;
			if(!strcmp(temp, "MEM")){
				char *address = strtok(NULL, " ");
				char *data = strtok(NULL, " ");

				if(!data || !address){
					fprintf(stderr, "%s:%u: error: \'MEM\' requires two arguments\n", argv[1], nline);
					goto next_line;
				}

				expected_values[values_end].address = (u16) strtol(address, NULL, 0); 
				expected_values[values_end++].data = (u8) strtol(data, NULL, 0); 
				continue;
			}

			char *val = strtok(NULL, " ,");
			if(!val){
				fprintf(stderr, "%s:%u: error: register requires an argument\n", argv[1], nline);
				goto next_line;
			}

			u16 value = (u16)strtol(val, NULL, 0);

			if(!strcmp(temp, "PC")){
				expected_values[0].should_check = 1;
				expected_values[0].value = value;
			}
			else if(!strcmp(temp, "AC")){
				expected_values[1].should_check = 1;
				expected_values[1].value = value;
			}
			else if(!strcmp(temp, "X")) {
				expected_values[2].should_check = 1;
				expected_values[2].value = value;
			}
			else if(!strcmp(temp, "Y")) {
				expected_values[3].should_check = 1;
				expected_values[3].value = value;
			}
			else if(!strcmp(temp, "SR")) {
				expected_values[4].should_check = 1;
				expected_values[4].value = value;
			}
			else if(!strcmp(temp, "SP")) {
				expected_values[5].should_check = 1;
				expected_values[5].value = value;
			}
			else if(!strcmp(temp, "IR")) {
				expected_values[6].should_check = 1;
				expected_values[6].value = value;
			}
			else {
				fprintf(stderr, "%s:%u: error: unknown register %s\n", argv[1], nline, temp);
				goto next_line;
			}
			
		}

        if(!preserve_state)
            MS6502_reset(mp);

		for(u32 i = 0; i < clock; i++) MS6502_clock(mp);

		for(u32 i = 0; i < values_end; i++){
			if(i < 7){
				if(!expected_values[i].should_check) continue;

				u16 actual_value;
				char *register_name;
				switch(i){
				case 0:
					register_name = "PC";
					actual_value = mp->PC;
					break;

				case 1:
					register_name = "AC";
					actual_value = mp->AC;
					break;

				case 2:
					register_name = "X";
					actual_value = mp->X;
					break;

				case 3:
					register_name = "Y";
					actual_value = mp->Y;
					break;
					
				case 4:
					register_name = "SR";
					actual_value = mp->SR;
					break;

				case 5:
					register_name = "SP";
					actual_value = mp->SP;
					break;

				case 6:
					register_name = "IR";
					actual_value = mp->IR;
					break;
				default: break;
				}


				if(actual_value != expected_values[i].value){
					printf("%s: after %u clock signals the value of %s register expected to be %x but was %x\n", 
						   curr_file_name, clock, register_name, expected_values[i].value, actual_value);
					error_count++;
				}
			}
			else if(expected_values[i].data != ram[expected_values[i].address]){
				printf("%s: after %u clock signals the value of memory cell with address %x expected to be %x but was %x\n",
					   curr_file_name, clock, expected_values[i].address, expected_values[i].data, ram[expected_values[i].address]);
				error_count++;
			}
		}
	next_line: ;
	}

	if(line) free(line);

	if(!error_count) printf("%s: every test has been passed successfuly!\n", argv[0]);

	free(mp);
	fclose(f);

	return 0;
}
