from threading import Thread
from typing import Any
import socket

from client import Client

class Server:
    threadsList: list[Thread] = []
    serverSocket: socket.socket

    def __init__(self, ip: str, port: int) -> None:
        self.serverSocket = self.CreateServerSocket(ip, port)
    
    def CreateServerSocket(ip: str, port: int) -> socket.socket:
        s: socket.socket = socket(socket.AF_INET, socket.SOCK_STREAM, 0)
        s.bind((ip, port))
        s.listen(50)
        s.setblocking(True)
        return s
    
    def __call__(self) -> Any:
        while(True):
            self.CheckConnections()
    
    def CheckConnections(self):
        try:
            c, addr = self.serverSocket.accept()
            c.setblocking(True)
            print(f"Incoming connection from {addr[0]}")
            client: Client = Client(c, addr[0])
            self.CreateThread(client)

            
        except:
            pass

    def CreateThread(self, client: Client):
        thread = Thread(target=client, args=[], daemon=True)
        thread.start()
        self.threadsList.append(thread)