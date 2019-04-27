#!/usr/bin/env python
# Copyright (C) 2019, Gyorgy Kalmar and Miklos Maroti

from __future__ import print_function
import socket
import logging
import time
import geopy.geocoders


class Server(object):
    def __init__(self, addr="0.0.0.0", port=1973):
        self.addr = addr
        self.port = port
        self.sock = None

    def __enter__(self):
        assert self.sock is None
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.settimeout(1)
        self.sock.bind((self.addr, self.port))
        logging.info("Server running on %s (%s)", self.addr, self.port)
        return self

    def recv(self):
        """Returns (data, addr) pair"""
        try:
            return self.sock.recvfrom(1024)
        except socket.timeout:
            return (None, None)

    def send(self, data, addr):
        self.sock.sendto(data, addr)

    def __exit__(self, err_typ, err_val, trace):
        assert self.sock is not None
        logging.info("Closing server")
        self.sock.close()
        self.sock = None


def run(args=None):
    logging.basicConfig(level=logging.INFO)
    geolocator = geopy.geocoders.GoogleV3()

    with Server() as server:
        while True:
            (data, addr) = server.recv()
            if data is not None:
                logging.info("Received %s from %s",
                             data.replace("\n", "<cr>"), addr)
                if data.startswith("gps"):
                    # location = geolocator.reverse("46.249937, 20.146077")
                    # print(location.address)
                    print("SZTE, 13, Dugonics tér, Belváros, Szeged, Szegedi járás, Csongrád megye, Dél-Alföld, Alföld és Észak, 6720, Magyarország")
                server.send("Hello", addr)


if __name__ == '__main__':
    run()
