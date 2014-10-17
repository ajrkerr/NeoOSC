require "serialport"

class Arduino
  PORT_STRING = "/dev/tty.usbmodem14141"  #may be different for you
  BAUD_RATE = 9600
  DATA_BITS = 8
  STOP_BITS = 1
  PARITY = SerialPort::NONE

  def initialize
    @serial_port = SerialPort.new(PORT_STRING, BAUD_RATE, DATA_BITS, STOP_BITS, PARITY)
  end

  def send_command(command, data)
    message = command + data.to_s + ";"
    puts "Sending::" + message
    send_message(message)
  end

  def send_message(message)
    @serial_port.write(message)
  end
end
