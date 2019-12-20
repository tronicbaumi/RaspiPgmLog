/*
 * Raspberry Pi PIC Programmer using GPIO connector
 * https://github.com/WallaceIT/picberry
 * Copyright 2014 Francesco Valla
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdint.h>
#include <string.h>
#include <iostream>
#include <unistd.h>

#include "dspic33ck.h"

/* delays (in microseconds; nanoseconds are rounded to 1us) */
#define DELAY_P1   			1		// 200ns
#define DELAY_P1A			1		// 80ns
#define DELAY_P1B			1		// 80ns
#define DELAY_P2			1		// 15ns
#define DELAY_P3			1		// 15ns
#define DELAY_P4			1		// 40ns
#define DELAY_P4A			1		// 40ns
#define DELAY_P5			1		// 20ns
#define DELAY_P6			1		// 100ns
#define DELAY_P7	        50000	// 50ms
#define DELAY_P8			12		// 12us
#define DELAY_P9A			10		// 10us
#define DELAY_P9B			15		// 15us - 23us max!
#define DELAY_P10			1		// 400ns
#define DELAY_P11       	16000	// 16ms
#define DELAY_P12       	4200	// 4.2ms
#define DELAY_P13       	34	    // 34.5us MAX! -> 34us
#define DELAY_P14			1		// 1us MAX!
#define DELAY_P15			1		// 10ns
#define DELAY_P16			0		// 0s
#define DELAY_P17   		1		// 100ns
#define DELAY_P18			1000	// 1ms
#define DELAY_P19			1		// 25ns
//#define DELAY_P20			25000	// 25ms
#define DELAY_P21			1		// 1us - 500us MAX!

#define ENTER_PROGRAM_KEY	0x4D434851

#define reset_pc() send_cmd(0x040200)
#define send_nop() send_cmd(0x000000)

static unsigned int counter=0;
static uint16_t nvmcon;

/*
 * send a 24-bit command via the SIX instruction
 */
void dspic33ck::send_cmd(uint32_t cmd)
{
	uint8_t i;

	GPIO_CLR(pic_data);

	/* send the SIX = 0x0000 instruction */
	for (i = 0; i < 4; i++) {
		GPIO_SET(pic_clk);
		delay_us(DELAY_P3);
		GPIO_CLR(pic_clk);
		delay_us(DELAY_P2);
	}

	delay_us(DELAY_P4);

	/* send the 24-bit command */
	for (i = 0; i < 24; i++) {
		if ( (cmd >> i) & 0x00000001 )
			GPIO_SET(pic_data);
		else
			GPIO_CLR(pic_data);
		delay_us(DELAY_P1A);
		GPIO_SET(pic_clk);
		delay_us(DELAY_P1B);
		GPIO_CLR(pic_clk);
	}

	delay_us(DELAY_P4A);

}

/* Send five NOPs (should be with a frequency greater than 2MHz...) */
inline void dspic33ck::send_prog_nop(void)
{
	uint8_t i;

	GPIO_CLR(pic_data);

	/* send 5 NOP commands */
	for (i = 0; i < 140; i++) {
		GPIO_SET(pic_clk);
		delay_us(DELAY_P1A);
		GPIO_CLR(pic_clk);
		delay_us(DELAY_P1B);
	}
}

/*
 * Read out the VISI register via the REGOUT instruction
 */
uint16_t dspic33ck::read_data(void)
{
	uint8_t i;
	uint16_t data = 0;

	GPIO_CLR(pic_data);
	GPIO_CLR(pic_clk);

	/* send the REGOUT=0x0001 instruction */
	for (i = 0; i < 4; i++) {
		if ( (0x0001 >> i) & 0x001 )
			GPIO_SET(pic_data);
		else
			GPIO_CLR(pic_data);
		delay_us(DELAY_P1A);
		GPIO_SET(pic_clk);
		delay_us(DELAY_P1B);
		GPIO_CLR(pic_clk);
	}

	delay_us(DELAY_P4);

	/* idle for 8 clock cycles, waiting for the data to be ready */
	for (i = 0; i < 8; i++) {
		GPIO_SET(pic_clk);
		delay_us(DELAY_P1B);
		GPIO_CLR(pic_clk);
		delay_us(DELAY_P1A);
	}

	delay_us(DELAY_P5);

	GPIO_IN(pic_data);

	/* read a 16-bit data word */
	for (i = 0; i < 16; i++) {
		GPIO_SET(pic_clk);
		delay_us(DELAY_P1B);
		data |= ( GPIO_LEV(pic_data) & 0x00000001 ) << i;
		GPIO_CLR(pic_clk);
		delay_us(DELAY_P1A);
	}

	delay_us(DELAY_P4A);
	GPIO_OUT(pic_data);
	return data;
}

