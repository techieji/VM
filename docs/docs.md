# Documentation
I'm quite lazy, so I'll be writing the docs as one or more Markdown documents.

## Functions
If you've read the readme, you'll know that every function sucks its arguments from the stack and returns its output to the stack. This makes the code hard-to-understand, but very easy to implement.

### Constants
Constants are written as is. That is, a constant with the value of `5` will be written as `5`. The one catch is that by default, the type of constants is a string. To cast it to another type, use one of the functions `int`, `bool`, or rarely, `str`. For example:
```
5
int
```
will lead to the stack having a value of the integer 5.

### Arithmetic
This section will be pretty self-explanatory. All of these functions take two numbers from the stack and do something with them

#### Add
Adds the numbers from the stack together. Note, if two strings are supplied, it concatenates.

#### Sub
Takes the difference.

#### Mul
Takes the product.

#### Div
Divides and returns a float.

#### Fdiv
Divides and returns the floor as an integer

### Booleans
All of the functions in this section either do or use booleans

#### Cmp
Takes two numbers from the stack and compares them. If they are equal, the result is 0. If the first is greater, the result is 1. If the second is greater, the result is -1. The reason this is included in this section is because, in conjunction with `eq`, it can be used to compare numbers.

#### Lt
Less than. Returns a boolean

#### Gt
Take a guess. If you didn't know, it's greater than.

#### Eq
Okay, you're smart enough to know this one.

#### If
Sucks a boolean and two values from the stack and returns one of the two values, depending on the boolean. Here's an example:
```
inp
+
eq
1
-1
if
```
This code returns 1 if the input is a `+`, otherwise, it returns -1. This is actually how `examples/addorsubtract.vm` is implemented. If you think that this definition is quite useless, you are quite wrong. You can conditionally execute code with the `jmp` instruction. I'll add an example later.
