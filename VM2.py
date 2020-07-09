import operator
import sys

instructionset = {                                 # everything has to be in reverse order because the vm is stack-based
        "int": (1, lambda x: int(x)),
        "str": (1, lambda x: str(x)),
        "bool": (1, lambda x: bool(x)),
        "inp": (0, lambda: input()),
        "out": (1, lambda x: print(x)),
        "add": (2, operator.add),
        "sub": (2, lambda x, y: y - x),
        "mul": (2, operator.mul),
        "div": (2, lambda x, y: y / x),
        "fdiv": (2, lambda x, y: y // x),
        "cmp": (2, lambda x, y: 0 if x == y else -1 if x > y else 1),
        "eq": (2, lambda x, y: x == y),
        "lt": (2, lambda x, y: y < x),
        "gt": (2, lambda x, y: y > x),
        "if": (3, lambda x, y, z: y if z else x)
}

class Machine:
    def __init__(self):
        self.stack = []

    def pop(self):
        return self.stack.pop()

    def push(self, x):
        self.stack.append(x)

    def doins(self, ins):
        try:
            instruction = instructionset[ins]
        except KeyError:
            self.push(ins)
            return
        i = instruction[0]
        args = []
        for _ in range(i):
            args.append(self.pop())
        self.push(instruction[1](*args))

    def execute(self, ins):
        for x in ins:
            self.doins(x)

def formattext(text: str):
    ins = text.split('\n')
    ins = filter(bool, ins)
    ins = [x for x in ins if x[0] != ';']
    return ins

def execute(text: str):
    m = Machine()
    ins = text.split('\n')
    ins = list(filter(bool, ins))
    instructions = []
    macros = {}
    for x in ins:
        if x[0] == '-':
            if x[0:9] == '-include(':
                filename = x[9:-1]
                with open(filename) as f:
                    filetext = formattext(f.read())
                    modulename = filetext[0][8:-1]
                    macros[modulename] = filetext[1:]
        elif x[0] != ';':
            if x in macros.keys():
                macrovalue = macros[x]
                for i in macrovalue:
                    instructions.append(i)
            else:
                instructions.append(x)
    m.execute(instructions)

with open(sys.argv[1]) as f:
    execute(f.read())
