import random
import re
import select
import socket
import sys

class ChatServer:
	def __init__(self, port):
		self.active = True
		self.serverSock = socket.socket()
		self.serverSock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
		self.serverSock.setblocking(0)
		self.serverSock.bind(("", int(port)))
		self.serverSock.listen(5)
		self.sockList = dict()
		self.operatorList = list()
		self.banList = list()
		self.Update()
	
	def Update(self):
		while self.active:
			inputList = self.sockList.keys()
			inputList.append(self.serverSock)
			inputList.append(sys.stdin)
			inputSocks, [], [] = select.select(inputList, [], [])
			for inputSock in inputSocks:
				if inputSock == self.serverSock:
					self.NewConnection()
				elif inputSock == sys.stdin:
					self.HandleMessage(sys.stdin, sys.stdin.readline())
				else:
					self.Read(inputSock)
	
	def NewConnection(self):
		newSock, newAddr = self.serverSock.accept()
		newSock.setblocking(0)
		if newAddr[0] in self.banList:
			self.Send(newSock, "29")
			self.Send(newSock, "2")
			newSock.close()
		else:
			self.sockList[newSock] = [str(), socket.gethostbyaddr(newAddr[0])[0], str(random.randint(1000, 9999))]
			for userSock in self.sockList.keys():
				self.Send(userSock, "1 " + self.sockList[newSock][2] + "@" + self.sockList[newSock][1])
			if len(self.operatorList) == 0:
				self.operatorList.append(newSock)
				self.Send(newSock, "11 " + self.sockList[newSock][2])
	
	def Send(self, sock, message):
		try:
			sock.send(message + "\n")
			return True
		except socket.error:
			return False
	
	def Read(self, sock):
		try:
			readBuffer = sock.recv(4096)
			if readBuffer == "":
				raise socket.error
		except socket.error:
			self.HandleMessage(sock, "2")
			return None
		self.sockList[sock][0] += readBuffer
		if self.sockList[sock][0].find("\n") <> -1:
			message = self.sockList[sock][0][:self.sockList[sock][0].find("\n")]
			self.sockList[sock][0] = self.sockList[sock][0][self.sockList[sock][0].find("\n") + 1:]
			self.HandleMessage(sock, message)
	
	def SearchNick(self, nick):
		for userSock in self.sockList.keys():
			if nick == self.sockList[userSock][2]:
				return userSock
		return None
	
	def HandleMessage(self, sock, rawMessage):
		rawMessage = rawMessage.replace("\n", "")
		rawMessage = rawMessage.replace("\r", "")
		rawMessage = rawMessage.lstrip(None)
		if rawMessage.find(" ") == -1:
			action = rawMessage
			message = str()
		else:
			action, message = rawMessage.split(" ", 1)
		if sock == sys.stdin:
			self.HandleSysMessage(action, message)
		elif action == "1":
			pass
		elif action == "2":
			for userSock in self.sockList.keys():
				if userSock <> sock:
					self.Send(userSock, "2 [" + self.sockList[sock][2] + "@" + self.sockList[sock][1] + "] " + message)
			del self.sockList[sock]
			sock.close()
		elif action == "3":
			if message <> "":
				for userSock in self.sockList.keys():
					self.Send(userSock, "3 <" + ("@" if sock in self.operatorList else "") + self.sockList[sock][2] + "> " + message)
		elif action == "4":
			names = str("3")
			for name in self.sockList.keys():
				names += " [" + self.sockList[name][2] + "]"
			self.Send(sock, names)
		elif action == "5":
			if len(message) < 3 or len(message) > 15:
				self.Send(sock, "20 3 15") # Load from config
			elif re.match("^[A-Za-z0-9_-]*$", message) == None:
				self.Send(sock, "21")
			elif self.SearchNick(message) <> None:
				self.Send(sock, "22 " + message)
			else:
				for userSock in self.sockList.keys():
					self.Send(userSock, "5 " + self.sockList[sock][2] + " " + message)
				self.sockList[sock][2] = message
		elif action == "6":
			whois = self.SearchNick(message)
			if whois <> None:
				self.Send(sock, "6 " + self.sockList[whois][2] + "@" + self.sockList[whois][1])
			else:
				self.Send(sock, "23 " + message)
		elif action == "7":
			userSock = self.SearchNick(message[:message.find(" ")])
			if userSock <> None:
				self.Send(userSock, message[message.find(" ") + 1:])
			else:
				pass # send error code
		elif action == "10":
			operators = "10"
			for operator in self.operatorList:
				operators += " [" + self.sockList[operator][2] + "]"
			self.Send(sock, operators)
		elif action == "11":
			newOperator = self.SearchNick(message)
			if sock not in self.operatorList:
				self.Send(sock, "27")
			elif newOperator <> None:
				if newOperator not in self.operatorList:
					self.operatorList.append(newOperator)
					for userSock in self.sockList.keys():
						self.Send(userSock, "11 " + self.sockList[newOperator][2] + " " + self.sockList[sock][2])
				else:
					self.Send(sock, "26")
			else:
				self.Send(sock, "24 " + message)
		elif action == "12":
			delOperator = self.SearchNick(message)
			if sock not in self.operatorList:
				self.Send(sock, "27")
			elif delOperator <> None:
				if delOperator in self.operatorList:
					self.operatorList.remove(delOperator)
					for userSock in self.sockList.keys():
						self.Send(userSock, "12 " + self.sockList[delOperator][2] + " " + self.sockList[sock][2])
				else:
					self.Send(sock, "26")
			else:
				self.Send(sock, "24 " + message)
		elif action == "13":
			bans = "13"
			for ban in self.banList:
				bans += " [" + ban + "]"
			self.Send(sock, bans)
		elif action == "14":
			newBan = self.SearchNick(message)
			if sock not in self.operatorList:
				self.Send(sock, "27")
			elif newBan <> None:
				if self.sockList[newBan][1] not in self.banList:
					self.banList.append(self.sockList[newBan][1])
					for userSock in self.sockList.keys():
						self.Send(userSock, "14 " + self.sockList[newBan][2] + "@" + self.sockList[newBan][1] + " " + self.sockList[sock][2])
				else:
					self.Send(sock, "26")
			else:
				self.Send(sock, "25 " + message)
		elif action == "15": # Modify to ip
			delBan = self.SearchNick(message)
			if sock not in self.operatorList:
				self.Send(sock, "27")
			elif delBan <> None:
				if delBan in self.banList:
					del self.banList[delBan]
					for userSock in self.sockList.keys():
						self.Send(userSock, "15 " + self.sockList[delBan][2] + " " + self.sockList[sock][2])
				else:
					self.Send(sock, "26")
			else:
				self.Send(sock, "25 " + message)
		else:
			self.Send(sock, "9 " + action)
	
	def HandleSysMessage(self, action, message):
		if action == "2":
			if message <> "":
				message = " " + message
			self.Close(message)
	
	def Close(self, message):
		for userSock in self.sockList.keys():
			try:
				self.Send(userSock, "2" + message)
			except socket.error:
				pass
			userSock.close()
		self.serverSock.close()
		self.active = False
