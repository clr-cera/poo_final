from socket import socket
from typing import Any
from time import sleep
import subprocess 

class Client:
    sock: socket
    addr: str
    file_name: str
    process: subprocess.Popen

    def __init__(self, sock: socket, addr: str) -> None:
        self.sock = sock
        self.addr = addr
        self.file_name = addr + ".bin"
        self.process = subprocess.Popen(["./Arquivos/main"],stdin=subprocess.PIPE)
        pass

    def __call__(self) -> Any:
        while(True):
            sleep(0.02)
            self.ListenCommands()

    def ListenCommands(self):
        try: 
            message = self.sock.recv(4096)
            if message:
                command: str = message.decode()
                self.ParseCommand(command)

        except:
            pass
    
    def ParseCommand(self, command: str):
        comList: list[str] = command.split()
        number: str = comList[0]
        arguments: list[str] = comList[1:]

        match number:
            case '1':
                self.load_csv(arguments)


    def load_csv(self, arguments: list[str]):
        if len(arguments) == 0:
            return
        csv_path = arguments[0]
        self.process.stdin.write(f"1 {csv_path} {self.file_name}".encode())
        
