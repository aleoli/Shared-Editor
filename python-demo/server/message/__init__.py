from threading import Thread, Lock
from queue import Queue

import json

from server.event import Event
from message import Message

# dati gli eventi in input genera eventi in output
class MessageProcessor(Thread):
	def __init__(self, event_manager):
		Thread.__init__(self)
		self.q = Queue()		# coda degli eventi in entrata
		self.event_manager = event_manager

	def run(self):
		while True:
			ev = self.q.get()
			new_ev = self.process(ev)
			if new_ev:
				self.event_manager.add_event(new_ev)

	# aggiunge un evento in coda
	def add_event(self, ev):
		self.q.put(ev)

	# in base al tipo dell'evento decide cosa farne
	def process(self, event):
		msg = json.loads(event.msg)
		if msg['type'] == 'SYMBOL':
			ev = Event(event.client_id, msg['msg'])
			ev.file_id = event.file_id
		else:
			print("{} not implemented".format(msg['type']))
			ev = None
		return ev
