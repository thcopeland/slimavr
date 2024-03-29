# Register data is long, extremely tedious, and error-prone to write manually.
# Luckily, the Atmel datasheets provide a convenient table containing the
# regmap information. This is a small Ruby utility to parse that table data
# after you've copy-and-pasted it into a file.

def check_args args
    if args.length < 1
        puts "Usage: ruby generate_regmap.rb FILE"
        false
    elsif not File.exists? args.first
        puts "Unable to read file #{args.first}"
        false
    else
        true
    end
end

def get_line_addr line
    match = line.match(/\((0x[a-fA-F0-9]+)\)/)
    match[1].to_i(16) unless match.nil?
end

def transform_file fname
    lines = File.readlines fname
    data = []

    0x00.upto(0x20) do |addr|
        data[addr] = [ "REG_VALUE", "r#{addr}"]
    end

    lines.each_with_index do |line, idx|
        addr = get_line_addr(line)

        if addr.nil? && line =~ /\.\.\./
            start_addr, end_addr = [get_line_addr(lines[idx-1]), get_line_addr(lines[idx+1])].minmax

            (start_addr+1).upto end_addr do |addr|
                data[addr] = [ "REG_RESERVED", "Reserved"]
            end
        elsif addr.nil?
            raise "unable to parse line #{idx+1}"
        else
            data[addr] = (
                try_match_value line or
                try_match_reserved line or
                try_match_unsupported line or
                try_match_timer_ctrl line or
                try_match_timer_high line or
                try_match_timer_low line or
                try_portx_in line or
                try_match_clear_on_set line or
                try_match_eep_control line or
                try_match_spm_control line or
                raise "unable to parse line #{idx+1}: #{line}"
            )
        end
    end

    ir = data.map.with_index do |item, idx|
        if item.nil?
            ["***missing entry at 0x#{idx.to_s(16)}... check your table!***", ""]
        else
            type, name = item
            ["\t{ .type = #{type} }#{idx < data.length-1 ? "," : ""}", "// #{name} (0x#{idx.to_s(16)})"]
        end
    end

    maxlength = ir.map { |code, cmmt| code.length }.max
    goodlength = (maxlength/4.0).ceil * 4

    puts "static struct avr_register regmap[] = {"
    ir.each do |code, cmmt|
        print code.ljust(goodlength)
        puts cmmt
    end
    puts "};"
end

def def_matcher name, type_src, regexp
    define_method name do |line|
        matches = line.match regexp
        type = type_src.dup

        if matches
            first, *rest = matches.to_a.compact

            rest.each_with_index do |match, idx|
                first.gsub! "$#{idx}", (match or "")
                type.gsub! "$#{idx}", (match or "")
            end

            [type, first]
        end
    end
end

def_matcher :try_match_value, "REG_VALUE", /PORT[A-Z]|DDR[A-Z]|SREG|SPH|SPL|EINT|RAMPZ|GPIOR\d+|GTCCR|ICR\d+[HL]?|TCNT\d+ |TIMSK\d+|EIND|RAMPZ|EEAR[HL]?|EEDR/
def_matcher :try_match_reserved, "REG_RESERVED", /Reserved/
def_matcher :try_match_unsupported, "REG_UNSUPPORTED", /UDR\d+|USART\d+|UCSR\d+[A-C]|TWAMR|TW[A-D]R|TWSR|ASSR|DIDR\d+|ADMUX|ADCSR[A-C]|ADC[HL]|XMCR[A-C]|PCMSK\d+|EICR[A-C]|PCICR|OSCCAL|CLKPR|WDTCSR|MCUCR|MCUSR|SMCR|OCDR|ADSR|ACSR|SP[DCS]R|EIMSK|EIFR|PCIFR/
def_matcher :try_match_timer_high, "REG_TIMER$0_HIGH", /OCR(\d+)[A-C]H|TCNT(\d+)H/
def_matcher :try_match_timer_low, "REG_TIMER$0_LOW", /OCR(\d+)[A-C]L|OCR(\d+)[A-C] |TCNT(\d+)L/
def_matcher :try_match_timer_ctrl, "REG_TIMER$0_CTRL", /TCCR(\d+)[A-C]/
def_matcher :try_portx_in, "REG_PORT$0_IN", /PIN([A-Z])/
def_matcher :try_match_clear_on_set, "REG_CLEAR_ON_SET", /TIFR\d+/
def_matcher :try_match_eep_control, "REG_EEP_CONTROL", /EECR/
def_matcher :try_match_spm_control, "REG_SPM_CONTROL", /SPMCSR/

check_args(ARGV) and transform_file(ARGV.first)
