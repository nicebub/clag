/**
        This is the source code file to the main sclex executable.
sclex is a lexical analyzer(lexer) generator which is a clone
of lex/flex.  Although the algorithms implemented are more
basic than what is found in today's versions of lex/flex,
they are enough to provide an efficent way to tokenize an
input stream.
*/

#include "../include/sclex_driver.h"

/** Function Prototype
        struct _ta *parse_file(buffer * mbuf,char *c, struct _lfile*);

Functionality: This function will run the parser on the specification
        file which will ultimately create a parse tree of the regular
        expressions constructed in the specification.

Parameters: buffer* mbuf, a pointer to an initialized buffer to use as a
        buffer to the input file, a pointer to a character char* c, used to
        loop through the input, and a pointer to an initialized
        lexfile _lfile*, pointing to the input file

Return Value: A pointer "struct _ta*" to a Full Parse Tree of the input

Results: The input specification file is parsed per section, the internal
         data structures store the short-hand declarations and RAW C code,
        then a parse tree is constructed from the definded regular expressions.
        Each regular expression's attached RAW C code fragments will also be
        stored internally and associated with their respective parts.

*/
struct _ta *parse_file(buffer *mbuf, char *c, struct _lfile *);

/** Function Prototype
        char* aux(buffer* mbuf, char*c);

Functionality: This function will eventually add the auxillary portion
        of the specifcation file to the source code of the scanner
        generated.

Parameters: buffer* mbuf, the existing buffer being used for the input,
        and the existing char* pointer to a character being used.

Return Value: Returns a pointer to the leading character of input

Results: The auxilary RAW C code is read and stored into a data
        structure to be copied into the lexer later on.
*/
char *aux(buffer *mbuf, char *c);

/** Function Prototype
        void display_memstats(void);

Functionality: Debug Function to display memory usage statistics by
        program termination.

Results: The memory statistics are printed to standard out.
*/
void display_memstats(void);

/** Function Prototype
        void cleanup_lex(buffer** mbuf,struct _lfile* lexfile,struct _DFA**
dfa); Functionality: Memory Cleanup before program termination. Anything the
main driver function creates is released as well as any othe live structure
        constructed during the lifecyle of the program.

Parameters: bufer** mbuf, the address to the buffer that was used during
        execution, a pointer to the lexfile structure _lfile* holding a pointer
        to the FILE opened, and memory address to the DFA constructed.

Results: All memory and FILE pointers are released back to the Operating
        System.
*/
void cleanup_lex(buffer **mbuf, struct _lfile *lexfile, struct _DFA **dfa);

/**
        Main Driver Function

Functionality: takes in the specification file, opens it and then starts
        to parse and seperate the individual comoponents of the file. (The
format of the file is shown below.) After creating a parse tree of the input,
        the regular expressions are used to construct a DFA that recognizes any
        of the strings generated by the regular expressions in the spec file.
        After the DFA is created, a C source code file is output with a
        lexer for this specific DFA.
                The code associated with each regular expression will be run
when the attached lexer is compiled and run on an input stream and a token is
recognized that matches one of the regular expressions constructed.

Specification File Format
-----------------------------------------------------------
%{		<-- symbolizes the start of the declarations section

C declarations and code or headers to include in the produced code

%}		<-- end of declarations

short-hand definitions for regular expressions

%%		<-- next section

regular expression rules and their code

regular expression rule {

                C code to run when the regular expression is matched
                        enclosed in curly brackets '{' and '}'

                                                }

%%		<-- next section
auxilary functions and code to include in the produced scanner

END OF FILE
-----------------------------------------------------------
*/
int main(int argc, const char **argv) {
  /*  lexfile structure to hold FILE pointer to .l specification file that will
   * be used*/
  struct _lfile lexfile;
  /*  buffer pointer used for buffer of .l file opened*/
  static buffer *mbuf;
  /* DFA structure pointer to point to DFA record created with this lexer */
  struct _DFA *dfa;

  /* used as current character on input and incremented with getchar routine */
  static char c;

  /* UNUSED */
  /* currently stores result after running the created DFA on the opened source
   * file */
  /*   int result;*/

  /* Initialize Variables */
  c = '\0';
  dfa = NULL;
  mbuf = NULL;
  lexfile.decs = NULL;
  lexfile.fpos = NULL;
  lexfile.tree = NULL;

/* looks for 1 filename on the command line
   for the .l spec file if it isn't given then produce an
        error and exit */
#define ARGUMENTS_NEEDED 2
  if (argc != ARGUMENTS_NEEDED) {
    lex_error(SCERR_NEED_ARGUMENT);
    exit(EXIT_FAILURE);
  }

/* initialize buffer from filename given */
#define ARGUMENT_IS_FILENAME 1
  mbuf = buffer_from_filename(argv[ARGUMENT_IS_FILENAME]);

  /* If the file is a .l file, this will start to parse the
   file with a recursive predictive parser */
  parse_file(mbuf, &c, &lexfile);

  /*
                  Check to see if the parse tree is constructed and if the
     firstpos sets are constructed for the tree, if not then cleanly exit and do
          nothing else.
                   If they are, then generate the DFA and output it to the new
     lexer file. lexfile.tree holds the parse tree and lexfile.fpos holds the
     firstpos sets
  */
  if (lexfile.tree && lexfile.fpos) {

    /* generate_dfa takes the parse tree, firstpos sets,and constructed alphabet
                    from the regular expressions as input*/
    dfa = generate_dfa(lexfile.tree, lexfile.fpos, lexfile.tree->alphabet);
    generate_output(lexfile, dfa);
  }

  /* Cleanup and release memory and file buffers, and exit */
  cleanup_lex(&mbuf, &lexfile, &dfa);
  /*    display_memstats();*/
  return EXIT_SUCCESS;
}

