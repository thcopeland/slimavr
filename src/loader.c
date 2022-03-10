#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "loader.h"

// the max record size is 1+8+256+2
#define BUFFSIZE 512

#define fail(...)   do { \
                        printf(__VA_ARGS__); \
                        exit(1); \
                    } while (0)

int load_ihex(char *mem, int memsize, char *fname) {
    FILE *f = fopen(fname, "r");
    if (!f) fail("Unable to open %s\n", fname);
    char buff[BUFFSIZE];
    uint32_t base_offset = 0;
    int line = 1;

    while (fgets(buff, BUFFSIZE, f)) {
        uint32_t addr = 0;
        int count, type, i = 0;
        if (buff[i++] != ':') continue;
        if (sscanf(buff+i, "%2X%4X%2X", &count, &addr, &type) == 3) i += 8;
        else fail("Invalid HEX record at %s:%i\n", fname, line);

        switch(type) {
            case 0x00: // data
                addr += base_offset;
                if (addr+count >= (uint32_t) memsize) {
                    fail("HEX record address 0x%08x at %s:%i exceeds memory limit (%i bytes)\n", addr+count-1, fname, line, memsize);
                }
                while (count--) {
                    unsigned val;
                    if (sscanf(buff+i, "%2X", &val) == 1) mem[addr] = val;
                    else fail("HEX data record at %s:%i is too short\n", fname, line);
                    addr++;
                    i += 2;
                }
                break;
            case 0x01: // end of file
                goto exit;
            case 0x02: // extended segment address
                if (sscanf(buff+i, "%4X", &base_offset)) base_offset *= 16;
                else fail("Invalid HEX segment address record at %s:%i\n", fname, line);
                break;
            case 0x03: // start segment address, ignored
                break;
            case 0x04: // extended linear address, unsupported
            case 0x05: // start linear address, unsupported
            default:
                fail("Unsupported HEX record type %02X at %s:%i\n", type, fname, line);
        }

        line++;
    }

exit:
    fclose(f);
    return 0;
}
