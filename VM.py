import operator

class Function:
    def __init__(self, function, arity):
        self.function = function
        self.arity = arity

    def __call__(self, *args):
        assert(len(args) == self.arity)        # This wastes time; I'll remove it later
        return function(args)

f = Function  # In essence, this is a typedef

class EvalTree:
    def __init__(self, function, arity, branches=[], parent=None):
        self.function = function
        self.arity = arity
        self.branches = branches
        self.parent = parent

    def addbranch(self, branch):
        self.branches.append(branch)

    def getparent(self):
        return self.parent

    def eval(self):
        if type(self) != EvalTree:
            return self

        else:
            return self.function(*map(EvalTree.eval, self.branches))

class Machine:
    instructionset = {
       '+': f(operator.add, 2)
    }
    def __init__(self):
        self.stack = []

    def makeTree(self, instructions: list) -> EvalTree:
        # 1. Scan the instructions for functions
        def isfunction(code: str) -> bool:
            return code in Machine.instructionset.keys()

        function = map(isfunction, instructions)
        
        # 2. Recursivly construct the tree
        args = []
        zipped = zip(range(len(instructions)), instructions, function)
        f = Machine.instructionset[instructions[0]]
        for i, ins, isfunc in zipped:
            if isfunc:
                r = []
                function = ins
                i, ins, isfunc = next(zipped)
                while len(r) <= Machine.instructionset[function].arity:
                    r.append(inc)
                args.append(None)
            else:
                args.append(ins)
        
        # 3. Run the tree
        EvalTree(f, f.arity, args).eval()
