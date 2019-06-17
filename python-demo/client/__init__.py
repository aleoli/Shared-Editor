from tkinter import *

from symbol import Symbol, SymbolId
from message import SymbolMessage

# estende il campo di testo, riceve come parametro la finestra parent e l'oggetto 'net' che si occupa della comunicazione col server
class SharedEditor(Text):
	def __init__(self, master, net):
		Text.__init__(self, master)
		self.site_id = 1		# lo deve ottenere dal server durante la connessione
		self.symbols = []
		self.counter = 0		# il contatore dei simboli inseriti da questo site
		self.net = net

	def print_symbols(self):
		_str = ''
		for s in self.symbols:
			print(s.pos)
			_str += str(s)
		print(_str)

	def localInsert(self, index, value):
		si = SymbolId(self.site_id, self.counter)
		print("Index: {}\tCounter: {}".format(index, self.counter))
		if index == 0:
			# sta all'inizio
			self.localInsert_begin(si, index, value)
		elif index == len(self.symbols):
			# sta alla fine
			self.localInsert_end(si, index, value)
		elif index > 0 and index < len(self.symbols):
			# sta nel mezzo
			self.localInsert_middle(si, index, value)
		else:
			raise Exception('il carattere inserito è fuori dal range')
		self.counter += 1
		self.print_symbols()

	def localRemove(self, index):
		if index < 0 or index >= len(self.symbols):
			raise Exception('il carattere da rimuovere è fuori dal range')
		s = self.symbols[index]
		self.send_to_server(s, 'REMOVE')
		del self.symbols[index]
		self.print_symbols()

	# processa un meggaggio di tipo SymbolMessage
	def process(self, sm):
		print(sm)
		if sm['message_action'] == 'INSERT':
			# insert
			self.insert_from_message(sm)
		elif sm['message_action'] == 'REMOVE':
			# remove
			self.remove_from_message(sm)
		self.print_symbols()

	# private
	def localInsert_begin(self, si, index, value):
		pos = []
		if not len(self.symbols) == 0:
			pos = self.symbols[0].pos.copy()
			pos[-1] -= 1
		pos.append(1)
		s = Symbol(si, value, pos)
		self.symbols.insert(index, s)
		self.send_to_server(s, 'INSERT')

	# private
	def localInsert_end(self, si, index, value):
		pos = self.symbols[-1].pos.copy()
		pos[-1] += 1
		s = Symbol(si, value, pos)
		self.symbols.append(s)
		self.send_to_server(s, 'INSERT')

	# private
	def localInsert_middle(self, si, index, value):
		pos = self.symbols[index-1].pos.copy()
		pos_next = self.symbols[index].pos.copy()
		if(len(pos) > len(pos_next)):
			# sto già inserendo nel mezzo
			pos[-1] += 1
		else:
			# devo inserirlo nel mezzo
			pos = pos_next
			pos[-1] -= 1
			pos.append(1)
		s = Symbol(si, value, pos)
		self.symbols.insert(index, s)
		self.send_to_server(s, 'INSERT')

	# private
	def send_to_server(self, s, action):
		m = SymbolMessage.new_message(s, action, self.site_id)
		self.net.send_event(m)

	# private
	def insert_from_message(self, sm):
		s = Symbol(sm['symbol']['id'], sm['symbol']['chr'], sm['symbol']['pos'])
		if len(self.symbols) == 0:
			self.symbols.append(s)
			self.insert_in_view(s, 0)
			return
		m_pos = sm['symbol']['pos'].copy()
		done = False
		for index in range(len(self.symbols)):
			sym = self.symbols[index]
			s_pos = sym.pos.copy()
			for a in range(len(m_pos)):
				if a >= len(s_pos) or m_pos[a] > s_pos[a]:
					# va dopo (esempio 1.1 > 1     ||      2 > 1)
					break
				if m_pos[a] < s_pos[a]:
					# va prima (sempio 1 < 2)
					self.symbols.insert(index, s)
					self.insert_in_view(s, index)
					done = True
					break
			if done:
				break
		if not done:
			# va al fondo
			self.symbols.append(s)
			self.insert_in_view(s, len(self.symbols))

	# private
	def remove_from_message(self, sm):
		if len(self.symbols) == 0:
			return
		m_pos = sm['symbol']['pos'].copy()
		for index in range(len(self.symbols)):
			sym = self.symbols[index]
			s_pos = sym.pos.copy()
			if not len(m_pos) == len(s_pos):
				continue
			found = True
			for a in range(len(m_pos)):
				if not s_pos[a] == m_pos[a]:
					found = False
					break
			if found:
				del self.symbols[index]
				self.remove_in_view(index)
				break

	# private
	def insert_in_view(self, s, index):
		pos = "{}.{}".format(1, index)
		self.insert(pos, str(s))

	# private
	def remove_in_view(self, index):
		pos = "{}.{}".format(1, index)
		self.delete(pos)
