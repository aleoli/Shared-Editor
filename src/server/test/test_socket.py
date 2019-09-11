import socket
import time

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(("localhost", 1234))

_str = "ciao!"
b = bytes([0, 0, 0, 5])
msg1 = b + _str.encode()

_str = "wei"
b = bytes([0, 0, 0, 3])
msg2 = b + _str.encode()

s.send(msg1 + msg2)

tmp = s.recv(1024)
print(tmp)

time.sleep(3)
