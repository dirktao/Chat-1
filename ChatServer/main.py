import sys
import ChatServer

if len(sys.argv) < 2:
	print "[program] [port]"
else:
	ChatServer.ChatServer(int(sys.argv[1]))
