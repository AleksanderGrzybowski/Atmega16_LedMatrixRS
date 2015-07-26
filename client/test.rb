#! /usr/bin/ruby
require 'rubygems'
require 'serialport'

ser = SerialPort.new("/dev/ttyUSB0", 9600, 8, 1, SerialPort::NONE)

ser.write "ab12345678"