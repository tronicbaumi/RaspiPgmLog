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

#include <iostream>

#include "../common.h"
#include "device.h"

using namespace std;

#define SF_DSPIC33E		0x00
#define SF_PIC24FJ		0x01

class dspic33e : public Pic{

	public:
		dspic33e(uint8_t sf){
			subfamily=sf;
		};
		void enter_program_mode(void);
		void exit_program_mode(void);
		bool setup_pe(void){return true;};
		bool read_device_id(void);
		void bulk_erase(void);
		void dump_configuration_registers(void);
		void read(char *outfile, uint32_t start, uint32_t count);
		void write(char *infile);
		uint8_t blank_check(void);

	protected:
		void send_cmd(uint32_t cmd);
		inline void send_prog_nop(void);
		uint16_t read_data(void);

		/*
		* DEVICES SECTION
		*                       ID       NAME           	  MEMSIZE
		*/
		pic_device piclist[80] = {{0x1C19, "PIC24EP32GP202", 0x0057EA}, 
									{0x1C1A, "PIC24EP32GP203", 0x0057EA},
									{0x1C18, "PIC24EP32GP204", 0x0057EA},
									{0x1C0D, "dsPIC33EP32GP502", 0x0057EA},  
									{0x1C0E, "dsPIC33EP32GP503", 0x0057EA}, 
									{0x1C0C, "dsPIC33EP32GP504", 0x0057EA},  
									{0x1C11, "PIC24EP32MC202", 0x0057EA},  
									{0x1C12, "PIC24EP32MC203", 0x0057EA},  
									{0x1C10, "PIC24EP32MC204", 0x0057EA},  
									{0x1C01, "dsPIC33EP32MC202", 0x0057EA},   
									{0x1C02, "dsPIC33EP32MC203", 0x0057EA},  
									{0x1C00, "dsPIC33EP32MC204", 0x0057EA},  
									{0x1C05, "dsPIC33EP32MC502", 0x0057EA},  
									{0x1C06, "dsPIC33EP32MC503", 0x0057EA},  
									{0x1C04, "dsPIC33EP32MC504", 0x0057EA},  
									{0x1D39, "PIC24EP64GP202", 0x00AFEA},  
									{0x1D3A, "PIC24EP64GP203", 0x00AFEA},  
									{0x1D38, "PIC24EP64GP204", 0x00AFEA},  
									{0x1D3B, "PIC24EP64GP206", 0x00AFEA},  
									{0x1D2D, "dsPIC33EP64GP502", 0x00AFEA}, 
									{0x1D2E, "dsPIC33EP64GP503", 0x00AFEA},  
									{0x1D2C, "dsPIC33EP64GP504", 0x00AFEA}, 
									{0x1D2F, "dsPIC33EP64GP506", 0x00AFEA},  
									{0x1D31, "PIC24EP64MC202", 0x00AFEA},  
									{0x1D32, "PIC24EP64MC203", 0x00AFEA}, 
									{0x1D30, "PIC24EP64MC204", 0x00AFEA},  
									{0x1D33, "PIC24EP64MC206", 0x00AFEA},  
									{0x1D21, "dsPIC33EP64MC202", 0x00AFEA},  
									{0x1D22, "dsPIC33EP64MC203", 0x00AFEA},  
									{0x1D20, "dsPIC33EP64MC204", 0x00AFEA},  
									{0x1D23, "dsPIC33EP64MC206", 0x00AFEA},  
									{0x1D25, "dsPIC33EP64MC502", 0x00AFEA},  
									{0x1D26, "dsPIC33EP64MC503", 0x00AFEA},  
									{0x1D24, "dsPIC33EP64MC504", 0x00AFEA},  
									{0x1D27, "dsPIC33EP64MC506", 0x00AFEA},  
									{0x1E59, "PIC24EP128GP202", 0x0157EA},  
									{0x1E58, "PIC24EP128GP204", 0x0157EA},  
									{0x1E5B, "PIC24EP128GP206", 0x0157EA}, 
									{0x1E4D, "dsPIC33EP128GP502", 0x0157EA},  
									{0x1E4C, "dsPIC33EP128GP504", 0x0157EA},  
									{0x1E4F, "dsPIC33EP128GP506", 0x0157EA},  
									{0x1E51, "PIC24EP128MC202", 0x0157EA},  
									{0x1E50, "PIC24EP128MC204", 0x0157EA},  
									{0x1E53, "PIC24EP128MC206", 0x0157EA},  
									{0x1E41, "dsPIC33EP128MC202", 0x0157EA},  
									{0x1E40, "dsPIC33EP128MC204", 0x0157EA},  
									{0x1E43, "dsPIC33EP128MC206", 0x0157EA},  
									{0x1E45, "dsPIC33EP128MC502", 0x0157EA},  
									{0x1E44, "dsPIC33EP128MC504", 0x0157EA},  
									{0x1E47, "dsPIC33EP128MC506", 0x0157EA},  
									{0x1F79, "PIC24EP256GP202", 0x02AFEA},  
									{0x1F78, "PIC24EP256GP204", 0x02AFEA},  
									{0x1F7B, "PIC24EP256GP206", 0x02AFEA},  
									{0x1F6D, "dsPIC33EP256GP502", 0x02AFEA},  
									{0x1F6C, "dsPIC33EP256GP504", 0x02AFEA},  
									{0x1F6F, "dsPIC33EP256GP506", 0x02AFEA},  
									{0x1F71, "PIC24EP256MC202", 0x02AFEA}, 
									{0x1F70, "PIC24EP256MC204", 0x02AFEA},  
									{0x1F73, "PIC24EP256MC206", 0x02AFEA},  
									{0x1F61, "dsPIC33EP256MC202", 0x02AFEA},  
									{0x1F60, "dsPIC33EP256MC204", 0x02AFEA},  
									{0x1F63, "dsPIC33EP256MC206", 0x02AFEA},  
									{0x1F65, "dsPIC33EP256MC502", 0x02AFEA},  
									{0x1F64, "dsPIC33EP256MC504", 0x02AFEA},  
									{0x1F67, "dsPIC33EP256MC506", 0x02AFEA},  
									{0x1799, "PIC24EP512GP202", 0x0557EA},  
									{0x1798, "PIC24EP512GP204", 0x0557EA}, 
									{0x179B, "PIC24EP512GP206", 0x0557EA}, 
									{0x178D, "dsPIC33EP512GP502", 0x0557EA},  
									{0x178C, "dsPIC33EP512GP504", 0x0557EA},  
									{0x178F, "dsPIC33EP512GP506", 0x0557EA}, 
									{0x1791, "PIC24EP512MC202", 0x0557EA}, 
									{0x1790, "PIC24EP512MC204", 0x0557EA},  
									{0x1793, "PIC24EP512MC206", 0x0557EA},  
									{0x1781, "dsPIC33EP512MC202", 0x0557EA}, 
									{0x1780, "dsPIC33EP512MC204", 0x0557EA},  
									{0x1783, "dsPIC33EP512MC206", 0x0557EA},  
									{0x1785, "dsPIC33EP512MC502", 0x0557EA}, 
									{0x1784, "dsPIC33EP512MC504", 0x0557EA},  
									{0x1787, "dsPIC33EP512MC506", 0x0557EA}};
};