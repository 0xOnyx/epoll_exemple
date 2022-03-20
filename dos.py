# Import socket module
import socket
import threading




def dos():
    # local host IP '127.0.0.1'
    host = '127.0.0.1'

    # Define the port on which you want to connect
    port = 5454

    s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)

    # connect to server on local computer
    s.connect((host,port))

    # message you send to server
    message = "DOSS"
    while True:
        # message sent to server
        s.send(message.encode('ascii'))

        # message received from server
        data = s.recv(1024)

        # print the received message
        # here it would be a reverse of sent message
        print('Received from the server :',str(data.decode('ascii')))

        # ask the client whether he wants to continue
        ans = input('\nDo you want to continue(y/n) :')
        # close the connection
        break
    s.close()

y = 0
thread = []
for i in range(20000):
    thread.append(threading.Thread(target=dos,))
for i in thread:
    y = y + 1
    print(y)
    i.start()
