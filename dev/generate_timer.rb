# Generate timer structs from an AVRA/Atmel assembly definition header. I've only
# tested it with the definitions included with AVRA however.
#
# Usage: ruby dev/generate_timer.rb ../../src/avra-1.4.2/includes/m1280def.inc

def parse_file(fname)
    defs = {}
    File.readlines(fname).each_with_index do |line, i|
        match = line.match(/.equ\s+([a-zA-Z0-9_]+)\s+=\s+([^\s]+)/)
        if match
            name, val = match[1], match[2]
            while defs[val]
                val = defs[val]
            end
            if val =~ /(-?0x[A-F0-9]+)|(-?\d+)/
                defs[name] = val
            else
                raise "invalid def on line #{i}: #{line}"
            end
        end
    end
    defs.transform_values { |val| val =~ /x/ ? val.to_i(16) : val.to_i }
end

def make_16_bit_alias!(defs, name)
    if defs["#{name}L"] && defs["#{name}"].nil?
        defs["#{name}"] = defs["#{name}L"]
    end
end

def make_aliases!(defs, num)
    make_16_bit_alias!(defs, "TCNT#{num}")
    make_16_bit_alias!(defs, "OCR#{num}A")
    make_16_bit_alias!(defs, "OCR#{num}B")
    make_16_bit_alias!(defs, "OCR#{num}C")
    make_16_bit_alias!(defs, "ICR#{num}")
end

def normalize_address(addr)
    if addr >= 0
        addr <= 0x3f ? addr + 0x20 : addr
    else
        addr
    end
end

def to16(x, size=2)
    if x >= 0
        "0x#{x.to_s(16).rjust(size, "0")}"
    else
        "0"
    end
end

def ato16(x, size=2)
    to16(normalize_address(x), size)
end

def bto16(x)
    to16(x ? 1 << x : -1)
end

WGM_TABLE = {
    "TIMER_STANDARD" => "{ WGM_NORMAL, WGM_PHASE_PWM_8BIT, WGM_PHASE_PWM_9BIT, WGM_PHASE_PWM_10BIT, WGM_CLEAR_ON_COMPARE_OCRA, WGM_FAST_PWM_8BIT, WGM_FAST_PWM_9BIT, WGM_FAST_PWM_10BIT, WGM_PHASE_FREQ_PWM_ICR, WGM_PHASE_FREQ_PWM_OCRA, WGM_PHASE_PWM_ICR, WGM_PHASE_PWM_OCRA, WGM_CLEAR_ON_COMPARE_ICR, WGM_RESERVED, WGM_FAST_PWM_ICR, WGM_FAST_PWM_OCRA }",
    "TIMER_REDUCED" => "{ WGM_NORMAL, WGM_PHASE_PWM_8BIT, WGM_CLEAR_ON_COMPARE_OCRA, WGM_FAST_PWM_8BIT, WGM_RESERVED, WGM_PHASE_PWM_OCRA, WGM_RESERVED, WGM_FAST_PWM_OCRA }"
}

puts "WARNING! Using ATmega640/1280/2560 specific OCnx pins!"
puts "if you are not generating for one of these MCUs, do"
puts "not use the generated OCnx values!"
def m640m1280m2560_ONLY_ocnx_pin(defs, n, x)
    pin_str = [
        { "A" => "PB7", "B" => "PG5" }, # OC0x
        { "A" => "PB5", "B" => "PB6", "C" => "PB7" }, # OC1x
        { "A" => "PB4", "B" => "PH6" }, # OC2x
        { "A" => "PE3", "B" => "PE4", "C" => "PE5" }, # OC3x
        { "A" => "PH3", "B" => "PH4", "C" => "PH5" }, # OC4x
        { "A" => "PL3", "B" => "PL4", "C" => "PL5" } # OC5x
    ][n][x]
    if pin_str
        match = pin_str.match(/P([A-L])(\d)/)
        [defs["PORT#{match[1]}"], match[2].to_i]
    else
        [-1, nil]
    end
