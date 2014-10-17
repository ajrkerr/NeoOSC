require 'rubygems'
require 'osc-ruby'
require 'osc-ruby/em_server'

require './arduino'



####

def send_reply(message)
  client = OSC::Client.new(message.ip_address, message.ip_port)
  puts message.to_a
  message = OSC::Message.new(message.address, message.to_a)

  client.send(message)
end

####





@server = OSC::EMServer.new( 3333 )
@arduino = Arduino.new
@arduino.send_command("h", 200)

@server.add_method '*' do |message|
  STDOUT.puts message.inspect
end

@server.add_method '/hue' do |message|
  @arduino.send_command("h", message.to_a.first)
  send_reply(message)
end

@server.add_method '/brightness' do |message|
  @arduino.send_command("b", message.to_a.first)
  send_reply(message)
end

@server.add_method '/ping' do |message|
  # @client = OSC::Client.new('localhost', 3333)
end

@server.run
