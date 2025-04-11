import socket
import random

SERVER_IP = '127.0.0.1'
SERVER_PORT = 55555

class Message:

    def __init__(self):
        self.command = ''
        self.threadsNumber = 0
        self.matrixSize = 0
        self.processTime=0
        self.data = b''

    def setCommand(self, command):
        self.command = command

    def setThreadsNumber(self, threadsNumber):
        self.threadsNumber = threadsNumber

    def setMatrixSize(self, matrixSize):
        self.matrixSize = matrixSize

    def processTime(self, processTime):
        self.processTime = processTime

    def setMatrix(self):
        for row in  range(int(self.matrixSize)):
            for number in range(int(self.matrixSize)):
                self.data += random.randint(0, 100).to_bytes(1, byteorder='big')

    def print(self):
        print(f"Command: {self.command}")
        print(f"Threads Number: {self.threadsNumber}")
        print(f"Matrix Size: {self.matrixSize}")
        print(f"Process Time: {self.processTime}")
        print("Matrix:")

        row = []
        i = 0
        for byte in self.data:
            if i % self.matrixSize == 0 and i != 0:  
                print(row)
                row = [] 
            row.append(byte)
            i += 1
        if row:
            print(row)

    def decode(self, buffer):
        ptr = 0

        len_threads = int.from_bytes(buffer[ptr:ptr + 4], byteorder='big')
        ptr += 4
        self.threadsNumber = int(buffer[ptr:ptr + len_threads].decode())
        ptr += len_threads

        len_matrixSize = int.from_bytes(buffer[ptr:ptr + 4], byteorder='big')
        ptr += 4
        self.matrixSize = int(buffer[ptr:ptr + len_matrixSize].decode())
        ptr += len_matrixSize

        len_processTime = int.from_bytes(buffer[ptr:ptr + 4], byteorder='big')
        ptr += 4
        self.processTime = float(buffer[ptr:ptr + len_processTime].decode())
        ptr += len_processTime

        len_matrix = int.from_bytes(buffer[ptr:ptr + 4], byteorder='big')
        ptr += 4
        self.data = buffer[ptr:ptr + len_matrix]
        ptr += len_matrix

    def encode(self):
    
        if(self.command =='D'):
            threads_bytes = str(self.threadsNumber).encode()
            matrixSize_bytes = str(self.matrixSize).encode()
            processTime_bytes = str(self.processTime).encode()

            length_bytes_threadsNumber = len(threads_bytes).to_bytes(4, byteorder='big')
            length_bytes_matrixSize = len(matrixSize_bytes).to_bytes(4, byteorder='big')
            length_bytes_processTime = len(processTime_bytes).to_bytes(4, byteorder='big')
            length_bytes_matrix = len(self.data).to_bytes(4, byteorder='big')

            body = (
                length_bytes_threadsNumber + threads_bytes +
                length_bytes_matrixSize + matrixSize_bytes +
                length_bytes_processTime + processTime_bytes +
                length_bytes_matrix + self.data
            )

            total_size_bytes = len(body).to_bytes(4, byteorder='big')
            return self.command.encode() + total_size_bytes + body
        else:
            return self.command.encode()

def main():

    try:
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client_socket.connect((SERVER_IP, SERVER_PORT))
        print("Connected to the server!")

        while True:

            command = input("Enter command: R to start, D to send data, B to start processing, S to check status, G to get answer, Q to quit: ")

            if(command == 'D'):

                while True:
                    threadsNumber = int(input("Enter threads number (from 1 to 8): "))
                    if 1 <= threadsNumber <= 8:
                        break
                    print("Threads number must be between 1 and 8.")

                while True:
                    matrixSize = int(input("Enter matrix size (from 3 to 20): "))
                    if 3 <= matrixSize <= 20:
                        break
                    print("Matrix size must be between 3 and 20.")

                msg = Message()
                msg.setCommand(command)
                msg.setMatrixSize(matrixSize)
                msg.setThreadsNumber(threadsNumber)
                msg.setMatrix()
                msg.print()

            elif (command == "Q"):
                print("Exiting...")
                msg = Message()
                msg.setCommand(command)
                encoded = msg.encode()
                client_socket.sendall(encoded)
                break
            else:
                msg = Message()
                msg.setCommand(command)

            encoded = msg.encode()
            client_socket.sendall(encoded)

            response = client_socket.recv(1) 
            print("Server response:", response)

            if (command == 'G'):
                msg = Message()
                msg.setCommand(command)
                
                length_bytes_total = int.from_bytes(client_socket.recv(4), byteorder='big')
                print("Total bytes:", length_bytes_total)

                receiveBufferMessage = b''
                while len(receiveBufferMessage) < length_bytes_total:
                    chunk = client_socket.recv(length_bytes_total - len(receiveBufferMessage))
                    receiveBufferMessage += chunk
                msg.decode(receiveBufferMessage)
                msg.print()
                
    except socket.error as e:
        print("Connection failed:", e)

    finally:
        client_socket.close()

main()