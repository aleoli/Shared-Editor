from threading import Thread
from queue import Queue

from socket import socket
from socket import AF_INET, SOCK_STREAM

import config

class NetworkOut(Thread):
	def __init__(self, s):
		Thread.__init__(self)
		self.s = s
		self.q = Queue()
		self.running = False

	# legge gli eventi da una coda e li invia al server
	def run(self):
		self.running = True
		while self.running:
			text = self.q.get()
			self.s.sendall(text.encode())

	# aggiunge un evento alla coda
	def send_event(self, msg):
		self.q.put(msg)
