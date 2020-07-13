# VM

Hello! This VM is meant for my personal uses, but it certainly can be used by other people. It is meant to be easy to understand and easy to create. One of my future goals is to make it fast, but to get there, I first have to make it competent. Please note that development on this VM will be stagnant until I find reason to add another operation. Here are the features which this VM currently has:

## Features
### Stack-based
Every function follows the same blueprint. The function pops its inputs from the stack and then pushs its outputs back onto the stack.

### Arithmetic
This VM has many different arithmetic operations, with more to come. It currently supports addition, subtraction, multiplication, true division, and floor division. It also supports a comparison operator, `cmp` which returns 0 if the inputs are equal, 1 if the first input is greater than the second, and -1 if the first input is less than the second.

### Types
My VM supports 3 different types: integers, strings, and booleans. I'm going to add floats pretty soon.

### Conditionals
Every good language has to have conditionals, be it high level or assembly. My implementation of `if` is fairly rudimentary at this time. The `if` function takes a boolean and two values. If the boolean is true, the first value is pushed to the stack; otherwise, the second value is pushed. The VM also contains some helpful functions for working with the `if` statement. `eq`, `lt`, and `gt` are other comparison operators. These ones, however, push a boolean to the stack, unlike `cmp`. The test file included with this, `addorsubtract.vm`, contains an example of how to use the `if` statement in conjunction with the other comparison operators.

### Procedures
Procedures are currently implemented as macros. If you put a section of code in a file and import it into another, the macro will be expanded and then the file will be executed.

### IO
Currently, there are two operations for IO: `inp`, which takes input and pushes it to the stack, and `out` which pops from the stack and prints it. `inp` returns a string, so if you need a number, you need to cast it with the `int` function.

### Macros
With macros, you can define functions and import functions in different files. The first two macros I'll be implementing are `--include` and `--function`, which allow you to import and define function, respectively. Each file can only contain one function (sorry!) but I'll do my best to make it easier to use just one file. Although these macros can help understandabilty in some cases, when used in conjunction with the `jmp` function, it can bring out some unintuitive behavior.

### Jump Instruction
The jump instruction, `jmp`, allows you to jump to a specific line number. Note that macros are expanded before execution and comments are removed, so make sure you take that into account. The guess the number game, located at `examples/guessthenumber`, gives an example of the unintuitive behavior that using macros with the jump instruction does. `examples/guessthenumber/best.vm` gives an example of proper code. Its only downside is that the lack of comments make it hard to understand.

## Examples
If you want to see some examples, here you go! This example adds two numbers given from the input:
```
; Semicolons start a comment
inp
int
; This reads a number from the input
inp
int
; You need two numbers to add them
add
out
; This prints the value of the addition
```

The `examples` folder also contains many examples. You can run them as stated in the section below.

## Use
You can use run a `.vm` file like this:
```
$ python3 VM2.py examples/addorsubtract.vm
5
+
6
11
$
```
In order to run multiple files, you do not give multiple arguments to the VM. All you have to do is include them from an arbitrary `main.vm` file, which will then import all the files and run them when necessary.

You can also run using the executable, located in `bin`:
```
$ ./VM2 examples/addorsubtract.vm
5
+
6
11
$
```

There is also a repl for this VM. You can run it like this:
```
$ python3 VM2.py
Entering REPL...
>
```
You can execute instructions in it. There is no way to restart the repl except by pressing Ctrl-D and running the repl again. 
