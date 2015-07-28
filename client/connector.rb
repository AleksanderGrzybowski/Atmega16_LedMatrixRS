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
end

def off(row, col)
	write_byte (encode_row_col(row, col) & ~(1 << 6))
end

def bulk(arr)
	write_byte 0xff

	arr.each do |byte|
		write_byte byte
	end
end
