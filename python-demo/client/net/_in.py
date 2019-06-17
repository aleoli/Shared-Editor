from threading import Thread

from socket import socket
from socket import AF_INET, SOCK_STREAM

import json

import config

class NetworkIn(Thread):
	def __init__(self, s, se):
		Thread.__init__(self)
		self.s = s
		self.se = se
		self.running = False

	# quando riceve dati dal socket li trasforma in un oggetto e li invia all'istanza di SharedEditor
	# NB: prende per vero che tutti i dati arrivino insieme in un solo pacchetto
	# NB: gestisce solo pacchetti che riguardano caratteri, se si deve fare una gestione di altre funzioanlità va complicato
	def run(self):
		self.running = True
		while self.running:
			data = self.s.recv(1024)
			if not data:
				break
			if self.se:
				tmp = json.loads(data)
				self.se.process(tmp)
