import operator

class Function:
    def __init__(self, function, arity):
        self.function = function
        self.arity = arity

    def __call__(self, *args):
        assert(len(args) == self.arity)        # This wastes time; I'll remove it later
        return function(args)

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
       '+': operator.add
    }
    def __init__(self):
        self.stack = []

    def run(self, instructions):
        pass
