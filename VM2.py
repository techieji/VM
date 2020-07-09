import operator

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

def execute(ins: str):
    m = Machine()
    m.execute(ins.split('\n'))

with open("file.vm") as f:
    execute(f.read())
