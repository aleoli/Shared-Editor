from tkinter import *

import config

from client.net import Network
from message import SymbolMessage
from client import SharedEditor

# non fa niente, era solo per vedere il testo come usciva
def saveas():
	global se
	t = se.get("1.0", "end-1c")
	print(t)

# viene schicciato un tasto sulla tastiera
def key(event):
	global se
	ch = event.char
	# considero solo i tasti a-z A-Z e spazio
	if (not (ch >= 'a' and ch <= 'z')) and (not (ch >= 'A' and ch <= 'Z')) and (not ch == ' '):
		return
	#print("pressed", ch, text.index(INSERT))
	se.localInsert(int(se.index(INSERT).split(".")[1]), ch)

# si è insrito un backspace
def backspace(event):
	global se
	index = int(se.index(INSERT).split(".")[1])
	if index > 0:
		se.localRemove(index-1)

net = Network()
net.daemon = True		# detached

# finestra grafica
root = Tk()

# manager per la gestione dell'editor condiviso (estende un campo di testo)
se = SharedEditor(root, net)
se.grid()

se.bind("<Key>", key)
se.bind("<BackSpace>", backspace)

net.se = se

button = Button(root, text="Save", command=saveas)
button.grid()

net.start()
root.mainloop()
