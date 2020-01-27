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

class dspic33ck : public Pic{

	public:
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
		uint16_t read_data(void);
        inline void exit_reset_vector(void);
        void read_four_code_words(uint16_t *data, uint32_t addr);
        uint32_t read_config_word(uint32_t addr);

		/*
		* DEVICES SECTION
		*    ID       NAME           	  MEMSIZE
		*/
		pic_device piclist[38] = {
            {0x7C74, "dsPIC33CK256MP508", 0x02BFFE},
            {0x7C73, "dsPIC33CK256MP506", 0x02BFFE},
            {0x7C72, "dsPIC33CK256MP505", 0x02BFFE},
            {0x7C71, "dsPIC33CK256MP503", 0x02BFFE},
            {0x7C70, "dsPIC33CK256MP502", 0x02BFFE},
            {0x7C64, "dsPIC33CK128MP508", 0x015FFE},
            {0x7C63, "dsPIC33CK128MP506", 0x015FFE},
            {0x7C62, "dsPIC33CK128MP505", 0x015FFE},
            {0x7C61, "dsPIC33CK128MP503", 0x015FFE},
            {0x7C60, "dsPIC33CK128MP502", 0x015FFE},
            {0x7C54, "dsPIC33CK64MP508", 0x00AFFE},
            {0x7C53, "dsPIC33CK64MP506", 0x00AFFE},
            {0x7C52, "dsPIC33CK64MP505", 0x00AFFE},
            {0x7C51, "dsPIC33CK64MP503", 0x00AFFE},
            {0x7C50, "dsPIC33CK64MP502", 0x00AFFE},
            {0x7C43, "dsPIC33CK32MP506", 0x005FFE},
            {0x7C42, "dsPIC33CK32MP505", 0x005FFE},
            {0x7C41, "dsPIC33CK32MP503", 0x005FFE},
            {0x7C40, "dsPIC33CK32MP502", 0x005FFE},
            {0x7C34, "dsPIC33CK256MP208", 0x02BFFE},
            {0x7C33, "dsPIC33CK256MP206", 0x02BFFE},
            {0x7C32, "dsPIC33CK256MP205", 0x02BFFE},
            {0x7C31, "dsPIC33CK256MP203", 0x02BFFE},
            {0x7C30, "dsPIC33CK256MP202", 0x02BFFE},
            {0x7C24, "dsPIC33CK128MP208", 0x015FFE},
            {0x7C23, "dsPIC33CK128MP206", 0x015FFE},
            {0x7C22, "dsPIC33CK128MP205", 0x015FFE},
            {0x7C21, "dsPIC33CK128MP203", 0x015FFE},
            {0x7C20, "dsPIC33CK128MP202", 0x015FFE},
            {0x7C14, "dsPIC33CK64MP208", 0x00AFFE},
            {0x7C13, "dsPIC33CK64MP206", 0x00AFFE},
            {0x7C12, "dsPIC33CK64MP205", 0x00AFFE},
            {0x7C11, "dsPIC33CK64MP203", 0x00AFFE},
            {0x7C10, "dsPIC33CK64MP202", 0x00AFFE},
            {0x7C03, "dsPIC33CK32MP206", 0x005FFE},
            {0x7C02, "dsPIC33CK32MP205", 0x005FFE},
            {0x7C01, "dsPIC33CK32MP203", 0x005FFE},
            {0x7C00, "dsPIC33CK32MP202", 0x005FFE}
        };
        
        // Configuration register adress endings
        uint8_t regaddr[17] = {
            0x00,
            0x10,
            0x14,
            0x18,
            0x1C,
            0x20,
            0x24,
            0x28,
            0x2C,
            0x30,
            0x34,
            0x38,
            0x3C,
            0x40,
            0x44,
            0xFC,
            0x00
        };

        // Configuration register names
        const char regname[17][10] = {
            "FSEC", 
            "FBSLIM", 
            "FSIGN",
            "FOSCSEL", 
            "FOSC", 
            "FWDT", 
            "FPOR", 
            "FICD", 
            "FDMTIVTL", 
            "FDMTIVTH",
            "FDMTCNTL",
            "FDMTCNTH", 
            "FDMT", 
            "FDEVOPT", 
            "FALTREG", 
            "FBTSEQ",
            "FBOOT"
        };
};