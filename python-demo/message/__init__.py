import json

from symbol import Symbol, SymbolId

class Message():
	def __init__(self):
		self.msg = ''
		self.type = 'GENERIC'

	# static
	def new_message(msg):
		m = Message()
		m.msg = msg
		return m

	def __str__(self):
		return json.dumps(self, default=lambda e: e.__dict__)


# messaggi che richiedono autenticazione
class LoggedMessage(Message):
	def __init__(self):
		Message.__init__(self)
		# leggi da qualche file di configurazione
		self.token = 'asdfghjkl'
		self.type = 'LOGGED'

	# static
	def new_message(msg):
		m = LoggedMessage()
		m.msg = msg
		return m

# messaggi per la gestione dei simboli
class SymbolMessage(LoggedMessage):
	def __init__(self):
		LoggedMessage.__init__(self)
		# leggi da qualche file di configurazione
		self.file_id = 1
		self.type = 'SYMBOL'

	# static
	def new_message(symbol, message_action, client_id):			# message action: INSERT | REMOVE
		m = SymbolMessage()
		msg = {}
		msg['symbol'] = symbol
		msg['message_action'] = message_action
		msg['client_id'] = client_id
		m.msg = msg
		return m


class SymbolMessageServer(Message):
	def __init__(self):
		Message.__init__(self)
		# leggi da qualche file di configurazione
		self.file_id = 1
		self.type = 'SYMBOL'

	# static
	def new_message(msg):
		m = SymbolMessageServer()
		m.msg = msg
		return m