/*
 * Enter ICSP Mode
 */
void dspic33ck::enter_program_mode(void){
    int i;

	GPIO_IN(pic_mclr);
	GPIO_OUT(pic_mclr);

	GPIO_CLR(pic_clk);

	GPIO_CLR(pic_mclr);		/*  remove VDD from MCLR pin */
	delay_us(DELAY_P6);
	GPIO_SET(pic_mclr);		/*  apply VDD to MCLR pin */
	delay_us(DELAY_P21);
	GPIO_CLR(pic_mclr);		/* remove VDD from MCLR pin */
	delay_us(DELAY_P18);

	/* Shift in the "enter program mode" key sequence (MSB first) */
	for (i = 31; i > -1; i--) {
		if ( (ENTER_PROGRAM_KEY >> i) & 0x01 )
			GPIO_SET(pic_data);
		else
			GPIO_CLR(pic_data);
		delay_us(DELAY_P1A);
		GPIO_SET(pic_clk);
		delay_us(DELAY_P1B);
		GPIO_CLR(pic_clk);

	}
	GPIO_CLR(pic_data);
	delay_us(DELAY_P19);
	GPIO_SET(pic_mclr);
	delay_us((DELAY_P7 + (5 * DELAY_P1)));

    /* idle for 5 clock cycles */
    for (i = 0; i < 5; i++) {
		GPIO_SET(pic_clk);
		delay_us(DELAY_P1B);
		GPIO_CLR(pic_clk);
		delay_us(DELAY_P1A);
	}

}

/*
 * Exit ICSP Mode
 */
void dspic33ck::exit_program_mode(void)
{
	GPIO_CLR(pic_clk);
	GPIO_CLR(pic_data);
	delay_us(DELAY_P16);
	GPIO_CLR(pic_mclr);		/* remove VDD from MCLR pin */
	delay_us(DELAY_P17);	/* wait (at least) P17 */
	GPIO_SET(pic_mclr);
	GPIO_IN(pic_mclr);
}

/*
 * Send a GOTO 0x200 command, before and after programming operations
 */
inline void dspic33ck::exit_reset_vector(void)
{
	send_nop();
	send_nop();
	send_nop();
	reset_pc();
	send_nop();
	send_nop();
	send_nop();
}

/*
 * read four 24-bit words (8-bit + 16-bit) from code memory and store them in the data array
 */
void dspic33ck::read_four_code_words(uint16_t *data, uint32_t addr)
{
	uint16_t raw_data[6];

	/* exit reset vector */
	exit_reset_vector();

	/* Initialize the TBLPAG register and the Read Pointer (W6) for the TBLRD instruction*/
	send_cmd(0x200000 | ((addr & 0x00FF0000) >> 12) );	// MOV #<DestAddress23:16>, W0
	send_cmd(0x8802A0);									// MOV W0, TBLPAG
	send_cmd(0x200006 | ((addr & 0x0000FFFF) << 4) );	// MOV #<DestAddress15:0>, W6

	/* Initialize the Write Pointer (W7) and store the next four locations of code memory to W0:W5 */
	send_cmd(0xEB0380);	// CLR W7
	send_nop();
	send_cmd(0xBA1B96);
	send_nop();
	send_nop();
	send_nop();
	send_nop();
	send_nop();
	send_cmd(0xBADBB6);
	send_nop();
	send_nop();
	send_nop();
	send_nop();
	send_nop();
	send_cmd(0xBADBD6);
	send_nop();
	send_nop();
	send_nop();
	send_nop();
	send_nop();
	send_cmd(0xBA1BB6);
	send_nop();
	send_nop();
	send_nop();
	send_nop();
	send_nop();
	send_cmd(0xBA1B96);
	send_nop();
	send_nop();
	send_nop();
	send_nop();
	send_nop();
	send_cmd(0xBADBB6);
	send_nop();
	send_nop();
	send_nop();
	send_nop();
	send_nop();
	send_cmd(0xBADBD6);
	send_nop();
	send_nop();
	send_nop();
	send_nop();
	send_nop();
	send_cmd(0xBA0BB6);
	send_nop();
	send_nop();
	send_nop();
	send_nop();
	send_nop();

	/* read six data words (16 bits each) */
	for(unsigned short i=0; i<6; i++){
		send_cmd(0x887E60 + i);
		send_nop();
		raw_data[i] = read_data();
		send_nop();
	}

	/*Reset the device’s internal PC*/
	exit_reset_vector();

	/*
	 * raw_data comes in "packed data format":
	 * LSWx: Least Significant 16 bits of instruction word
	 * MSBx: Most Significant Byte of instruction word
	 * 	  LSW0
	 * MSB1 | MSB0
	 *    LSW1
	 * 	  LSW2
	 * MSB3 | MSB2
	 *    LSW3
	 * so we need to store data correctly to be able to process it correctly 
	 */
	data[0] = raw_data[0];
	data[1] = raw_data[1] & 0x00FF;
	data[3] = (raw_data[1] & 0xFF00) >> 8;
	data[2] = raw_data[2];
	data[4] = raw_data[3];
	data[5] = raw_data[4] & 0x00FF;
	data[7] = (raw_data[4] & 0xFF00) >> 8;
	data[6] = raw_data[5];
}

