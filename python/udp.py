'''
Documentation, License etc.

@package udp
'''

from threading import Thread
import sys
import socket
import time
from struct import *

CMD_who = 0
CMD_listValues = 10
CMD_readValue = 20
CMD_writeValue = 30
CMD_descValue = 40

FORMAT_type_int8 = "c"
FORMAT_type_int16 = "h"
FORMAT_type_int32 = "i"
FORMAT_type_int64 = "q"
FORMAT_type_float = "f"
FORMAT_type_string = "c"

FORMAT_MSG = '>HHBBH'

DST_PORT = 6666

class protocol():

    def __init__(self):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM )
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        
        self.port = 3883
        self.dstIp = '127.0.0.1'
        
        self.sock.bind(('', self.port))
        
    def buildMsg(self, src, dst, cmd, ack, id, data):
      size = len(id) + len(data)
      msg = pack(FORMAT_MSG, src, dst, cmd, ack, size)
      msg += id
      msg += data
      return msg

    def sendFrame(self, dst, cmd, ack, id, data):
        msg = self.buildMsg(self.port, dst, cmd, ack, id, data)
        
        self.dstPort = dst
        self.sock.sendto(msg, (self.dstIp, dst))
        return 0
      
    def receiveFrame(self):  
	  msg,addr = self.sock.recvfrom(1024)
	  src, dst, cmd, ack, size = unpack(FORMAT_MSG, msg[:8])
	  return msg[8:10], msg[10:]
      
    def formatData(self, format, number, data):
	data = pack(">" + str(number) + format, data)
	return data

    def unformatData(self, format, number, data):
	data = unpack(">" + str(number) + format, data)
	if number == 1:
	  return data[0] # extract from tuple
	else:
	  return data
      
    def write(self, dst, id, format, number, data):
      self.sendFrame(dst, CMD_writeValue, 0, id, self.formatData(format, number, data))
      self.receiveFrame() # get ack
      return 0
      
    def read(self, dst, id, format, number):
      self.sendFrame(dst, CMD_readValue, 0, id, "")
      status, data = self.receiveFrame()
      data = self.unformatData(format, number, data)
      return data
