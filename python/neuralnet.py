#!/usr/bin/env python
# Copyright (C) 2019, Miklos Maroti

from __future__ import print_function


class Node(object):
    def __init__(self):
        self.mark = None
        self.subs = []
        self.value = 0.0

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

    def backprop(self):
        pass

    def evaluate(self):
        pass


class NodeConst(Node):
    def __init__(self, value):
        self.mark = None
        self.subs = []
        self.value = float(value)

    def __str__(self):
        return "const {}".format(self.value)

    def const_prop(self):
        self.mark = self


class NodeInput(Node):
    def __init__(self, name):
        self.mark = None
        self.subs = []
        self.name = name
        self.value = 0.0

    def __str__(self):
        return "input {}".format(self.name)

    def const_prop(self):
        self.mark = self


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
        elif sub0 == self.subs[0] and sub1 == self.subs[1]:
            self.mark = self
        else:
            self.mark = NodeSum(sub0, sub1)

    def backprop(self):
        self.subs[0].mark = NodeSum(self.subs[0].mark, self.mark)
        self.subs[1].mark = NodeSum(self.subs[1].mark, self.mark)

    def evaluate(self):
        self.value = self.subs[0].value + self.subs[1].value


class NodeProd(Node):
    def __init__(self, arg0, arg1):
        self.mark = None
        self.subs = [arg0, arg1]
        self.value = 0.0

    def __str__(self):
        return "prod {} {}".format(self.subs[0].mark, self.subs[1].mark)

    def const_prop(self):
        sub0 = self.subs[0].mark
        sub1 = self.subs[1].mark
        if isinstance(sub0, NodeConst) and isinstance(sub1, NodeConst):
            self.mark = NodeConst(sub0.value * sub1.value)
        elif isinstance(sub0, NodeConst) and sub0.value == 1:
            self.mark = sub1
        elif isinstance(sub0, NodeConst) and sub0.value == 0:
            self.mark = NodeConst(0)
        elif isinstance(sub1, NodeConst) and sub1.value == 1:
            self.mark = sub0
        elif isinstance(sub1, NodeConst) and sub1.value == 0:
            self.mark = NodeConst(0)
        elif sub0 == self.subs[0] and sub1 == self.subs[1]:
            self.mark = self
        else:
            self.mark = NodeProd(sub0, sub1)

    def backprop(self):
        self.subs[0].mark = NodeSum(self.subs[0].mark,
                                    NodeProd(self.subs[1], self.mark))
        self.subs[1].mark = NodeSum(self.subs[1].mark,
                                    NodeProd(self.subs[0], self.mark))

    def evaluate(self):
        self.value = self.subs[0].value * self.subs[1].value


class NodeRelu(Node):
    def __init__(self, arg0, arg1):
        self.mark = None
        self.subs = [arg0, arg1]
        self.value = 0.0

    def __str__(self):
        return "relu {} {}".format(self.subs[0].mark, self.subs[1].mark)

    def const_prop(self):
        sub0 = self.subs[0].mark
        sub1 = self.subs[1].mark
        if isinstance(sub0, NodeConst):
            if sub0.value > 0:
                self.mark = sub1
            else:
                self.mark = NodeConst(0)
        elif isinstance(sub1, NodeConst) and sub1.value == 0:
            self.mark = NodeConst(0)
        elif sub0 == self.subs[0] and sub1 == self.subs[1]:
            self.mark = self
        else:
            self.mark = NodeRelu(sub0, sub1)

    def backprop(self):
        self.subs[1].mark = NodeSum(self.subs[1].mark,
                                    NodeRelu(self.subs[0], self.mark))

    def evaluate(self):
        self.value = self.subs[1].value if self.subs[0].value > 0.0 else 0.0


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


def backprop(out):
    nodes = topsort_nodes([out])
    for node in nodes:
        node.mark = NodeConst(0)
    out.mark = NodeConst(1)
    for idx in range(len(nodes) - 1, -1, -1):
        nodes[idx].backprop()


def get_derivs(nodes):
    return const_prop([node.mark for node in nodes])


def evaluate(nodes):
    nodes2 = topsort_nodes(nodes)
    for node in nodes2:
        node.evaluate()
    return [node.value for node in nodes]


if __name__ == "__main__":
    nodes = []
    nodes.append(NodeInput("v1"))
    nodes.append(NodeInput("v2"))
    nodes.append(NodeConst(1.0))
    nodes.append(NodeConst(0.0))
    nodes.append(NodeSum(nodes[0], nodes[2]))
    nodes.append(NodeSum(nodes[1], nodes[3]))
    nodes.append(NodeRelu(nodes[4], nodes[4]))
    nodes.append(NodeRelu(nodes[5], nodes[5]))
    nodes.append(NodeProd(nodes[6], nodes[7]))
    print_nodes(nodes)
    print_nodes(const_prop(nodes[8:]))
    backprop(nodes[8])
    derivs = get_derivs(nodes[0:1])
    print_nodes(derivs)
    nodes[0].value = 2.0
    nodes[1].value = 2.0
    print(evaluate(nodes))
    print(evaluate(derivs))
