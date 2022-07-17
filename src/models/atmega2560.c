#include "../model.h"

static struct avr_register regmap[] = {
	{ .type = REG_VALUE },         // r0 (0x0)
	{ .type = REG_VALUE },         // r1 (0x1)
	{ .type = REG_VALUE },         // r2 (0x2)
	{ .type = REG_VALUE },         // r3 (0x3)
	{ .type = REG_VALUE },         // r4 (0x4)
	{ .type = REG_VALUE },         // r5 (0x5)
	{ .type = REG_VALUE },         // r6 (0x6)
	{ .type = REG_VALUE },         // r7 (0x7)
	{ .type = REG_VALUE },         // r8 (0x8)
	{ .type = REG_VALUE },         // r9 (0x9)
	{ .type = REG_VALUE },         // r10 (0xa)
	{ .type = REG_VALUE },         // r11 (0xb)
	{ .type = REG_VALUE },         // r12 (0xc)
	{ .type = REG_VALUE },         // r13 (0xd)
	{ .type = REG_VALUE },         // r14 (0xe)
	{ .type = REG_VALUE },         // r15 (0xf)
	{ .type = REG_VALUE },         // r16 (0x10)
	{ .type = REG_VALUE },         // r17 (0x11)
	{ .type = REG_VALUE },         // r18 (0x12)
	{ .type = REG_VALUE },         // r19 (0x13)
	{ .type = REG_VALUE },         // r20 (0x14)
	{ .type = REG_VALUE },         // r21 (0x15)
	{ .type = REG_VALUE },         // r22 (0x16)
	{ .type = REG_VALUE },         // r23 (0x17)
	{ .type = REG_VALUE },         // r24 (0x18)
	{ .type = REG_VALUE },         // r25 (0x19)
	{ .type = REG_VALUE },         // r26 (0x1a)
	{ .type = REG_VALUE },         // r27 (0x1b)
	{ .type = REG_VALUE },         // r28 (0x1c)
	{ .type = REG_VALUE },         // r29 (0x1d)
	{ .type = REG_VALUE },         // r30 (0x1e)
	{ .type = REG_VALUE },         // r31 (0x1f)
	{ .type = REG_VALUE },         // PINA (0x20)
	{ .type = REG_VALUE },         // DDRA (0x21)
	{ .type = REG_VALUE },         // PORTA (0x22)
	{ .type = REG_VALUE },         // PINB (0x23)
	{ .type = REG_VALUE },         // DDRB (0x24)
	{ .type = REG_VALUE },         // PORTB (0x25)
	{ .type = REG_VALUE },         // PINC (0x26)
	{ .type = REG_VALUE },         // DDRC (0x27)
	{ .type = REG_VALUE },         // PORTC (0x28)
	{ .type = REG_VALUE },         // PIND (0x29)
	{ .type = REG_VALUE },         // DDRD (0x2a)
	{ .type = REG_VALUE },         // PORTD (0x2b)
	{ .type = REG_VALUE },         // PINE (0x2c)
	{ .type = REG_VALUE },         // DDRE (0x2d)
	{ .type = REG_VALUE },         // PORTE (0x2e)
	{ .type = REG_VALUE },         // PINF (0x2f)
	{ .type = REG_VALUE },         // DDRF (0x30)
	{ .type = REG_VALUE },         // PORTF (0x31)
	{ .type = REG_VALUE },         // PING (0x32)
	{ .type = REG_VALUE },         // DDRG (0x33)
	{ .type = REG_VALUE },         // PORTG (0x34)
	{ .type = REG_CLEAR_ON_SET },  // TIFR0 (0x35)
	{ .type = REG_CLEAR_ON_SET },  // TIFR1 (0x36)
	{ .type = REG_CLEAR_ON_SET },  // TIFR2 (0x37)
	{ .type = REG_CLEAR_ON_SET },  // TIFR3 (0x38)
	{ .type = REG_CLEAR_ON_SET },  // TIFR4 (0x39)
	{ .type = REG_CLEAR_ON_SET },  // TIFR5 (0x3a)
	{ .type = REG_UNSUPPORTED },   // PCIFR (0x3b)
	{ .type = REG_UNSUPPORTED },   // EIFR (0x3c)
	{ .type = REG_UNSUPPORTED },   // EIMSK (0x3d)
	{ .type = REG_VALUE },         // GPIOR0 (0x3e)
	{ .type = REG_UNSUPPORTED },   // EECR (0x3f)
	{ .type = REG_UNSUPPORTED },   // EEDR (0x40)
	{ .type = REG_UNSUPPORTED },   // EEARL (0x41)
	{ .type = REG_UNSUPPORTED },   // EEARH (0x42)
	{ .type = REG_VALUE },         // GTCCR (0x43)
	{ .type = REG_VALUE },         // TCCR0A (0x44)
	{ .type = REG_VALUE },         // TCCR0B (0x45)
	{ .type = REG_VALUE },         // TCNT0  (0x46)
	{ .type = REG_TIMER5_LOW },    // OCR0A  (0x47)
	{ .type = REG_TIMER5_LOW },    // OCR0B  (0x48)
	{ .type = REG_RESERVED },      // Reserved (0x49)
	{ .type = REG_VALUE },         // GPIOR1 (0x4a)
	{ .type = REG_VALUE },         // GPIOR2 (0x4b)
	{ .type = REG_UNSUPPORTED },   // SPCR (0x4c)
	{ .type = REG_UNSUPPORTED },   // SPSR (0x4d)
	{ .type = REG_UNSUPPORTED },   // SPDR (0x4e)
	{ .type = REG_RESERVED },      // Reserved (0x4f)
	{ .type = REG_UNSUPPORTED },   // ACSR (0x50)
	{ .type = REG_UNSUPPORTED },   // OCDR (0x51)
	{ .type = REG_RESERVED },      // Reserved (0x52)
	{ .type = REG_UNSUPPORTED },   // SMCR (0x53)
	{ .type = REG_UNSUPPORTED },   // MCUSR (0x54)
	{ .type = REG_UNSUPPORTED },   // MCUCR (0x55)
	{ .type = REG_RESERVED },      // Reserved (0x56)
	{ .type = REG_VALUE },         // SPMCSR (0x57)
	{ .type = REG_RESERVED },      // Reserved (0x58)
	{ .type = REG_RESERVED },      // Reserved (0x59)
	{ .type = REG_RESERVED },      // Reserved (0x5a)
	{ .type = REG_VALUE },         // RAMPZ (0x5b)
	{ .type = REG_VALUE },         // EIND (0x5c)
	{ .type = REG_VALUE },         // SPL (0x5d)
	{ .type = REG_VALUE },         // SPH (0x5e)
	{ .type = REG_VALUE },         // SREG (0x5f)
	{ .type = REG_UNSUPPORTED },   // WDTCSR (0x60)
	{ .type = REG_UNSUPPORTED },   // CLKPR (0x61)
	{ .type = REG_RESERVED },      // Reserved (0x62)
	{ .type = REG_RESERVED },      // Reserved (0x63)
	{ .type = REG_UNSUPPORTED },   // USART0 (0x64)
	{ .type = REG_UNSUPPORTED },   // USART3 (0x65)
	{ .type = REG_UNSUPPORTED },   // OSCCAL (0x66)
	{ .type = REG_RESERVED },      // Reserved (0x67)
	{ .type = REG_UNSUPPORTED },   // PCICR (0x68)
	{ .type = REG_UNSUPPORTED },   // EICRA (0x69)
	{ .type = REG_UNSUPPORTED },   // EICRB (0x6a)
	{ .type = REG_UNSUPPORTED },   // PCMSK0 (0x6b)
	{ .type = REG_UNSUPPORTED },   // PCMSK1 (0x6c)
	{ .type = REG_UNSUPPORTED },   // PCMSK2 (0x6d)
	{ .type = REG_VALUE },         // TIMSK0 (0x6e)
	{ .type = REG_VALUE },         // TIMSK1 (0x6f)
	{ .type = REG_VALUE },         // TIMSK2 (0x70)
	{ .type = REG_VALUE },         // TIMSK3 (0x71)
	{ .type = REG_VALUE },         // TIMSK4 (0x72)
	{ .type = REG_VALUE },         // TIMSK5 (0x73)
	{ .type = REG_UNSUPPORTED },   // XMCRA (0x74)
	{ .type = REG_UNSUPPORTED },   // XMCRB (0x75)
	{ .type = REG_RESERVED },      // Reserved (0x76)
	{ .type = REG_RESERVED },      // Reserved (0x77)
	{ .type = REG_UNSUPPORTED },   // ADCL (0x78)
	{ .type = REG_UNSUPPORTED },   // ADCH (0x79)
	{ .type = REG_UNSUPPORTED },   // ADCSRA (0x7a)
	{ .type = REG_UNSUPPORTED },   // ADCSRB (0x7b)
	{ .type = REG_UNSUPPORTED },   // ADMUX (0x7c)
	{ .type = REG_UNSUPPORTED },   // DIDR2 (0x7d)
	{ .type = REG_UNSUPPORTED },   // DIDR0 (0x7e)
	{ .type = REG_UNSUPPORTED },   // DIDR1 (0x7f)
	{ .type = REG_VALUE },         // TCCR1A (0x80)
	{ .type = REG_VALUE },         // TCCR1B (0x81)
	{ .type = REG_VALUE },         // TCCR1C (0x82)
	{ .type = REG_RESERVED },      // Reserved (0x83)
	{ .type = REG_TIMER5_LOW },    // TCNT1L (0x84)
	{ .type = REG_TIMER5_HIGH },   // TCNT1H (0x85)
	{ .type = REG_VALUE },         // ICR1L (0x86)
	{ .type = REG_VALUE },         // ICR1H (0x87)
	{ .type = REG_TIMER5_LOW },    // OCR1AL (0x88)
	{ .type = REG_TIMER5_HIGH },   // OCR1AH (0x89)
	{ .type = REG_TIMER5_LOW },    // OCR1BL (0x8a)
	{ .type = REG_TIMER5_HIGH },   // OCR1BH (0x8b)
	{ .type = REG_TIMER5_LOW },    // OCR1CL (0x8c)
	{ .type = REG_TIMER5_HIGH },   // OCR1CH (0x8d)
	{ .type = REG_RESERVED },      // Reserved (0x8e)
	{ .type = REG_RESERVED },      // Reserved (0x8f)
	{ .type = REG_VALUE },         // TCCR3A (0x90)
	{ .type = REG_VALUE },         // TCCR3B (0x91)
	{ .type = REG_VALUE },         // TCCR3C (0x92)
	{ .type = REG_RESERVED },      // Reserved (0x93)
	{ .type = REG_TIMER5_LOW },    // TCNT3L (0x94)
	{ .type = REG_TIMER5_HIGH },   // TCNT3H (0x95)
	{ .type = REG_VALUE },         // ICR3L (0x96)
	{ .type = REG_VALUE },         // ICR3H (0x97)
	{ .type = REG_TIMER5_LOW },    // OCR3AL (0x98)
	{ .type = REG_TIMER5_HIGH },   // OCR3AH (0x99)
	{ .type = REG_TIMER5_LOW },    // OCR3BL (0x9a)
	{ .type = REG_TIMER5_HIGH },   // OCR3BH (0x9b)
	{ .type = REG_TIMER5_LOW },    // OCR3CL (0x9c)
	{ .type = REG_TIMER5_HIGH },   // OCR3CH (0x9d)
	{ .type = REG_RESERVED },      // Reserved (0x9e)
	{ .type = REG_RESERVED },      // Reserved (0x9f)
	{ .type = REG_VALUE },         // TCCR4A (0xa0)
	{ .type = REG_VALUE },         // TCCR4B (0xa1)
	{ .type = REG_VALUE },         // TCCR4C (0xa2)
	{ .type = REG_RESERVED },      // Reserved (0xa3)
	{ .type = REG_TIMER5_LOW },    // TCNT4L (0xa4)
	{ .type = REG_TIMER5_HIGH },   // TCNT4H (0xa5)
	{ .type = REG_VALUE },         // ICR4L (0xa6)
	{ .type = REG_VALUE },         // ICR4H (0xa7)
	{ .type = REG_TIMER5_LOW },    // OCR4AL (0xa8)
	{ .type = REG_TIMER5_HIGH },   // OCR4AH (0xa9)
	{ .type = REG_TIMER5_LOW },    // OCR4BL (0xaa)
	{ .type = REG_TIMER5_HIGH },   // OCR4BH (0xab)
	{ .type = REG_TIMER5_LOW },    // OCR4CL (0xac)
	{ .type = REG_TIMER5_HIGH },   // OCR4CH (0xad)
	{ .type = REG_RESERVED },      // Reserved (0xae)
	{ .type = REG_RESERVED },      // Reserved (0xaf)
	{ .type = REG_VALUE },         // TCCR2A (0xb0)
	{ .type = REG_VALUE },         // TCCR2B (0xb1)
	{ .type = REG_VALUE },         // TCNT2  (0xb2)
	{ .type = REG_TIMER5_LOW },    // OCR2A  (0xb3)
	{ .type = REG_TIMER5_LOW },    // OCR2B  (0xb4)
	{ .type = REG_RESERVED },      // Reserved (0xb5)
	{ .type = REG_UNSUPPORTED },   // ASSR (0xb6)
	{ .type = REG_RESERVED },      // Reserved (0xb7)
	{ .type = REG_UNSUPPORTED },   // TWBR (0xb8)
	{ .type = REG_UNSUPPORTED },   // TWSR (0xb9)
	{ .type = REG_UNSUPPORTED },   // TWAR (0xba)
	{ .type = REG_UNSUPPORTED },   // TWDR (0xbb)
	{ .type = REG_UNSUPPORTED },   // TWCR (0xbc)
	{ .type = REG_UNSUPPORTED },   // TWAMR (0xbd)
	{ .type = REG_RESERVED },      // Reserved (0xbe)
	{ .type = REG_RESERVED },      // Reserved (0xbf)
	{ .type = REG_UNSUPPORTED },   // UCSR0A (0xc0)
	{ .type = REG_UNSUPPORTED },   // UCSR0B (0xc1)
	{ .type = REG_UNSUPPORTED },   // UCSR0C (0xc2)
	{ .type = REG_RESERVED },      // Reserved (0xc3)
	{ .type = REG_UNSUPPORTED },   // USART0 (0xc4)
	{ .type = REG_UNSUPPORTED },   // USART0 (0xc5)
	{ .type = REG_UNSUPPORTED },   // UDR0 (0xc6)
	{ .type = REG_RESERVED },      // Reserved (0xc7)
	{ .type = REG_UNSUPPORTED },   // UCSR1A (0xc8)
	{ .type = REG_UNSUPPORTED },   // UCSR1B (0xc9)
	{ .type = REG_UNSUPPORTED },   // UCSR1C (0xca)
	{ .type = REG_RESERVED },      // Reserved (0xcb)
	{ .type = REG_UNSUPPORTED },   // USART1 (0xcc)
	{ .type = REG_UNSUPPORTED },   // USART1 (0xcd)
	{ .type = REG_UNSUPPORTED },   // UDR1 (0xce)
	{ .type = REG_RESERVED },      // Reserved (0xcf)
	{ .type = REG_UNSUPPORTED },   // UCSR2A (0xd0)
	{ .type = REG_UNSUPPORTED },   // UCSR2B (0xd1)
	{ .type = REG_UNSUPPORTED },   // UCSR2C (0xd2)
	{ .type = REG_RESERVED },      // Reserved (0xd3)
	{ .type = REG_UNSUPPORTED },   // USART2 (0xd4)
	{ .type = REG_UNSUPPORTED },   // USART2 (0xd5)
	{ .type = REG_UNSUPPORTED },   // UDR2 (0xd6)
	{ .type = REG_RESERVED },      // Reserved (0xd7)
	{ .type = REG_RESERVED },      // Reserved (0xd8)
	{ .type = REG_RESERVED },      // Reserved (0xd9)
	{ .type = REG_RESERVED },      // Reserved (0xda)
	{ .type = REG_RESERVED },      // Reserved (0xdb)
	{ .type = REG_RESERVED },      // Reserved (0xdc)
	{ .type = REG_RESERVED },      // Reserved (0xdd)
	{ .type = REG_RESERVED },      // Reserved (0xde)
	{ .type = REG_RESERVED },      // Reserved (0xdf)
	{ .type = REG_RESERVED },      // Reserved (0xe0)
	{ .type = REG_RESERVED },      // Reserved (0xe1)
	{ .type = REG_RESERVED },      // Reserved (0xe2)
	{ .type = REG_RESERVED },      // Reserved (0xe3)
	{ .type = REG_RESERVED },      // Reserved (0xe4)
	{ .type = REG_RESERVED },      // Reserved (0xe5)
	{ .type = REG_RESERVED },      // Reserved (0xe6)
	{ .type = REG_RESERVED },      // Reserved (0xe7)
	{ .type = REG_RESERVED },      // Reserved (0xe8)
	{ .type = REG_RESERVED },      // Reserved (0xe9)
	{ .type = REG_RESERVED },      // Reserved (0xea)
	{ .type = REG_RESERVED },      // Reserved (0xeb)
	{ .type = REG_RESERVED },      // Reserved (0xec)
	{ .type = REG_RESERVED },      // Reserved (0xed)
	{ .type = REG_RESERVED },      // Reserved (0xee)
	{ .type = REG_RESERVED },      // Reserved (0xef)
	{ .type = REG_RESERVED },      // Reserved (0xf0)
	{ .type = REG_RESERVED },      // Reserved (0xf1)
	{ .type = REG_RESERVED },      // Reserved (0xf2)
	{ .type = REG_RESERVED },      // Reserved (0xf3)
	{ .type = REG_RESERVED },      // Reserved (0xf4)
	{ .type = REG_RESERVED },      // Reserved (0xf5)
	{ .type = REG_RESERVED },      // Reserved (0xf6)
	{ .type = REG_RESERVED },      // Reserved (0xf7)
	{ .type = REG_RESERVED },      // Reserved (0xf8)
	{ .type = REG_RESERVED },      // Reserved (0xf9)
	{ .type = REG_RESERVED },      // Reserved (0xfa)
	{ .type = REG_RESERVED },      // Reserved (0xfb)
	{ .type = REG_RESERVED },      // Reserved (0xfc)
	{ .type = REG_RESERVED },      // Reserved (0xfd)
	{ .type = REG_RESERVED },      // Reserved (0xfe)
	{ .type = REG_RESERVED },      // Reserved (0xff)
	{ .type = REG_VALUE },         // PINH (0x100)
	{ .type = REG_VALUE },         // DDRH (0x101)
	{ .type = REG_VALUE },         // PORTH (0x102)
	{ .type = REG_VALUE },         // PINJ (0x103)
	{ .type = REG_VALUE },         // DDRJ (0x104)
	{ .type = REG_VALUE },         // PORTJ (0x105)
	{ .type = REG_VALUE },         // PINK (0x106)
	{ .type = REG_VALUE },         // DDRK (0x107)
	{ .type = REG_VALUE },         // PORTK (0x108)
	{ .type = REG_VALUE },         // PINL (0x109)
	{ .type = REG_VALUE },         // DDRL (0x10a)
	{ .type = REG_VALUE },         // PORTL (0x10b)
	{ .type = REG_RESERVED },      // Reserved (0x10c)
	{ .type = REG_RESERVED },      // Reserved (0x10d)
	{ .type = REG_RESERVED },      // Reserved (0x10e)
	{ .type = REG_RESERVED },      // Reserved (0x10f)
	{ .type = REG_RESERVED },      // Reserved (0x110)
	{ .type = REG_RESERVED },      // Reserved (0x111)
	{ .type = REG_RESERVED },      // Reserved (0x112)
	{ .type = REG_RESERVED },      // Reserved (0x113)
	{ .type = REG_RESERVED },      // Reserved (0x114)
	{ .type = REG_RESERVED },      // Reserved (0x115)
	{ .type = REG_RESERVED },      // Reserved (0x116)
	{ .type = REG_RESERVED },      // Reserved (0x117)
	{ .type = REG_RESERVED },      // Reserved (0x118)
	{ .type = REG_RESERVED },      // Reserved (0x119)
	{ .type = REG_RESERVED },      // Reserved (0x11a)
	{ .type = REG_RESERVED },      // Reserved (0x11b)
	{ .type = REG_RESERVED },      // Reserved (0x11c)
	{ .type = REG_RESERVED },      // Reserved (0x11d)
	{ .type = REG_RESERVED },      // Reserved (0x11e)
	{ .type = REG_RESERVED },      // Reserved (0x11f)
	{ .type = REG_VALUE },         // TCCR5A (0x120)
	{ .type = REG_VALUE },         // TCCR5B (0x121)
	{ .type = REG_VALUE },         // TCCR5C (0x122)
	{ .type = REG_RESERVED },      // Reserved (0x123)
	{ .type = REG_TIMER5_LOW },    // TCNT5L (0x124)
	{ .type = REG_TIMER5_HIGH },   // TCNT5H (0x125)
	{ .type = REG_VALUE },         // ICR5L (0x126)
	{ .type = REG_VALUE },         // ICR5H (0x127)
	{ .type = REG_TIMER5_LOW },    // OCR5AL (0x128)
	{ .type = REG_TIMER5_HIGH },   // OCR5AH (0x129)
	{ .type = REG_TIMER5_LOW },    // OCR5BL (0x12a)
	{ .type = REG_TIMER5_HIGH },   // OCR5BH (0x12b)
	{ .type = REG_TIMER5_LOW },    // OCR5CL (0x12c)
	{ .type = REG_TIMER5_HIGH },   // OCR5CH (0x12d)
	{ .type = REG_RESERVED },      // Reserved (0x12e)
	{ .type = REG_RESERVED },      // Reserved (0x12f)
	{ .type = REG_UNSUPPORTED },   // UCSR3A (0x130)
	{ .type = REG_UNSUPPORTED },   // UCSR3B (0x131)
	{ .type = REG_UNSUPPORTED },   // UCSR3C (0x132)
	{ .type = REG_RESERVED },      // Reserved (0x133)
	{ .type = REG_UNSUPPORTED },   // USART3 (0x134)
	{ .type = REG_UNSUPPORTED },   // USART3 (0x135)
	{ .type = REG_UNSUPPORTED },   // UDR3 (0x136)
	{ .type = REG_RESERVED },      // Reserved (0x137)
	{ .type = REG_RESERVED },      // Reserved (0x138)
	{ .type = REG_RESERVED },      // Reserved (0x139)
	{ .type = REG_RESERVED },      // Reserved (0x13a)
	{ .type = REG_RESERVED },      // Reserved (0x13b)
	{ .type = REG_RESERVED },      // Reserved (0x13c)
	{ .type = REG_RESERVED },      // Reserved (0x13d)
	{ .type = REG_RESERVED },      // Reserved (0x13e)
	{ .type = REG_RESERVED },      // Reserved (0x13f)
	{ .type = REG_RESERVED },      // Reserved (0x140)
	{ .type = REG_RESERVED },      // Reserved (0x141)
	{ .type = REG_RESERVED },      // Reserved (0x142)
	{ .type = REG_RESERVED },      // Reserved (0x143)
	{ .type = REG_RESERVED },      // Reserved (0x144)
	{ .type = REG_RESERVED },      // Reserved (0x145)
	{ .type = REG_RESERVED },      // Reserved (0x146)
	{ .type = REG_RESERVED },      // Reserved (0x147)
	{ .type = REG_RESERVED },      // Reserved (0x148)
	{ .type = REG_RESERVED },      // Reserved (0x149)
	{ .type = REG_RESERVED },      // Reserved (0x14a)
	{ .type = REG_RESERVED },      // Reserved (0x14b)
	{ .type = REG_RESERVED },      // Reserved (0x14c)
	{ .type = REG_RESERVED },      // Reserved (0x14d)
	{ .type = REG_RESERVED },      // Reserved (0x14e)
	{ .type = REG_RESERVED },      // Reserved (0x14f)
	{ .type = REG_RESERVED },      // Reserved (0x150)
	{ .type = REG_RESERVED },      // Reserved (0x151)
	{ .type = REG_RESERVED },      // Reserved (0x152)
	{ .type = REG_RESERVED },      // Reserved (0x153)
	{ .type = REG_RESERVED },      // Reserved (0x154)
	{ .type = REG_RESERVED },      // Reserved (0x155)
	{ .type = REG_RESERVED },      // Reserved (0x156)
	{ .type = REG_RESERVED },      // Reserved (0x157)
	{ .type = REG_RESERVED },      // Reserved (0x158)
	{ .type = REG_RESERVED },      // Reserved (0x159)
	{ .type = REG_RESERVED },      // Reserved (0x15a)
	{ .type = REG_RESERVED },      // Reserved (0x15b)
	{ .type = REG_RESERVED },      // Reserved (0x15c)
	{ .type = REG_RESERVED },      // Reserved (0x15d)
	{ .type = REG_RESERVED },      // Reserved (0x15e)
	{ .type = REG_RESERVED },      // Reserved (0x15f)
	{ .type = REG_RESERVED },      // Reserved (0x160)
	{ .type = REG_RESERVED },      // Reserved (0x161)
	{ .type = REG_RESERVED },      // Reserved (0x162)
	{ .type = REG_RESERVED },      // Reserved (0x163)
	{ .type = REG_RESERVED },      // Reserved (0x164)
	{ .type = REG_RESERVED },      // Reserved (0x165)
	{ .type = REG_RESERVED },      // Reserved (0x166)
	{ .type = REG_RESERVED },      // Reserved (0x167)
	{ .type = REG_RESERVED },      // Reserved (0x168)
	{ .type = REG_RESERVED },      // Reserved (0x169)
	{ .type = REG_RESERVED },      // Reserved (0x16a)
	{ .type = REG_RESERVED },      // Reserved (0x16b)
	{ .type = REG_RESERVED },      // Reserved (0x16c)
	{ .type = REG_RESERVED },      // Reserved (0x16d)
	{ .type = REG_RESERVED },      // Reserved (0x16e)
	{ .type = REG_RESERVED },      // Reserved (0x16f)
	{ .type = REG_RESERVED },      // Reserved (0x170)
	{ .type = REG_RESERVED },      // Reserved (0x171)
	{ .type = REG_RESERVED },      // Reserved (0x172)
	{ .type = REG_RESERVED },      // Reserved (0x173)
	{ .type = REG_RESERVED },      // Reserved (0x174)
	{ .type = REG_RESERVED },      // Reserved (0x175)
	{ .type = REG_RESERVED },      // Reserved (0x176)
	{ .type = REG_RESERVED },      // Reserved (0x177)
	{ .type = REG_RESERVED },      // Reserved (0x178)
	{ .type = REG_RESERVED },      // Reserved (0x179)
	{ .type = REG_RESERVED },      // Reserved (0x17a)
	{ .type = REG_RESERVED },      // Reserved (0x17b)
	{ .type = REG_RESERVED },      // Reserved (0x17c)
	{ .type = REG_RESERVED },      // Reserved (0x17d)
	{ .type = REG_RESERVED },      // Reserved (0x17e)
	{ .type = REG_RESERVED },      // Reserved (0x17f)
	{ .type = REG_RESERVED },      // Reserved (0x180)
	{ .type = REG_RESERVED },      // Reserved (0x181)
	{ .type = REG_RESERVED },      // Reserved (0x182)
	{ .type = REG_RESERVED },      // Reserved (0x183)
	{ .type = REG_RESERVED },      // Reserved (0x184)
	{ .type = REG_RESERVED },      // Reserved (0x185)
	{ .type = REG_RESERVED },      // Reserved (0x186)
	{ .type = REG_RESERVED },      // Reserved (0x187)
	{ .type = REG_RESERVED },      // Reserved (0x188)
	{ .type = REG_RESERVED },      // Reserved (0x189)
	{ .type = REG_RESERVED },      // Reserved (0x18a)
	{ .type = REG_RESERVED },      // Reserved (0x18b)
	{ .type = REG_RESERVED },      // Reserved (0x18c)
	{ .type = REG_RESERVED },      // Reserved (0x18d)
	{ .type = REG_RESERVED },      // Reserved (0x18e)
	{ .type = REG_RESERVED },      // Reserved (0x18f)
	{ .type = REG_RESERVED },      // Reserved (0x190)
	{ .type = REG_RESERVED },      // Reserved (0x191)
	{ .type = REG_RESERVED },      // Reserved (0x192)
	{ .type = REG_RESERVED },      // Reserved (0x193)
	{ .type = REG_RESERVED },      // Reserved (0x194)
	{ .type = REG_RESERVED },      // Reserved (0x195)
	{ .type = REG_RESERVED },      // Reserved (0x196)
	{ .type = REG_RESERVED },      // Reserved (0x197)
	{ .type = REG_RESERVED },      // Reserved (0x198)
	{ .type = REG_RESERVED },      // Reserved (0x199)
	{ .type = REG_RESERVED },      // Reserved (0x19a)
	{ .type = REG_RESERVED },      // Reserved (0x19b)
	{ .type = REG_RESERVED },      // Reserved (0x19c)
	{ .type = REG_RESERVED },      // Reserved (0x19d)
	{ .type = REG_RESERVED },      // Reserved (0x19e)
	{ .type = REG_RESERVED },      // Reserved (0x19f)
	{ .type = REG_RESERVED },      // Reserved (0x1a0)
	{ .type = REG_RESERVED },      // Reserved (0x1a1)
	{ .type = REG_RESERVED },      // Reserved (0x1a2)
	{ .type = REG_RESERVED },      // Reserved (0x1a3)
	{ .type = REG_RESERVED },      // Reserved (0x1a4)
	{ .type = REG_RESERVED },      // Reserved (0x1a5)
	{ .type = REG_RESERVED },      // Reserved (0x1a6)
	{ .type = REG_RESERVED },      // Reserved (0x1a7)
	{ .type = REG_RESERVED },      // Reserved (0x1a8)
	{ .type = REG_RESERVED },      // Reserved (0x1a9)
	{ .type = REG_RESERVED },      // Reserved (0x1aa)
	{ .type = REG_RESERVED },      // Reserved (0x1ab)
	{ .type = REG_RESERVED },      // Reserved (0x1ac)
	{ .type = REG_RESERVED },      // Reserved (0x1ad)
	{ .type = REG_RESERVED },      // Reserved (0x1ae)
	{ .type = REG_RESERVED },      // Reserved (0x1af)
	{ .type = REG_RESERVED },      // Reserved (0x1b0)
	{ .type = REG_RESERVED },      // Reserved (0x1b1)
	{ .type = REG_RESERVED },      // Reserved (0x1b2)
	{ .type = REG_RESERVED },      // Reserved (0x1b3)
	{ .type = REG_RESERVED },      // Reserved (0x1b4)
	{ .type = REG_RESERVED },      // Reserved (0x1b5)
	{ .type = REG_RESERVED },      // Reserved (0x1b6)
	{ .type = REG_RESERVED },      // Reserved (0x1b7)
	{ .type = REG_RESERVED },      // Reserved (0x1b8)
	{ .type = REG_RESERVED },      // Reserved (0x1b9)
	{ .type = REG_RESERVED },      // Reserved (0x1ba)
	{ .type = REG_RESERVED },      // Reserved (0x1bb)
	{ .type = REG_RESERVED },      // Reserved (0x1bc)
	{ .type = REG_RESERVED },      // Reserved (0x1bd)
	{ .type = REG_RESERVED },      // Reserved (0x1be)
	{ .type = REG_RESERVED },      // Reserved (0x1bf)
	{ .type = REG_RESERVED },      // Reserved (0x1c0)
	{ .type = REG_RESERVED },      // Reserved (0x1c1)
	{ .type = REG_RESERVED },      // Reserved (0x1c2)
	{ .type = REG_RESERVED },      // Reserved (0x1c3)
	{ .type = REG_RESERVED },      // Reserved (0x1c4)
	{ .type = REG_RESERVED },      // Reserved (0x1c5)
	{ .type = REG_RESERVED },      // Reserved (0x1c6)
	{ .type = REG_RESERVED },      // Reserved (0x1c7)
	{ .type = REG_RESERVED },      // Reserved (0x1c8)
	{ .type = REG_RESERVED },      // Reserved (0x1c9)
	{ .type = REG_RESERVED },      // Reserved (0x1ca)
	{ .type = REG_RESERVED },      // Reserved (0x1cb)
	{ .type = REG_RESERVED },      // Reserved (0x1cc)
	{ .type = REG_RESERVED },      // Reserved (0x1cd)
	{ .type = REG_RESERVED },      // Reserved (0x1ce)
	{ .type = REG_RESERVED },      // Reserved (0x1cf)
	{ .type = REG_RESERVED },      // Reserved (0x1d0)
	{ .type = REG_RESERVED },      // Reserved (0x1d1)
	{ .type = REG_RESERVED },      // Reserved (0x1d2)
	{ .type = REG_RESERVED },      // Reserved (0x1d3)
	{ .type = REG_RESERVED },      // Reserved (0x1d4)
	{ .type = REG_RESERVED },      // Reserved (0x1d5)
	{ .type = REG_RESERVED },      // Reserved (0x1d6)
	{ .type = REG_RESERVED },      // Reserved (0x1d7)
	{ .type = REG_RESERVED },      // Reserved (0x1d8)
	{ .type = REG_RESERVED },      // Reserved (0x1d9)
	{ .type = REG_RESERVED },      // Reserved (0x1da)
	{ .type = REG_RESERVED },      // Reserved (0x1db)
	{ .type = REG_RESERVED },      // Reserved (0x1dc)
	{ .type = REG_RESERVED },      // Reserved (0x1dd)
	{ .type = REG_RESERVED },      // Reserved (0x1de)
	{ .type = REG_RESERVED },      // Reserved (0x1df)
	{ .type = REG_RESERVED },      // Reserved (0x1e0)
	{ .type = REG_RESERVED },      // Reserved (0x1e1)
	{ .type = REG_RESERVED },      // Reserved (0x1e2)
	{ .type = REG_RESERVED },      // Reserved (0x1e3)
	{ .type = REG_RESERVED },      // Reserved (0x1e4)
	{ .type = REG_RESERVED },      // Reserved (0x1e5)
	{ .type = REG_RESERVED },      // Reserved (0x1e6)
	{ .type = REG_RESERVED },      // Reserved (0x1e7)
	{ .type = REG_RESERVED },      // Reserved (0x1e8)
	{ .type = REG_RESERVED },      // Reserved (0x1e9)
	{ .type = REG_RESERVED },      // Reserved (0x1ea)
	{ .type = REG_RESERVED },      // Reserved (0x1eb)
	{ .type = REG_RESERVED },      // Reserved (0x1ec)
	{ .type = REG_RESERVED },      // Reserved (0x1ed)
	{ .type = REG_RESERVED },      // Reserved (0x1ee)
	{ .type = REG_RESERVED },      // Reserved (0x1ef)
	{ .type = REG_RESERVED },      // Reserved (0x1f0)
	{ .type = REG_RESERVED },      // Reserved (0x1f1)
	{ .type = REG_RESERVED },      // Reserved (0x1f2)
	{ .type = REG_RESERVED },      // Reserved (0x1f3)
	{ .type = REG_RESERVED },      // Reserved (0x1f4)
	{ .type = REG_RESERVED },      // Reserved (0x1f5)
	{ .type = REG_RESERVED },      // Reserved (0x1f6)
	{ .type = REG_RESERVED },      // Reserved (0x1f7)
	{ .type = REG_RESERVED },      // Reserved (0x1f8)
	{ .type = REG_RESERVED },      // Reserved (0x1f9)
	{ .type = REG_RESERVED },      // Reserved (0x1fa)
	{ .type = REG_RESERVED },      // Reserved (0x1fb)
	{ .type = REG_RESERVED },      // Reserved (0x1fc)
	{ .type = REG_RESERVED },      // Reserved (0x1fd)
	{ .type = REG_RESERVED },      // Reserved (0x1fe)
	{ .type = REG_RESERVED }       // Reserved (0x1ff)
};

const struct avr_model MODEL_ATMEGA2560 = {
    .romsize        = 256*1024,
    .regsize        = 512,
    .ramsize        = 8*1024,
    .eepsize        = 4*1024,
    .ramstart       = 0x200,
    .romstart       = 0,
    .eepstart       = 0,
    .memend         = 0x2200,
    .pcsize         = 3,
    .eep_pgsize     = 16,
    .flash_pgsize   = 256,
    .flash_bootsize = 8192,
    .flash_nrwwsize = 8192,
    .io_offset      = 0x20,
    .reg_rampz      = 0x5b,
    .reg_eind       = 0x5c,
    .reg_stack      = 0x5d,
    .reg_status     = 0x5f,
    .reg_spmcsr     = 0x57,

    .regmap         = regmap
};
