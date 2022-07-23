.include "m1280def.inc"
.include "base.inc"

test_value:
  ldi r16, 1
  sts status, r16
  ldi r16, 42
  out SPL, r16
  in r17, SPL
  cpse r16, r17
  jmp fail

test_reserved:
  ldi r16, 2
  sts status, r16
  ldi r16, 42
  sts 0x1ff, r16
  lds r17, 0x1ff
  cpi r17, 0xff
  breq test_clear_on_set
  jmp fail

test_clear_on_set:
  ldi r16, 3
  sts status, r16
  ldi r16, 42
  out TIFR1, r16
  in r17, TIFR1
  cpi r17, 0
  brne fail

test_16bit_atomic:
  ldi r16, 4
  sts status, r16
  ldi r16, low(1234)
  ldi r17, high(1234)
  sts OCR1AH, r17
  sts OCR1AL, r16
  lds r18, OCR1AL
  lds r19, OCR1AH
  cp r16, r18
  cpc r17, r19
  brne fail
  sts OCR1AH, r1
  sts OCR1AL, r1
  sts OCR1AL, r16
  sts OCR1AH, r17
  lds r18, OCR1AL
  lds r19, OCR1AH
  cp r16, r18
  cpc r1, r19
  brne fail
  sts OCR1AL, r16
  sts OCR1AH, r17
  sts OCR1AL, r16
  lds r18, OCR1AL
  lds r19, OCR1AH
  cp r16, r18
  cpc r17, r19
  brne fail
  ldi r20, 42
  sts OCR3AH, r20
  lds r19, OCR1AH
  lds r18, OCR1AL
  cp r16, r18
  cpc r20, r19
  brne fail

  sts status, r1
pass:
  rjmp pass

fail:
  rjmp fail