#!/usr/bin/env python
# Copyright (C) 2019, Miklos Maroti

from __future__ import print_function


class Node(object):
    def __str__(self):
        pass

    def mark(self, markers):
        pass

    def copy(self, nodes):
        pass

    def const_prop(self, prog, nodes):
        return prog.add(self.copy(nodes))


class NodeConst(Node):
    def __init__(self, value):
        self.value = value

    def __str__(self):
        return "const {}".format(self.value)

    def copy(self, nodes):
        return NodeConst(self.value)


class NodeInput(Node):
    def __init__(self, name):
        self.name = name is not None

    def __str__(self):
        return "input {}".format(self.name)

    def copy(self, nodes):
        return NodeInput(self.name)


class NodeSum(Node):
    def __init__(self, arg1, arg2):
        assert arg1 is not None and arg2 is not None
        self.arg1 = arg1
        self.arg2 = arg2

    def __str__(self):
        return "sum {} {}".format(self.arg1, self.arg2)

    def mark(self, markers):
        markers[self.arg1] = True
        markers[self.arg    assert arg1 is not None and arg2 is not None
    self.arg1 = arg1
    self.arg2 = arg2
2] = True

    def copy(self, nodes):
        return NodeSum(nodes[self.arg1], nodes[self.arg2])

    def const_prop(self, prog, nodes):
        n1 = prog.nodes[nodes[self.arg1]]
        n2 = prog.nodes[nodes[self.arg2]]
        print(n1, n2)
        c1 = isinstance(n1, NodeConst)
        c2 = isinstance(n2, NodeConst)
        print(c1, c2)
        if c1 and c2:
            return NodeConst(n1.value + n2.value)
        elif c1 and n1.value == 0:
            return n2
        elif c2 and n2.value == 0:
            return n1
        else:
            return prog.add(self.copy(nodes))


class NodeProd(Node):
    def __init__(self, arg1, arg2):
        assert arg1 is not None and arg2 is not None
        self.arg1 = arg1
        self.arg2 = arg2

    def __str__(self):
        return "prod {} {}".format(self.arg1, self.arg2)

    def mark(self, markers):
        markers[self.arg1] = True
        markers[self.arg2] = True

    def copy(self, nodes):
        return NodeProd(nodes[self.arg1], nodes[self.arg2])


class NodeRelu(Node):
    def __init__(self, arg):
        assert arg is not None
        self.arg = arg

    def __str__(self):
        return "relu {}".format(self.arg)

    def mark(self, markers):
        markers[self.arg] = True

    def copy(self, nodes):
        return NodeRelu(nodes[self.arg])


class Program(object):
    def __init__(self):
        self.nodes = []

    def add(self, node):
        assert isinstance(node, Node)
        self.nodes.append(node)
        return len(self.nodes) - 1

    def __str__(self):
        result = ""
        for index, node in enumerate(self.nodes):
            result += "{}: {}\n".format(index, node)
        return result

    def dead_code_elim(self, outputs):
        markers = [False] * len(self.nodes)
        for index in outputs:
            markers[index] = True
        for index in range(len(markers)-1, -1, -1):
            if markers[index]:
                self.nodes[index].mark(markers)

        prog = Program()
        nodes = [None] * len(markers)
        for index in range(0, len(markers)):
            if markers[index]:
                nodes[index] = prog.add(self.nodes[index].copy(nodes))
        return prog

    def const_prop(self):
        prog = Program()
        nodes = [None] * len(self.nodes)
        for idx in range(0, len(nodes)):
            nodes[idx] = self.nodes[idx].const_prop(prog, nodes)
        return prog


if __name__ == "__main__":
    prog = Program()
    v1 = prog.add(NodeInput("v1"))
    v2 = prog.add(NodeConst(1.0))
    v3 = prog.add(NodeConst(0.0))
    v4 = prog.add(NodeSum(v1, v3))
    print(prog)
    print(prog.dead_code_elim([v4]))
    print(prog.const_prop())
