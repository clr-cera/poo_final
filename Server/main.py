PORT: int = 4200
IP: str = "127.0.0.1"

from server import Server
import subprocess 

def main():
    print("Server will be initiated")
    Server(IP, PORT)()

def test():
    process = subprocess.Popen(["./Arquivos/main"],stdin=subprocess.PIPE)
    csv_path = "Jogador.csv"
    process.stdin.write(f"1 {csv_path} test.bin".encode())


if __name__ == "__main__":
    main()