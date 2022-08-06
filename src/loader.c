#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include "opt.h"
#include "loader.h"

#define BUFFSIZE 512

int avr_load_ihex(struct avr *avr, char *fname) {
    FILE *f = fopen(fname, "r");
    char buff[BUFFSIZE];
    uint32_t line = 1,
             base_addr = 0;

     if (!f) {
         printf("avr_load_ihex: %s\n", strerror(errno));
         return AVR_EFILE;
     }

    while (fgets(buff, BUFFSIZE, f)) {
        unsigned val, addr, count, type;
        // read the record header
        if (sscanf(buff, ":%2X%4X%2X", &count, &addr, &type) != 3) {
            printf("avr_load_ihex: malformed header at %s:%d\n", fname, line);
            fclose(f);
            return AVR_EFORMAT;
        }
        int i = 9;
        int checksum = count + addr + (addr >> 8) + type;

        // read and load the record data
        switch(type) {
            case 0x00: // data
                addr += base_addr;
                while (count--) {
                    sscanf(buff+i, "%2X", &val);
                    avr->rom[addr++] = val;
                    checksum += val;
                    i += 2;
                }
                break;
            case 0x01: // end of file
                goto done;
            case 0x02: // extended segment address
                sscanf(buff+i, "%4X", &val);
                checksum += val + (val>>8);
                base_addr = (val << 4);
                i += 4;
                break;
            case 0x03: // start segment address, ignored
                break;
            case 0x04: // extended linear address, unsupported
            case 0x05: // start linear address, unsupported
                printf("avr_load_ihex: unsupported record type %02X at %s:%d\n", type, fname, line);
                fclose(f);
                return AVR_EFORMAT;
            default:
                fclose(f);
                return AVR_EFORMAT;
        }

        // test checksum
        sscanf(buff+i, "%2X", &val);
        if ((val + checksum) & 0xff) {
            printf("avr_load_ihex: checksum failed at %s:%d (0x%02X != 0x%02X)\n", fname, line, (-checksum) & 0xff, val);
            fclose(f);
            return AVR_ECHECKSUM;
        }

        line++;
    }

done:
    fclose(f);
    return 0;
}
