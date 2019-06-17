from threading import Thread

from socket import socket
from socket import AF_INET, SOCK_STREAM

import config

from client.net._in import NetworkIn
from client.net._out import NetworkOut

class Network(Thread):
	def __init__(self):
		Thread.__init__(self)
		self._in = None
		self._out = None
		self.se = None			# oggetto del tipo SharedEditor a cui notificare le modifiche

	# connette un socket al server e poi istanzia 2 thread, uno per l'ascolto e l'altro per l'invio
	def run(self):
		with socket(AF_INET, SOCK_STREAM) as s:
			s.connect((config.HOST, config.PORT))
			self._in = NetworkIn(s, self.se)
			self._out = NetworkOut(s)
			self._in.start()
			self._out.start()
			self._in.join()
			self._out.join()

	# fa da proxy per il thread di invio
	def send_event(self, msg):
		self._out.send_event(str(msg))
