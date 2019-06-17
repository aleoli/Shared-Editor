from socket import socket
from socket import AF_INET, SOCK_STREAM
from threading import Thread

import config

from server.net.client_connection import ClientConnection

# si occupa di aspettare che nuovi client si colleghino e ne istanzia una ClientConnection
class Listener(Thread):
	def __init__(self, event_manager, message_processor):
		Thread.__init__(self)
		self.event_manager = event_manager
		self.message_processor = message_processor

	def run(self):
		with socket(AF_INET, SOCK_STREAM) as s:
			s.bind((config.HOST, config.PORT))
			s.listen()
			id = 0
			while True:
				conn, addr = s.accept()
				cc = ClientConnection(id, conn, addr, self.event_manager, self.message_processor)
				id += 1
				cc.start()
