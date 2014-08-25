Sorry that the sorting part isn't commented; I wrote it a long time ago and lost the original version; I just now added the decimal input reader which is why it is better commented; Instructions: First you type in a two digit decimal number representing how many bytes you are going to input and then you type in those bytes and it spits them out in order; My algorithm for reading a 2 digit number feels rather long so if anyone is able to shorten that then please do so; Written by Aaron Kaufer;

[The way the magic formula works:
10*(x-48)+(y-48) = 10*x-480+y-48 = 10*x+y-520 = 10*x+y-16-256-256 -> 10*x+y-16]


magic formula: 10*x plus y minus 16; only works with 8 bit cells
,[->++++++++++<] |0|10x|
,[->+<]>         |0|10x plus y|
---------------- |0|10x plus y minus 16|

#####SORTING#####
[-<+>>,+[->>+>>+<<[->+<]<[->+<]<[->+<]>>>>[-<<<<+>>>>]<<<]>>[-<<<[->>>>+<<<<]>[-<+>]>[-<+>]>[-<+>]<]<<<]>>>>>[>[-<<+<+>>>]<<<[->>>+<<<]>[->-[-<<+<+>>>]<<<[->>>+<<<]+>[<->[-]]<[>>[-]<<[-]]>>]>[-<+>]<[-<<<.>>>]<<<+>>[->+<]<[->+<]<[->+<]>>>>[-<<<<+>>>>]>]





########################################################################################################
This is the old draft of the number reader; It works regardless of the bits per cell but is much longer;
[

#####READING 2 DIGIT DECIMAL NUMBER#######
>++++++++[-<++++++>]            preload 48 (ascii 0)
,<[->-<]>                       read input then subtract 48 from it
[-<++++++++++>]<[->+<]>>        multiply by ten
>++++++++[-<++++++>],<[->-<]>   in the next two blocks repeat the procedure of reading and subtracting 48
[-<<+>>]<<                      finally add the last block to the second block

]