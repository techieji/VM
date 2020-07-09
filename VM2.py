import operator
import sys

variables = {}

def setvar(varname, value): # I have to do this because of Python's stupid lambda expressions
    variables[varname] = value

class Termination(Exception):
    pass

def terminate():
    raise Termination("Program terminated")

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
        "if": (3, lambda x, y, z: y if z else x),
        "set": (2, lambda x, y: setvar(x, y)),
        "get": (1, lambda x: variables[x]),
        "terminate": (0, terminate)
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
    ins = formattext(text)
    instructions = []
    macros = {}
    for x in ins:
        if x[0:2] == '--':
            if x[0:9] == '--include':
                filename = x[10:-1]
                with open(filename) as f:
                    filetext = formattext(f.read())
                    modulename = filetext[0][11:-1]
                    macros[modulename] = filetext[1:]
        else:
            if x in macros.keys():
                macrovalue = macros[x]
                for i in macrovalue:
                    instructions.append(i)
            else:
                instructions.append(x)
    m = Machine()
    m.execute(instructions)

def execute2(ins: str):
    m = Machine()
    ins = formattext(ins)
    m.execute(ins)

def repl():
    m = Machine()
    try:
        while True:
            m.doins(input('> '))
    except KeyboardInterrupt:
        repl()
    except EOFError:
        return

def main():
    if len(sys.argv) == 2:
        with open(sys.argv[1]) as f:
            execute(f.read())
    elif len(sys.argv) == 3:
        with open(sys.argv[1]) as f:
            if sys.argv[2] == "--nomacro":
                execute2(f.read())
            elif sys.argv[2] == "--macro":
                execute(f.read())
    else:
        print("Entering REPL...")
        repl()

if __name__ == "__main__":
    main()
