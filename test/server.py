import socket

HOST = '192.168.70.99'
PORT = 9099

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind((HOST, PORT)) # allow this to be a server
server.listen(5) # listen for max of 5 connections

while True:
    comm_socket, address = server.accept()
    print(f"Connected to {address}")
    message = comm_socket.recv(1024).decode("utf-8")
    print(f"Message from client is: {message}")
    comm_socket.send(f"Got your message! Thank you!".encode("utf-8"))
    comm_socket.close()
    print(f"Connection with {address} ended!")
