import socket

PORT: int = 4200
IP: str = "127.0.0.1"

def main():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(((IP,PORT)))
    csv_path = "dado1.csv"

    s.sendall(f"1 {csv_path}".encode())
    

if __name__ == "__main__":
    main()