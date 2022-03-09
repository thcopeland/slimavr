#include "avr.h"
#include "loader.h"

int main(int argc, char **argv) {
    char buff[100];
    load_ihex(buff, 100, "hello");
}
