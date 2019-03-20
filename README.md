     This is a lexical analyzer generator, written in C, to mimic the functionality LEX/FLEX. I wrote this all in the month of October '18 as a project to get back into programming after 15 years.  My skills were never that strong to begin with, and they still lack quite a bit as you will notice from the source code.  I have yet add any commenting as I was in a rush to complete the project previously and I just picked it back up March '19 to continue finishing it up.  I am motivated currently to work on this program again. I hope to turn this experiment and learning-to-code-again experience into something understandable and useful.
	 
     The lexical analyzer generator is currently an executable called "scandriver" as I've been in development mode.
	 
	 Running "scandriver" at the command line, it takes one argument a file ending in .l like the included "lex.l" example.
	 
		 The file format for the ".l" file is based off of lex/flex language specification file and should be similar:
		 First section starts with %% for includes and definitions.	 
		 Following another %% the rules section and its including user code	 
		 The last section follows another %% and is any other user-defined functions and needed code.
	 	 

	"scandriver" will output a C source file called "sclex.yy.c"  -- which currently includes and requires the file buffer.h --
	
	This file is the lexical analyzer that is generated based on the rules of language from the .l file (lex.l)


	(the following prototype is found in the included lex_test.c lex_driver source file)
		extern int sclex(FILE* infile);

	
	The function sclex(), which is included in the "sclex.yy.c" source file, is the main lexical analyzer. sclex takes a FILE pointer from an opened file and returns the first token found, any subsequent calls to sclex() which will be on that same FILE pointer will return the following tokens found in order.  -- see lex_test.c for an example.
	
	-- scandriver --
	 To COMPILE:
	 	at the command line prompt: make    (in the source directory)

	 
	 To RUN the included test driver:
	 	at the command line prompt: ./scandriver rules.l

							(where rules.l is language specification file)	


	-- lex_driver tester -- 
	 To COMPILE:
	 	at the command line prompt: make    (in the source directory)

	 
	 To RUN the included test driver:
	 	at the command line prompt: ./lex_driver input_min_c_file  

							(where input_min_c_file meets the requirements of the lex.l definitions)

	Use the included "ex.r" file with the "lex_driver" demo binary. The "ex.r" example follows the language definition described in the example "lex.l"

	 
	 
	 I have a few GOALS for this project:
	 		- I would love community help with any coding, suggestions, review, bug find, feature requests,
													Additions, changes, etc... (Am I leaving anything out?)
			
			- I intend on creating a few other tools, a suite, or toolchain, for a complete compiler.  My next tool will I am currently going to be working on is a YACC/BISON minimal type of clone to create the next stage to my own compiler I am constructing.  This other tool will be made will be made to work with CLAG.
			
			
			- I would like to make this program a threaded app where the lexer will buffer at most a certain amount of tokens, that is equal to the size of a particular buffer for the tokens, up to the end of all the tokens which is, like -1 or EOF...

			-  clean up character sets so that can have multiple shorthand classes
			    in the set.

			- clean up further character class so isn't dependent on minus
			            being second and minus sign can be last symbol in class.

			- add negative character classes ie the ^ symbol, which matches
			            anything not mentioned in the class minus newline '\n'

			- add . as a regular expression matching any character of the alphabet

			- add the ability to have comments in the entire .l file or portions
			            of the file.

			- eventually add the lookahead operator '/'

			- use better set data structure and algorithms.
			            would like to go to bitmaps, for ASCII for char sets
			                    and for integer sets maybe start at a minimum of
			                    10 items and if needed, dynamically make the
			                    set larger, but still bitmapped?

			- take out all c library code, if possible, anything that uses
			            string.h or stdlib for minor things like NULL

			- add buffer class to actual output file so isn't an included file with header

			- change buffer to use read instead of fread perhaps

			- add the ability for stack use for a stack of dfas, then can swap from
			            dfa to dfa depending on context. like for string or comment, normal, etc.

			- need to add literal and string support so characters can be written normally
			            between quotations.

			- add the ability to swap buffers, this may be useful when converting regex definition
			            short-hands and we don't want to manipulate the original buffer but still use, the
			            current code for translations which already uses the existing buffer code.

			            --further on list--

		 	- add operators like $ and ^ representing end and beginning of match