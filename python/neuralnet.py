#!/usr/bin/env python
# Copyright (C) 2019, Miklos Maroti

from __future__ import print_function


class Node(object):
    def __init__(self):
        self.mark = None
        self.subs = []

    def remark(self, value):
        self.mark = value
        for sub in self.subs:
            sub.remark(value)

    def topsort(self, list):
        if self.mark is None:
            for sub in self.subs:
                sub.topsort(list)
            self.mark = True
            list.append(self)

    def __str__(self):
        pass

    def const_prop(self):
        pass


class NodeConst(Node):
    def __init__(self, value):
        self.mark = None
        self.subs = []
        self.value = value

    def __str__(self):
        return "const {}".format(self.value)

    def const_prop(self):
        self.mark = NodeConst(self.value)


class NodeInput(Node):
    def __init__(self, name):
        self.mark = None
        self.subs = []
        self.name = name

    def __str__(self):
        return "input {}".format(self.name)

    def const_prop(self):
        self.mark = NodeInput(self.name)


class NodeSum(Node):
    def __init__(self, arg0, arg1):
        self.mark = None
        self.subs = [arg0, arg1]

    def __str__(self):
        return "sum {} {}".format(self.subs[0].mark, self.subs[1].mark)

    def const_prop(self):
        sub0 = self.subs[0].mark
        sub1 = self.subs[1].mark
        if isinstance(sub0, NodeConst) and isinstance(sub1, NodeConst):
            self.mark = NodeConst(sub0.value + sub1.value)
        elif isinstance(sub0, NodeConst) and sub0.value == 0:
            self.mark = sub1
        elif isinstance(sub1, NodeConst) and sub1.value == 0:
            self.mark = sub0
        else:
            self.mark = NodeSum(sub0, sub1)


class NodeProd(Node):
    def __init__(self, arg0, arg1):
        self.mark = None
        self.subs = [arg0, arg1]

    def __str__(self):
        return "prod {} {}".format(self.subs[0].mark, self.subs[1].mark)


class NodeRelu(Node):
    def __init__(self, arg):
        self.mark = None
        self.subs = [arg]

    def __str__(self):
        return "relu {}".format(self.subs[0].mark)


def topsort_nodes(nodes):
    for node in nodes:
        node.remark(None)
    nodes2 = []
    for node in nodes:
        node.topsort(nodes2)
    return nodes2


def print_nodes(nodes):
    nodes = topsort_nodes(nodes)
    for idx, node in enumerate(nodes):
        node.mark = idx
        print("{}: {}".format(idx, node))
    print()


def const_prop(nodes):
    nodes2 = topsort_nodes(nodes)
    for node in nodes2:
        node.const_prop()
    return [node.mark for node in nodes]


if __name__ == "__main__":
    nodes = []
    nodes.append(NodeInput("v1"))
    nodes.append(NodeConst(1.0))
    nodes.append(NodeConst(0.0))
    nodes.append(NodeSum(nodes[0], nodes[2]))
    nodes.append(NodeSum(nodes[1], nodes[2]))
    print_nodes(nodes)
    nodes2 = const_prop(nodes[3:])
    print_nodes(nodes2)