/*
 * Read a 24-bit configuration word and reaturn it
 */
uint32_t dspic33ck::read_config_word(uint32_t addr)
{
	uint16_t data_head, data_body;

	/*  Exit the Reset vector */
	exit_reset_vector();

	/*  Initialize the TBLPAG register, the Write Pointer (W7) and the Read Pointer (W6) for the TBLRD instruction. */
	send_cmd(0x200000 | ((addr & 0x00FF0000) >> 12));
	send_cmd(0x20FCC7);
	send_cmd(0x8802A0);
	send_cmd(0x200006 | ((addr & 0x0000FFFF) << 4));
	
	/* Store the Configuration register and send the contents of the VISI register. */
	send_nop();
	send_cmd(0xBA8B96);
	send_nop();
	send_nop();
	send_nop();
	send_nop();
	send_nop();
	data_head = read_data(); // read upper byte

	send_cmd(0xBA0B96);
	send_nop();
	send_nop();
	send_nop();
	send_nop();
	send_nop();
	data_body = read_data(); // read lower word

	return ((data_head << 16) | data_body);
}

/*
 * Read the device ID and Revision and setup the application internal memory for the individual device 
 */
bool dspic33ck::read_device_id(void)
{
	bool found = 0;

	device_id = read_config_word(0xFF0000);
	device_rev = read_config_word(0xFF0002);

	for (unsigned short i=0;i < sizeof(piclist)/sizeof(piclist[0]);i++){

		if (piclist[i].device_id == device_id){

			strcpy(name, piclist[i].name);
			mem.code_memory_size = piclist[i].code_memory_size;
			mem.program_memory_size = 0x00801800;
			mem.location = (uint16_t*) calloc(mem.program_memory_size,sizeof(uint16_t));
			mem.filled = (bool*) calloc(mem.program_memory_size,sizeof(bool));
			found = 1;
			break;
		}
	}

	return found;
}

/*
 * Erase all user code memory and reset configuration registers
 */
void dspic33ck::bulk_erase(void)
{
	/* Exit the Reset vector */
	exit_reset_vector();

	/* Set the NVMCON register to erase all user program memory. */
	send_cmd(0x2400EA);
	send_cmd(0x88468A);
	send_nop();
	send_nop();

	/* Initiate the erase cycle */
	send_cmd(0x20055A);
	send_cmd(0x8846BA);
	send_cmd(0x200AAA);
	send_cmd(0x8846BA);
	send_cmd(0xA8E8D1);
	send_nop();
	send_nop();
	send_nop();

	delay_us(DELAY_P11);

	/* Generate clock pulses for the code memory Bulk Erase operation to complete until the WR bit is clear */
	do{
		send_nop();
		send_cmd(0x804680);
		send_nop();		
		send_cmd(0x887E60);
		send_nop();
		nvmcon = read_data();
		exit_reset_vector();
	} while((nvmcon & 0x8000) == 0x8000);
	
	if(flags.client) fprintf(stdout, "@FIN");
}

