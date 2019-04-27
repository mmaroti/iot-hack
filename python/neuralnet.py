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

    def print_c(self):
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

    def print_c(self):
        return "{:.1f}f".format(self.value)

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

    def print_c(self):
        return self.name

    def const_prop(self):
        self.mark = self


class NodeSum(Node):
    def __init__(self, arg0, arg1):
        self.mark = None
        self.subs = [arg0, arg1]

    def __str__(self):
        return "sum {} {}".format(self.subs[0].mark, self.subs[1].mark)

    def print_c(self):
        return "{} + {}".format(self.subs[0].mark, self.subs[1].mark)

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

    def print_c(self):
        return "{} * {}".format(self.subs[0].mark, self.subs[1].mark)

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

    def print_c(self):
        return "{} > 0.0f ? {} : 0.0f".format(self.subs[0].mark, self.subs[1].mark)

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


class NodeSquare(Node):
    def __init__(self, arg0):
        self.mark = None
        self.subs = [arg0]

    def __str__(self):
        return "square {}".format(self.subs[0].mark)

    def print_c(self):
        return "{} * {}".format(self.subs[0].mark, self.subs[0].mark)

    def const_prop(self):
        sub0 = self.subs[0].mark
        if isinstance(sub0, NodeConst):
            self.mark = NodeConst(sub0.value * sub0.value)
        elif sub0 == self.subs[0]:
            self.mark = self
        else:
            self.mark = NodeSquare(sub0)

    def backprop(self):
        self.subs[0].mark = NodeSum(
            self.subs[0].mark,
            NodeProd(NodeConst(2), NodeProd(self.subs[0], self.mark)))

    def evaluate(self):
        self.value = self.subs[0].value * self.subs[0].value


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


def print_c(nodes):
    nodes = topsort_nodes(nodes)
    for idx, node in enumerate(nodes):
        node.mark = "tmp{}".format(idx)
        print("float tmp{} = {};".format(idx, node.print_c()))
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


def dotprod_bias(left, right, bias):
    assert len(left) == len(right)
    node = bias
    for idx in range(len(left)):
        node = NodeSum(node, NodeProd(left[idx], right[idx]))
    return node


def sum_square_diff(left, right):
    assert len(left) == len(right)
    node = NodeConst(0)
    for idx in range(len(left)):
        s = NodeSum(NodeProd(NodeConst(-1), left[idx]), right[idx])
        node = NodeSum(node, NodeSquare(s))
    return node


def test():
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
    derivs = get_derivs(nodes[0:2])
    print_nodes(derivs)
    nodes[0].value = 2.0
    nodes[1].value = 3.0
    print(evaluate(nodes[8:9]))
    print(evaluate(derivs))
    nodes[0].value = 2.1
    nodes[1].value = 3.0
    print(evaluate(nodes[8:9]))
    nodes[0].value = 2.0
    nodes[1].value = 3.1
    print(evaluate(nodes[8:9]))


def simple_autoenc():
    # old inputs
    x0 = NodeInput("x0")
    x1 = NodeInput("x1")

    # new inputs
    x2 = NodeInput("x2")
    x3 = NodeInput("x3")
    x4 = NodeInput("x4")

    # compression weights
    w0 = NodeInput("w[0]")
    w1 = NodeInput("w[1]")
    w2 = NodeInput("w[2]")
    w3 = NodeInput("w[3]")
    w4 = NodeInput("w[4]")
    w5 = NodeInput("w[5]")
    w6 = NodeInput("w[6]")
    w7 = NodeInput("w[7]")
    w8 = NodeInput("w[8]")
    w9 = NodeInput("w[9]")
    wa = NodeInput("w[10]")
    wb = NodeInput("w[11]")

    # old compressed data
    d0 = NodeInput("d0")
    d1 = NodeInput("d1")

    # new compressed data
    d2 = dotprod_bias([x0, x1, x2, x3, x4], [w0, w1, w2, w3, w4], wa)
    d3 = dotprod_bias([x0, x1, x2, x3, x4], [w5, w6, w7, w8, w9], wb)

    # calculate the relu
    # d2 = NodeRelu(d2, d2)
    # d3 = NodeRelu(d3, d3)

    # decoder weights
    v0 = NodeInput("w[12]")
    v1 = NodeInput("w[13]")
    v2 = NodeInput("w[14]")
    v3 = NodeInput("w[15]")
    v4 = NodeInput("w[16]")
    v5 = NodeInput("w[17]")
    v6 = NodeInput("w[18]")
    v7 = NodeInput("w[19]")
    v8 = NodeInput("w[20]")
    v9 = NodeInput("w[21]")
    va = NodeInput("w[22]")

    # just zero
    z = NodeConst(0.0)

    # output data
    y0 = dotprod_bias([z, d0, z, z, d2, z, d1, z, z, d3],
                      [v0, v1, v2, v3, v4, v5, v6, v7, v8, v9], va)
    y1 = dotprod_bias([d0, z, z, d2, z, d1, z, z, d3, z],
                      [v0, v1, v2, v3, v4, v5, v6, v7, v8, v9], va)
    y2 = dotprod_bias([z, z, d2, z, z, z, z, d3, z, z],
                      [v0, v1, v2, v3, v4, v5, v6, v7, v8, v9], va)

    # calculate the loss
    ls = sum_square_diff([x0, x1, x2], [y0, y1, y2])

    backprop(ls)
    wd = get_derivs([w0, w1, w2, w3, w4, w5, w6, w7, w8, w9, wa, wb])
    vd = get_derivs([v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, va])

    zs = const_prop([ls, d2, d3, y0, y1, y2] + wd + vd)
    print_c(zs)

    print("ls = {};".format(zs[0].mark))
    print("d2 = {};".format(zs[1].mark))
    print("d3 = {};".format(zs[2].mark))
    print("y0 = {};".format(zs[3].mark))
    print("y1 = {};".format(zs[4].mark))
    print("y2 = {};".format(zs[5].mark))

    for idx in range(23):
        print("wd[{}] += {};".format(idx, zs[6 + idx].mark))


def dump_training_data(filename=""):
    import numpy as np
    data = np.load(filename)
    print(data.shape)
    s = ""
    for i in range(0, len(data) / 3):
        if i != 0:
            s += ","
            if i % 20 == 0:
                s += "\n"
            s += str(data[3 * i])
    print(s)


if __name__ == "__main__":
    simple_autoenc()
    # dump_training_data("../data/accel/00_d.np.npy")
