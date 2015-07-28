#! /usr/bin/ruby
require 'rubygems'
require 'serialport'

$ser = SerialPort.new("/dev/ttyUSB0", 9600, 8, 1, SerialPort::NONE)


def write_byte(tosend)
	$ser.write tosend.chr
end

def encode_row_col(row, col)
	# TODO checks
	row &= 7
	col &= 7

	encoded = col
	encoded |= (row << 3)
	encoded &= ~(1 << 7)

	encoded
end

def on(row, col)
	write_byte (encode_row_col(row, col) | (1 << 6))
	nil
end

def off(row, col)
	write_byte (encode_row_col(row, col) & ~(1 << 6))
	nil
end

def bulk(arr)
	write_byte 0xff

	arr.each do |byte|
		write_byte byte
	end

	nil
end

def clear
	bulk [0x00]*8
	nil
end

def flood
	bulk [0xff]*8
	nil
end

def test_all
	clear
	7.downto(0) do |i|
		7.downto(0) do |j|
			on i, j
			sleep 0.1
			off i, j
		end
	end

	nil
end