/*
 * Read all configuration registers and output them to the console.
 * The output will not be saved anywhere
 */
void dspic33ck::dump_configuration_registers(void)
{
	/* The configuration registers are always stored in the last 0xFE */
	uint32_t addr = mem.code_memory_size - 0xFE;
	uint32_t data = 0;

	cerr << endl << "Configuration registers:" << endl << endl;
	for(unsigned short i=0; i<17; i++){

		data = read_config_word(addr | regaddr[i]);
		fprintf(stderr," - %s: 0x%06x\n", regname[i], data);

		if(i == 15){
			addr = 0x801800;
		}
	}

	cerr << endl;
}

/* Read PIC memory and write the contents to a .hex file */
void dspic33ck::read(char *outfile, uint32_t start, uint32_t count)
{
	uint32_t addr, startaddr, stopaddr;
	uint32_t config_word;
	uint16_t data[8];
	int i=0;

	startaddr = start;
	stopaddr = mem.code_memory_size;
	if(count != 0 && count < stopaddr){
		stopaddr = startaddr + count;
		fprintf(stderr, "Read only %d memory locations, from %06X to %06X\n",
				count, startaddr, stopaddr);
	}

	if(!flags.debug) cerr << "[ 0%]";
	if(flags.client) fprintf(stdout, "@000");
	counter=0;

	/* Output data to W0:W5; repeat until all desired code memory is read. */
	for(addr=startaddr; addr < stopaddr; addr=addr+8) {

		read_four_code_words(data,addr);

		for(unsigned short i=0; i<8; i++){
			if (flags.debug)
				fprintf(stderr, "\n addr = 0x%06X data = 0x%04X",
						(addr+i), data[i]);

			if (i%2 == 0 && data[i] != 0xFFFF) {
				mem.location[addr+i]        = data[i];
				mem.filled[addr+i]      = 1;
			}

			if (i%2 == 1 && data[i] != 0x00FF) {
				mem.location[addr+i]        = data[i];
				mem.filled[addr+i]      = 1;
			}
		}

		if(counter != addr*100/stopaddr){
			counter = addr*100/stopaddr;
			if(flags.client)
				fprintf(stdout,"@%03d", counter);
			if(!flags.debug)
				fprintf(stderr,"\b\b\b\b\b[%2d%%]", counter);
		}

		/* TODO: checksum */
	}

	/*read configuration registers*/
	addr = mem.code_memory_size - 0xFE;
	for(i=0; i<17; i++){
		
		config_word = read_config_word(addr | regaddr[i]);

		if ((config_word & 0x0000FFFF) != 0x0000FFFF) {
			mem.location[addr] = (config_word & 0x0000FFFF);
			mem.filled[addr] = 1;
		}

		if ((config_word & 0x00FF0000) != 0x00FF0000) {
			mem.location[addr+1] = ((config_word & 0x00FF0000) >> 16);
			mem.filled[addr+1] = 1;
		}

		if(i == 15){
			addr = 0x801800;
		}
	}

	if(!flags.debug) cerr << "\b\b\b\b\b";
	if(flags.client) fprintf(stdout, "@FIN");
	write_inhx(&mem, outfile);
}

