#Adam Abbas Shell Project

###Rolling list of features this shell implements!

* Basic commands (ls, echo etc)!
* CD into a directory in the program! (Will print the location)
* Seperate commands with semicolons! **Don't use spaces**: *command1;command2;command3*
* Piping! Typing in *command*  *|* *command* takes the output from the first command and inputs it into the second command!
* Redirection both ways! Same format as pipe, but use < or > instead of | and put in a file as your second command
* Appending upon redirecting out works! Use >> to append to the second file


###Attempted implementations
* Attempted to read arguments in command *space* ; *space* command format. As of right now, only works without spaces.

###Known bugs
* FIXED (was a return type issue) ~~Commands performed after a pipe run twice? The order also looks like it gets messed up~~

###Files:
Just the one `shell.c`

###HEADERS!:

```=== int find_char() === ```
Inputs:
- char * full_string;
- char to_find;
Returns: The index of char `to_find` in string `full_string` or -1 if not found

``` === int find_string() === ```
Inputs:
- char ** full_string
- char * to_find
Returns: the index of string `to_find` in string array `full_string`

``` === char ** parse_args === ```
Inputs:
- char * line
- int numseps
- char * lf

Returns: A string array that took `line` and seperated it at `lf`
The input numseps is the number of seperations that parse args will run (the number of times lf appears in line)

``` === redirect_out() === ```
Inputs:
- char ** exec
- int greater_than
- int append

Returns: nothing

Takes an array of commands `exec`. Makes the file after the `>` act as STDOUT for the command before `>`. `greater_than` represents the index of the ">", and allows the program to know what to run before and after it. `append` signifies whether the program is dealing with a `>` or '>>'

``` === redirect_in() === ```
Inputs:
- char ** exec
- int less_than
Returns: nothing

Takes an array of commands `exec` and makes the file after `<` act as STDIN for the command before `<`. `less_than` represents the index of the `<`.
