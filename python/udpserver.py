#!/usr/bin/env python
# Copyright (C) 2019, Gyorgy Kalmar and Miklos Maroti

from __future__ import print_function
import socket
import logging
import time


class Server(object):
    def __init__(self, addr="127.0.0.1", port=1234):
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
        try:
            return self.sock.recvfrom(1024)
        except socket.timeout:
            return None

    def __exit__(self, err_typ, err_val, trace):
        assert self.sock is not None
        logging.info("Closing server")
        self.sock.close()
        self.sock = None


def run(args=None):
    logging.basicConfig(level=logging.INFO)

    with Server() as server:
        while True:
            print(server.recv())


if __name__ == '__main__':
    run()
