#ifndef SLIMAVR_EEPROM_H
#define SLIMAVR_EEPROM_H

#define AVR_ECCR_EERE  0x01
#define AVR_ECCR_EEPE  0x02
#define AVR_ECCR_EEMPE 0x04
#define AVR_ECCR_EERIE 0x08
#define AVR_ECCR_EEPM0 0x10
#define AVR_ECCR_EEPM1 0x20
#define AVR_ECCR_MASK  (AVR_ECCR_EERE|AVR_ECCR_EEPE|AVR_ECCR_EEMPE|AVR_ECCR_EERIE|AVR_ECCR_EEPM0|AVR_ECCR_EEPM1)

struct avr_eeprom_state {
  uint16_t progress;  // progress of EEPROM write (0 - complete)
  uint16_t addr;      // EEPROM write address
  uint8_t value;      // value to write to EEPROM
  uint8_t status;     // [interrupt:4][access window timer:4]
};

struct avr;

void avr_init_eeprom_state(struct avr_eeprom_state *eep);
void avr_set_eeprom_reg(struct avr *avr, uint16_t addr, uint8_t val, uint8_t mask);
void avr_update_eeprom(struct avr *avr);

#endif
