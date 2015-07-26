#! /usr/bin/ruby
require 'rubygems'
require 'serialport'

ser = SerialPort.new("/dev/ttyUSB0", 9600, 8, 1, SerialPort::NONE)

1.upto(100) do
	tosend = ("ab".encode! "ASCII")
	tosend += ( (1..10).map {|t| rand(0..255).chr}).join
	ser.write tosend
	sleep 1
end