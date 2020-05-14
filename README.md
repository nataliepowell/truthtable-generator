# truthtable-generator
Course project in C that generates a truth table for a given boolean logic circuit. Reads input from a text file. Evaluates any combination of AND, OR, NAND, NOR, XOR, NOT, MULTIPLEXER, and DECODER gates. Input file can also contain temporary variables that are not specified as either inputs or outputs.

Example:

INPUT 4   w x y z
OUTPUT 4  a b c d

OR   w 0  a
AND  x 1  b
NAND y 1  c
NOR  z 0  d
_________________
0 0 0 0 | 0 0 1 1
0 0 0 1 | 0 0 1 0
0 0 1 0 | 0 0 0 1
0 0 1 1 | 0 0 0 0
0 1 0 0 | 0 1 1 1
0 1 0 1 | 0 1 1 0
0 1 1 0 | 0 1 0 1
0 1 1 1 | 0 1 0 0
1 0 0 0 | 1 0 1 1
1 0 0 1 | 1 0 1 0
1 0 1 0 | 1 0 0 1
1 0 1 1 | 1 0 0 0
1 1 0 0 | 1 1 1 1
1 1 0 1 | 1 1 1 0
1 1 1 0 | 1 1 0 1
1 1 1 1 | 1 1 0 0
