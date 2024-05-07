#!/usr/bin/env python3

import socket
import sys


def main(argv):
    multicast_group = argv[1]
    multicast_port = int(argv[2])
    interface_ip = argv[3]

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
    sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, 32)
    sock.bind(('', multicast_port))
    sock.setsockopt(socket.SOL_IP, socket.IP_ADD_MEMBERSHIP,
                    socket.inet_aton(multicast_group) + socket.inet_aton(interface_ip))

    while True:
        received = sock.recv(1500)
        print(received)
        print('Received packet of {0} bytes'.format(len(received)))


if __name__ == '__main__':
    if len(sys.argv) != 4:
        print("Usage: {0} <group address> <port> <interface ip>".format(sys.argv[0]))
        sys.exit(1)
    main(sys.argv)