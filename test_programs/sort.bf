Sorry that the sorting part isn't commented; I wrote it a long time ago and lost the original version; I just now added the decimal input reader which is why it is better commented; Instructions: First you type in a two digit decimal number representing how many bytes you are going to input and then you type in those bytes and it spits them out in order; My algorithm for reading a 2 digit number feels rather long so if anyone is able to shorten that then please do so; Written by Aaron Kaufer;



#####READING 2 DIGIT DECIMAL NUMBER#######
>++++++++[-<++++++>]            preload 48 (ascii 0)
,<[->-<]>                       read input then subtract 48 from it
[-<++++++++++>]<[->+<]>>        multiply by ten
>++++++++[-<++++++>],<[->-<]>   in the next two blocks repeat the procedure of reading and subtracting 48
[-<<+>>]<<                      finally add the last block to the second block

#####SORTING#####
[-<+>>,+[->>+>>+<<[->+<]<[->+<]<[->+<]>>>>[-<<<<+>>>>]<<<]>>[-<<<[->>>>+<<<<]>[-<+>]>[-<+>]>[-<+>]<]<<<]>>>>>[>[-<<+<+>>>]<<<[->>>+<<<]>[->-[-<<+<+>>>]<<<[->>>+<<<]+>[<->[-]]<[>>[-]<<[-]]>>]>[-<+>]<[-<<<.>>>]<<<+>>[->+<]<[->+<]<[->+<]>>>>[-<<<<+>>>>]>]