void dspic33ck::write(char *infile)
{
	uint16_t i,j;
	uint16_t data[8];
	uint32_t addr = 0;
	uint32_t temp_addr = 0;

	unsigned int filled_locations=1;

	filled_locations = read_inhx(infile, &mem);
	if(!filled_locations) return;

	bulk_erase();

	delay_us(100000);

	/* Exit reset vector */
	exit_reset_vector();

	/* Initialize the TBLPAG register for writing to the latches*/
	send_cmd(0x200FAC);
	send_cmd(0x8802AC);

	/* WRITE CODE MEMORY */
	if(!flags.debug) cerr << "[ 0%]";
	if(flags.client) fprintf(stdout, "@000");
	counter=0;

	for (addr = 0; addr < mem.code_memory_size; ){

		/*if nothing to write, skip*/
		while(!mem.filled[addr]) addr++;

		/*get next two words to write*/
		for(j=0;j<4;j++){
			if (mem.filled[addr+j])	data[j] = mem.location[addr+j];
			else data[j] = 0xFFFF;
			if(flags.debug)
				fprintf(stderr,"\n  Writing 0x%06X to address 0x%06X ", data[j], addr+j );
		}

		/* Load W0:W2 with the next two packed instruction words to program. */
		send_cmd(0x200000 | (data[0] << 4));
		send_cmd(0x200001 | (0x00FFFF & ((data[3] << 8) | (data[1] & 0x00FF))) <<4);
		send_cmd(0x200002 | (data[2] << 4));

		/*Set the Read Pointer (W6) and Write Pointer (W7), and load the (next set of) write latches*/
		send_cmd(0xEB0300);
		send_nop();
		send_cmd(0xEB0380);
		send_nop();
		send_cmd(0xBB0BB6);
		send_nop();
		send_nop();
		send_cmd(0xBBDBB6);
		send_nop();
		send_nop();
		send_cmd(0xBBEBB6);
		send_nop();
		send_nop();
		send_cmd(0xBB0B96);
		send_nop();
		send_nop();

		/*Set the NVMADRU/NVMADR register pair to point to the correct address*/
		send_cmd(0x200003 | ((addr & 0x0000FFFF) << 4));
		send_cmd(0x200004 | ((addr & 0x00FF0000) >> 12));
		send_cmd(0x884693);
		send_cmd(0x8846A4);

		/*Set the NVMCON register to program two instruction words*/
		send_cmd(0x24001A);
		send_nop();
		send_cmd(0x88468A);
		send_nop();
		send_nop();

		/*Initiate the write cycle*/
		send_cmd(0x20055A);
		send_cmd(0x8846BA);
		send_cmd(0x200AAA);
		send_cmd(0x8846BA);
		send_cmd(0xA8E8D1);
		send_nop();
		send_nop();
		send_nop();

		delay_us(DELAY_P13);

		/*Generate clock pulses for the program operation to complete until the WR bit is clear.*/
		do{
			send_nop();
			send_cmd(0x804680);
			send_nop();
			send_cmd(0x887E60);
			send_nop();
			nvmcon = read_data();
			send_nop();
			send_nop();
			send_nop();
			reset_pc();
			send_nop();
			send_nop();
			send_nop();
		} while((nvmcon & 0x8000) == 0x8000);

		if(counter != addr*100/filled_locations){
			if(flags.client)
				fprintf(stdout,"@%03d", (addr*100/(filled_locations+0x100)));
			if(!flags.debug)
				fprintf(stderr,"\b\b\b\b\b[%2d%%]", addr*100/(filled_locations+0x100));
			counter = addr*100/filled_locations;
		}

		addr += 4;
	}

	if(!flags.debug) cerr << "[100%]";
	if(!flags.debug) cerr << "\b\b\b\b\b\b";
	if(flags.client) fprintf(stdout, "@100");

	delay_us(100000);

	/* WRITE CONFIGURATION REGISTERS */
	if(!flags.debug)
		cerr << endl << "Writing Configuration registers..." << endl;

	/*Exit the Reset vector*/
	exit_reset_vector();

	/* Initialize the TBLPAG register for writing to the latches */
	send_cmd(0x200FAC);
	send_cmd(0x8802AC);

	addr = mem.code_memory_size - 0xFE;

	for(i=0; i<16; i += 2){

		/* Load W0:W1 with the next two Configuration Words to program */
		/*get first word*/
		addr = addr | regaddr[i];
		send_cmd(0x200000 | ((mem.location[addr] & 0x0000FFFF) << 4));
		send_cmd(0x200001 | ((mem.location[addr] & 0xFFFF0000) >> 12));

		temp_addr = addr;

		/*get second word*/
		addr = addr | regaddr[i+1];
		send_cmd(0x200002 | ((mem.location[addr] & 0x0000FFFF) << 4));
		send_cmd(0x200003 | ((mem.location[addr] & 0xFFFF0000) >> 12));

		/*Set the Write Pointer (W3) and load the write latches */
		send_cmd(0xEB0300);
		send_nop();
		send_cmd(0xBB0B00);
		send_nop();
		send_nop();
		send_cmd(0xBB9B01);
		send_nop();
		send_nop();
		send_cmd(0xBB0B02);
		send_nop();
		send_nop();
		send_cmd(0xBB9B03);
		send_nop();
		send_nop();

		/* Set the NVMADRU/NVMADR register pair to point to the correct Configuration Word address*/
		send_cmd(0x200004 | ((temp_addr & 0x0000FFFF) << 4));
		send_cmd(0x200005 | ((temp_addr & 0xFFFF0000) >> 12));
		send_cmd(0x884694);
		send_cmd(0x8846A5);

		/*Set the NVMCON register to program two instruction words*/
		send_cmd(0x24001A);
		send_nop();
		send_cmd(0x88468A);
		send_nop();
		send_nop();

		/* Initiate the write cycle */
		send_cmd(0x20055A);
		send_cmd(0x8846BA);
		send_cmd(0x200AAA);
		send_cmd(0x8846BA);
		send_cmd(0xA8E8D1);
		send_nop();
		send_nop();
		send_nop();

		delay_us(DELAY_P13);

		/*Generate clock pulses for the program operation to complete until the WR bit is clear*/
		do{
			send_nop();
			send_cmd(0x804680);
			send_nop();
			send_cmd(0x887E60);
			send_nop();
			nvmcon = read_data();
			send_nop();
			send_nop();
			send_nop();
			reset_pc();
			send_nop();
			send_nop();
			send_nop();
		} while((nvmcon & 0x8000) == 0x8000);

		if(flags.debug)
			fprintf(stderr,"\n - %s set to 0x%01x",
					regname[i], mem.location[addr]);

	}

	if(flags.debug) cerr << endl;

	delay_us(100000);

	/* VERIFY CODE MEMORY */
	if(!flags.noverify){
		if(!flags.debug) cerr << "Verifying Code Memory ...\n";
		if(!flags.debug) cerr << "[ 0%]";
		if(flags.client) fprintf(stdout, "@000");
		counter = 0;

		addr=0;
		for(addr=0; addr < mem.code_memory_size; addr=addr+8) {

			read_four_code_words(data, addr);
			
			for(i=0; i<8; i++){
				if (flags.debug)
					fprintf(stderr, "\n addr = 0x%06X data = 0x%04X", (addr+i), data[i]);

				if(mem.filled[addr+i] && data[i] != mem.location[addr+i]){
					fprintf(stderr,"\n\n ERROR at address %06X: written %04X but %04X read!\n\n",
									addr+i, mem.location[addr+i], data[i]);
					//return;
				}

			}

			if(counter != addr*100/mem.code_memory_size){
				counter = addr*100/mem.code_memory_size;
				if(flags.client)
					fprintf(stdout,"@%03d", counter);
				if(!flags.debug)
					fprintf(stderr,"\b\b\b\b\b[%2d%%]", counter);
			}
		}

		if(!flags.debug) cerr << "[100%]";
		if(!flags.debug) cerr << "\b\b\b\b\b";
		if(flags.client) fprintf(stdout, "@FIN");
	}
	else{
		if(flags.client) fprintf(stdout, "@FIN");
	}
}

