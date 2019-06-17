from socket import socket
from socket import AF_INET, SOCK_STREAM
from threading import Thread

import config

from server.event import Event

# thread per la gestione della connessione con un singolo client
class ClientConnection(Thread):
	def __init__(self, id, conn, addr, event_manager, message_processor):
		Thread.__init__(self)
		self.id = id
		self.token = 'asdfghjkl'		# va generato casualemnte e inviato al client che lo userà come autenticazione
		self.file_id = None				# file attualmente in modifica, ovviamente all'inizio è null
		self.daemon = True          # detached
		self.conn = conn
		self.addr = addr
		self.event_manager = event_manager
		self.event_manager.add_client(self)
		self.message_processor = message_processor

	def __del__(self):
		self.event_manager.remove_client(self)

	def run(self):
		with self.conn:
			while True:
				data = self.conn.recv(1024)
				if not data:
					break
				# per semplicità faccio che mi è già arrivato tutto
				self.file_id = 1
				ev = Event(self.id, data)
				if ev.token and not ev.token == self.token:
					raise Exception('token non valido')
				if not ev.file_id == self.file_id:
					raise Exception('file id non corretto')
				self.message_processor.add_event(ev)

	def send_event(self, str):
		try:
			self.conn.sendall(str.encode())
		except Exception as e:
			print(e)
			print("non ho potuto inviare il messaggio")
