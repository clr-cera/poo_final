import socket
from time import sleep

PORT: int = 4200
IP: str = "127.0.0.1"
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(((IP,PORT)))

def test1(s: socket.socket):
    csv_path = "dado1.csv"

    s.sendall(f"1 {csv_path}".encode())

    output = listen_returnal(s)
    output = output.replace("\\n", "\n")
    print(output)
    
def test2(s: socket.socket):
    s.sendall("2".encode())
    
    output = listen_returnal(s)
    output = output.replace("\\n", "\n")
    print(output)


def listen_returnal(s: socket.socket) -> str:
    string: str = ""
    readLen: int

    while(True):
        try: 
            message = s.recv(4096)
            if message:
                try:
                    output: str = message.decode()
                    length = int(output.split()[0])


                    rest: str = output[len(output.split()[0])+1:]
                    string += rest

                    readLen = length - len(rest)

                except Exception as e:
                    print(e)


                if readLen <= 0:
                    return string
                
                while(readLen > 0):
                    message = s.recv(4096)

                    if message:
                        output: str = message.decode()
                        string += output

                        readLen -= 4096

                        if readLen <= 0:
                            return string



        except:
            pass



if __name__ == "__main__":
    test1(s)
    sleep(0.1)
    test2(s)
    sleep(0.1)