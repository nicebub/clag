# sclex
> A Lexical Analyzer Generator written in **C** -- currently works on Linux and OS X.
## General
sclex follows the same specifications that ***lex*** and **GNU *flex*** follow, *except* for what hasn't
     been implemented *yet*.

## Background
I wrote sclex as my first project to get back into programming after some time AFK.

Currently,
  I am adding refactoring, adding unit tests via Unity, and CMock, and adding to the already existing
  documentation.

### Other Goals

*   For educational purposes I am not using the C Standard Library much
   but I am implementing my own data structures, alogorithms, and functions
 
*   I intend on trying to recompile sclex with my own C compiler when that is finished.
   
*   Add additional Unit Tests and Mocks.
   
*   Keep refactoring and refining code - going more modular and object-oriented.

*   Remove existing use of streams and buffers from C Standard Library and use OS calls instead.

  EXCUSE THE MESS! 
## Internal Features
* *Recursive Descent* Predictive **Parser** (to parse the *Regular Expressions*(RE's) in the specificiation file
* Internal *Syntax-Tree* Construction
* Tabularized *Determinitistic Finite Automata*(**DFA**) construction
* Implements Dragon Book Algorithms firstpos,lastpos,followpos,nullable, etc.
* Double-Sided Buffer implementation


## User Features
* regular expressions used to define token strings to recognize
* regular expression short-hand definitions
* character class set and ranges
* a subset of escape characters
* lex/flex specification file -- ending in **.l**
* **and more...**

# Compiling
To COMPILE: (in the source directory) Run make:
```
computer:src/sclex nicebub$ make
```
# Running
To Run the generator on the specification file and generate the source file "sclex.yy.c"
(lex.l is a properly written specification file)
```
computer:src/sclex nicebub$ ./build/sclex example/lex.l 
```

To Run the demo lexer driver
Use the included "ex.r" file in the examples folder, with the "lex_driver" demo binary.

The "ex.r" example follows the language definition described in the example "lex.l"

First make the lex_driver binary:
```
computer:src/sclex nicebub$ make lex_driver
```

Then run the lex_driver binary on the example file:_
```

computer:src/sclex nicebub$ ./build/lex_driver examples/ex.r
```

# Running Tests
First initialize the git submodules once, within the project directory:
```
computer:src/sclex nicebub$ git submodule init
```

Next, run the genrunner.sh script from the tools directory to create the test runners. Start this script from the root directory of the source code project:

```
computer:src/sclex nicebub$ ./tools/genrunners.sh
```

Then run make test:

```
computer:src/sclex nicebub$ make test
```

## Regular Expression Usages and Definitions
In the right column are examples or symbols that represent what can be written as a regular expression. The regular expressions are used to to tell the lexer how to match patterns of the input.

Usage | Definition
----- | ----------
_a_   | _a_ stands for any alpha-numeric or printable character, will be matched
_ab_  | _ab_ two regular expressions concatenated with each other, a followed by b will be matched
_(E)_ | _(E)_ is a regular expression, ie, a string surrounded by parenthesis will be matched
_E\|C_ | Both _E_ and _C_ are regular expressions and either one can be matched
_E\*_  | stands for 0 or more _E_ regular expressions concatenated with each other can match the input
_E\?_  | stands for 0 or 1 _E_ regular expression will match the input
_E\+_  | stands for 1 or more _E_ regular expression concatenated will match the input stream
_\[abc\]_| stands for a character set that matches any of the characters in the brackets to one in the input
_\[a-z\]_| stands for a character set that can match the input to any character in the range of lowercase letters a through z
_\\x_   | is an escape character, x can be any of: n,t,0,\\,\|,\.,\[,\],\{,\},\(,\),\*,\?,and \%
_\{name\}_ | is a short-hand definition name surrounded by curly brackets, expanded and matched to the input
### Short-hand definitions
The lexer supports defining names to represent long or heavily-used regular expressions.  These definitions are written in the top section of the lexer spec file of the form:
```
definition name 	regular expression
```
definitions can include previously defined short-hand names in the file above them, ie.
```
{ title }		{ movie } | { tv }*
```

You can find more examples in the included file **lex.l**

# Specification File
This file is read by the generator and used to create the C source file that will later become your lexer.

The layout of the file is as follows
```
%{
	/* included C source code and possibly token definitions, ie. */

#include <stdio.h>

#define TOKEN1 5
#define STMT 6
#define PLUS 7
....

%}

	/* short-hand definitions */

digit [0-9]
digits {digit}+
...

%%

	/* regular expressions with C source code to run when this
	 * regular expression is matched to the input */

a|b*(cat[cat]){digits}   _at least 1 required space token_ {
	return STMT; 			/* returns token STMT defined above */

}

{expr} {
	printf("found a expression!\n");
	return EXPR;
}

%%

/* Auxillary Code to include into the generated lexer */

int my_function(float* in_float, int num_args){
	...
	...
	return blah;
}
```

# Output
The generator will output a C source file called "sclex.yy.c". Within this file is a function called **sclex()**. This function can be called from another program or *main* function to start tokenizing input from an already opened file represented with a **FILE** pointer.

The prototype for the function is:
```
int sclex(FILE * opened_file);
```
Calling this function will run the lexer. 
When a token is found, the existing C code will be run. Ideally it will eventually return an **INTEGER** which represents a token defined somewhere else in the program.

Subsequent calls to ***sclex(filename)*** will run the lexer again on any left over file input in the buffer.
(See lex_test.c for a demo use case of the lexer)

# Further Goals
A few GOALS for this projects progress:

* I would love community help with:
	* any coding 
	* suggestions
	* code review
	* bug discovery 
	* feature requests
	* feature implementation
	* etc...
* I intend on creating a few other tools to complement sclex:
	* a suite, or toolchain, for a complete compiler.
	* a YACC/BISON minimal type of clone to create the next stage to my own compiler - available [here](https://github.com/nicebub/scyak/).
			
* Sclex Features
	* To make this program a threaded app where the lexer will buffer at most a certain amount of tokens
	* clean up character sets so that can have multiple shorthand classes in the set.
	* clean up further character class so isn't dependent on '-' being second and '-' can be last symbol in class.
	* add negative character classes ie the ^ symbol, which matches anything not mentioned in the class minus newline '\n'
	* add . as a regular expression matching any character of the alphabet
	* add the ability to have comments in the entire .l file or portions of the file.
	* eventually add the lookahead operator '/'.
	* use better set data structure and algorithms.
		* would like to go to bitmaps, for ASCII for char sets
		* for integer sets maybe start at a minimum of 10 items and if needed,
		* dynamically make the set larger, but still bitmapped?
	* take out all c library code, if possible, anything that uses string.h or stdlib for minor things like NULL
	* add buffer class to actual output file so isn't an included file with header
	* change buffer to use read instead of fread perhaps
	* add the ability for stack use for a stack of dfas
		* Thus stack swaps from dfa to dfa depending on context. (like for string or comment, normal, etc.)
	* need to add literal and string support so characters can be written normally between quotations.
	* add the ability to swap buffers
		* this may be useful when converting regex definition short-hands and we don't want to manipulate the original buffer but still use, the current code for translations which already uses the existing buffer code.
	* add operators like $ and ^ representing end and beginning of match
	
# Contact
Feel free to contact [me](mailto:scott@laureltreecomputing.com) with questions, comments, suggestions, etc. I'm open to feedback.
