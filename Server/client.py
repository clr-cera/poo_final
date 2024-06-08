from socket import socket
from typing import Any
from time import sleep
import subprocess 

class Client:
    sock: socket
    addr: str
    file_name: str
    process: subprocess.Popen
    identifier: int

    def __init__(self, sock: socket, addr: str, identifier: int) -> None:
        self.sock = sock
        self.addr = addr
        self.identifier = identifier
        self.file_name = addr + f".{self.identifier}"+ ".bin"
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

            
            if not message:
                print(f"Closed connection with {self.addr}")
                self.sock.close()
                exit()
            

        except:
            pass
    
    def ParseCommand(self, command: str):
        comList: list[str] = command.split()
        number: str = comList[0]
        print(number)
        arguments: list[str] = comList[1:]

        self.process = subprocess.Popen(["./Arquivos/main"],stdin=subprocess.PIPE,stdout=subprocess.PIPE)


        match number:
            case '1':
                print(f"Loading csv for {self.addr}.{self.identifier}")
                self.load_csv(arguments)

            case '2':
                print(f"Searching all registers for {self.addr}.{self.identifier}") 
                self.search_all()

            case '3':
                print(f"Searching registers with filter for {self.addr}.{self.identifier}")
                self.search_filter(arguments)
            
            case '5':
                print(f"Removing registers with filter for {self.addr}.{self.identifier}")
                self.remove_filter(arguments)

            case '6':
                print(f"Inserting register for {self.addr}.{self.identifier}")
                self.insert_register(arguments)

        out, _ = self.process.communicate()
        output = str(out)
        output = output[2:len(output)-3]

        print(output)

        try:
            self.send_back(output)
        except Exception as e:
            print(e)

        self.process.kill()
         


    def load_csv(self, arguments: list[str]):
        if len(arguments) == 0:
            return
        csv_path = arguments[0]
        self.process.stdin.write(f"1 {csv_path} {self.file_name}\n".encode()) 

    def search_all(self):
        self.process.stdin.write(f"2 {self.file_name}\n".encode())

    def search_filter(self, arguments: list[str]):
        self.process.stdin.write(f"3 {self.file_name} 1\n".encode())
        self.process.stdin.write((' '.join(arguments) + "\n").encode())

    def remove_filter(self, arguments: list[str]):
        self.process.stdin.write(f"5 {self.file_name} {f'{self.addr}.{self.identifier}.index.bin'} 1\n".encode())
        self.process.stdin.write((' '.join(arguments) + "\n").encode())

    def insert_register(self, arguments: list[str]):
        self.process.stdin.write(f"6 {self.file_name} {f'{self.addr}.{self.identifier}.index.bin'} 1\n".encode())
        self.process.stdin.write((' '.join(arguments) + "\n").encode())

    def send_back(self, output: str):
        output = f"{len(output)} " + output
        self.sock.sendall(output.encode())