end

def generate_timer(defs, num)
    make_aliases!(defs, num)
    resolution = defs["TCNT#{num}H"].nil? ? 8 : 16
    type = defs["WGM#{num}3"].nil? ? "TIMER_REDUCED" : "TIMER_STANDARD"
    comparators = defs["OCR#{num}B"].nil? ? 1 : defs["OCR#{num}C"].nil? ? 2 : 3

    puts <<END
{ // timer #{num}
    .type           = #{type},
    .resolution     = #{resolution},
    .comparators    = #{comparators},
    .wgm_table      = #{WGM_TABLE[type]},
    .clock_src_table = { CS_DISABLED, CS_1, CS_8, CS_64, CS_256, CS_1024, CS_FALLING, CS_RISING },
    .reg_tcnt       = #{ato16(defs["TCNT#{num}"])},
    .reg_ocra       = #{ato16(defs["OCR#{num}A"])},
    .reg_ocrb       = #{ato16(defs["OCR#{num}B"] || -1)},
    .reg_ocrc       = #{ato16(defs["OCR#{num}C"] || -1)},
    .reg_tccra      = #{ato16(defs["TCCR#{num}A"])},
    .reg_tccrb      = #{ato16(defs["TCCR#{num}B"])},
    .reg_tccrc      = #{ato16(defs["TCCR#{num}C"] || -1)},
    .reg_oca        = #{ato16(m640m1280m2560_ONLY_ocnx_pin(defs, num, "A")[0])},
    .reg_ocb        = #{ato16(m640m1280m2560_ONLY_ocnx_pin(defs, num, "B")[0])},
    .reg_occ        = #{ato16(m640m1280m2560_ONLY_ocnx_pin(defs, num, "C")[0])},
    .msk_oca        = #{bto16(m640m1280m2560_ONLY_ocnx_pin(defs, num, "A")[1])},
    .msk_ocb        = #{bto16(m640m1280m2560_ONLY_ocnx_pin(defs, num, "B")[1])},
    .msk_occ        = #{bto16(m640m1280m2560_ONLY_ocnx_pin(defs, num, "C")[1])},
    .reg_foc        = #{ato16(defs["TCCR#{num}C"] || defs["TCCR#{num}B"])},
    .msk_foca       = #{bto16(defs["FOC#{num}A"])},
    .msk_focb       = #{bto16(defs["FOC#{num}B"])},
    .msk_focc       = #{bto16(defs["FOC#{num}C"])},
    .reg_icr        = #{ato16(defs["ICR#{num}"] || -1)},
    .reg_timsk      = #{ato16(defs["TIMSK#{num}"])},
    .msk_ociea      = #{bto16(defs["OCIE#{num}A"])},
    .msk_ocieb      = #{bto16(defs["OCIE#{num}B"])},
    .msk_ociec      = #{bto16(defs["OCIE#{num}C"])},
    .msk_toie       = #{bto16(defs["TOIE#{num}"])},
    .reg_tifr       = #{ato16(defs["TIFR#{num}"])},
    .msk_ocfa       = #{bto16(defs["OCF#{num}A"])},
    .msk_ocfb       = #{bto16(defs["OCF#{num}B"])},
    .msk_ocfc       = #{bto16(defs["OCF#{num}C"])},
    .msk_tovf       = #{bto16(defs["TOV#{num}"])},
    .reg_external   = 0,
    .msk_external   = 0,
    .vec_compa      = #{to16(2*defs["OC#{num}Aaddr"], 4)},
    .vec_compb      = #{to16(2*(defs["OC#{num}Baddr"] || -1), 4)},
    .vec_compc      = #{to16(2*(defs["OC#{num}Caddr"] || -1), 4)},
    .vec_ovf        = #{to16(2*defs["OVF#{num}addr"], 4)}
},
END
end

defs = parse_file(ARGV[0])
10.times do |i|
    generate_timer(defs, i) if defs["TCNT#{i}"] || defs["TCNT#{i}L"]
end
