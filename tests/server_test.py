import socket
import json

HOST = '127.0.0.1'  # localhost
PORT = 4312        # port

try:
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind((HOST, PORT))

    while True:
        s.listen()
        conn, addr = s.accept()

        with conn:
            print('Connected by', addr)
            while True:
                data = conn.recv(1024)
                if not data:
                    print("Client disconnected. Connection {} lost".format(addr))
                    break

                try:
                    d = json.loads(data)
                    print(d['cmd'])
                    conn.sendall(str.encode(d['cmd']))
                except json.decoder.JSONDecodeError:
                    print("JSON decoding error for data: ", data)

except socket.error as err:
    print("Socket error: ", err)
    print(type(err))