/*
        struct _ta *parse_file(buffer * mbuf,char *c, struct _lfile * file)

Functionality: Parse the input specification file into its components
        by using the pointers to the memory buffer mbuf and the input pointer
        c. Creates a parse tree of the regular expressions parsed and stores
        in other data structures the definitions, declarations, and C code
        to be included into the lexer.

*/
#define pass_ws()                                                              \
  while ((is_ws(*current_char) == 0) || *current_char == '\n')                 \
    *current_char = getchar(mbuf);
#define check_for(sign, error)                                                 \
  if (*current_char != sign) {                                                 \
    lex_error(error);                                                          \
    return NULL;                                                               \
  }

struct _ta *parse_file(buffer *mbuf, char *current_char, struct _lfile *file) {
  /* If not at and of file already, look for spec file section delimiters.
          if they aren't there then return an error and exit
  */
  if (*current_char != EOF && (*current_char = getchar(mbuf)) != EOF) {
    check_for('%', SCERR_DECL_UNDECLARED);
    *current_char = getchar(mbuf);
    check_for('{', SCERR_DECL_UNDECLARED);
    /* Parse the declarations section of the spec file and store
               the short-hand declarations to use later
    */
    file->decs = declarations(mbuf, current_char, file);
    /* Look for delimiters or exit with an error */
    pass_ws();
    check_for('%', SCERR_MUST_USE_SEPR);
    *current_char = getchar(mbuf);
    check_for('%', SCERR_MUST_USE_SEPR);
    *current_char = getchar(mbuf);
    pass_ws();
    *current_char = ungetchar(mbuf);

    /* Parse the regular expressions and return the parse tree
               created of all the regular expressions combined
    */
    file->tree = translations(mbuf, current_char, file);

    /* Look for delimiters or exit with an error */
    pass_ws();
    check_for('%', SCERR_SEPR_AFTER_TRANS);
    *current_char = getchar(mbuf);
    check_for('%', SCERR_SEPR_AFTER_TRANS);
  }
  return NULL;
}
#undef pass_ws
#undef check_for

/*
        char* aux(buffer* mbuf, char*c){

Functionality; To include all the code found in the auxilary
        portion of the specification file. Currently its unimplemented
*/
char *aux(buffer *mbuf, char *c) { return NULL; }

/* DEBUG Function for displaying memory usage */
void display_memstats(void) {
  int tnm = tnum_created();
  printf("+--------------------------------------------------------------------"
         "-------------------+\n");
  printf("+ value		|	ta	|	t	|	"
         "sa	|	s		+\n");
  printf("+ created	|	%d	|	%d	|	%d	"
         "|	%d		+\n",
         atnum_created(), tnm, asnum_created(), snum_created());
  printf("+ deleted	|	%d	|	%d	|	%d	"
         "|	%d		+\n",
         atnum_deleted(), tnum_deleted(), asnum_deleted(), snum_deleted());
  printf("+Total Loss	|	%d	|	%d	|	%d	"
         "|	%d		+\n",
         atnum_created() - atnum_deleted(), tnm - tnum_deleted(),
         asnum_created() - asnum_deleted(), snum_created() - snum_deleted());
  printf("+--------------------------------------------------------------------"
         "-------------------+\n");
  printf("*note - ta is tree arrays, t are tree nodes, sa are set \n");
  printf("*note - arrays, and s are sets themselvese\n\n");
}

/*
        void cleanup_lex(buffer** mbuf,struct _lfile* lexfile,struct _DFA**
dfa){

Functionality: Clean up and release all memory used and closing and releasing
        all files that were opened and pointers attained
*/
void cleanup_lex(buffer **mbuf, struct _lfile *lexfile, struct _DFA **dfa) {
    int y;
  /* release memory buffer */
  delete_buffer(*mbuf);
  /* release declarations portion of spec file */
  free((*lexfile).decs);
  (*lexfile).decs = NULL;
  /* release included C code attached to each regular expression definition */
  for (y = 0; y < (*lexfile).num_defs; y++) {
    delete_buffer((*lexfile).defbuf[y]);
    (*lexfile).defbuf[y] = NULL;
  }
  free((*lexfile).defbuf);
  (*lexfile).defbuf = NULL;
  /* release parse tree */
  delete_ta((*lexfile).tree);
  /* release firstpos sets */
  delete_iseta((*lexfile).fpos);
  /* release DFA table constructed */
  delete_dfa(*dfa);
  *dfa = NULL;
  (*lexfile).fpos = NULL;
  (*lexfile).tree = NULL;
  *mbuf = NULL;
}
