from server.event_manager import EventManager
from server.net.listener import Listener
from server.message import MessageProcessor

event_manager = EventManager()
event_manager.start()

message_processor = MessageProcessor(event_manager)
message_processor.start()

listener = Listener(event_manager, message_processor)
listener.start()


listener.join()
message_processor.join()
event_manager.join()
