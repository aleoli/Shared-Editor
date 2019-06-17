class Symbol():
	def __init__(self, id, chr, pos):
		self.id = id		# tipo: SymbolId
		self.chr = chr		# tipo: char
		self.pos = pos		# tipo: vettore di interi

	def __str__(self):
		return self.chr

class SymbolId():
	def __init__(self, client_id, char_id):
		self.client_id = client_id		# tipo: int
		self.char_id = char_id			# tipo: int