uint8_t dspic33ck::blank_check(void)
{
	uint32_t addr;
	unsigned short i;
	uint16_t data[8];
	uint8_t ret = 0;

	if(!flags.debug) cerr << "[ 0%]";

	counter=0;

	/* exit reset vector */
	exit_reset_vector();

	/* Output data to W0:W5; repeat until all desired code memory is read. */
	for(addr=0; addr < (mem.code_memory_size - 0xFE); addr=addr+8) {

		read_four_code_words(data, addr);

		if(counter != addr*100/mem.code_memory_size){
			counter = addr*100/mem.code_memory_size;
			fprintf(stderr, "\b\b\b\b\b[%2d%%]", counter);	
		}

		for(i=0; i<8; i++){
			if(flags.debug)			
				fprintf(stderr, "\n addr = 0x%06X data = 0x%04X",
								(addr+i), data[i]);
			if ((i%2 == 0 && data[i] != 0xFFFF) || (i%2 == 1 && data[i] != 0x00FF)) {
				if(!flags.debug) cerr << "\b\b\b\b\b";
				ret = 1;
				fprintf(stderr, "\n Addr: 0x%08x \n", addr + i);
				fprintf(stderr, "\n Data: 0x%08x \n", data[i]);
				addr = mem.code_memory_size + 10;
				break;
			}
		}
	}

	if(addr <= (mem.code_memory_size + 8)){
		if(!flags.debug) cerr << "\b\b\b\b\b";
		ret = 0;
	};

	exit_reset_vector();

	return ret;
}