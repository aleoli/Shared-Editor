from threading import Thread, Lock
from queue import Queue

import json

class EventManager(Thread):
	def __init__(self):
		Thread.__init__(self)
		self.clients_lock = Lock()
		self.clients = []
		self.events = Queue()			# thread safe

	# legge un evento dalla coda e lo invia a tutti i client connessi
	# NB: gli vengono recapitati solo i messagi che devono essere inviati agli altri client collegati sullo stesso documento
	# NB: i client non vengono mai disconnessi
	def run(self):
		while True:
			msg = self.events.get()
			print("manager:\t{}".format(msg.msg))
			self.clients_lock.acquire()
			print("{} clients connected".format(len(self.clients)))
			for cc in self.clients:
				if (not cc.id == msg.client_id):# and msg.file_id and msg.file_id == cc.file_id:
					cc.send_event(json.dumps(msg.msg, default=lambda e: e.__dict__))
			self.clients_lock.release()

	# aggiunge un evento alla coda
	def add_event(self, m):
		self.events.put(m)

	# aggiunge un client - thread safe
	def add_client(self, cc):
		self.clients_lock.acquire()
		self.clients.append(cc)
		self.clients_lock.release()

	def remove_client(self, cc):
		print("remove client")
