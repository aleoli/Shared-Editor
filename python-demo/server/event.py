import json

class Event():
	def __init__(self, client_id, msg):
		self.client_id = client_id
		try:
			data = json.loads(msg)
			if data['type'] == 'SYMBOL':
				self.file_id = data['file_id']
				self.token = data['token']
		except:
			data = msg
		self.msg = msg
