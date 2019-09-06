import socket
import time

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(("localhost", 1234))

s.send(bytes([0, 0, 0, 5]))

s.send("ciao!".encode())

tmp = s.recv(1024)
print(tmp)

time.sleep(3)
