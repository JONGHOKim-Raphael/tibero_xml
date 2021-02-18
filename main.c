/*
   A simple test program to parse XML documents with expat
   <http://expat.sourceforge.net/>. It just displays the element
   names.

   On Debian, compile with:

   gcc parse_expat.c -lexpat

   Inspired from <http://www.xml.com/pub/a/1999/09/expat/index.html>
*/

#include <expat.h>
#include <stdio.h>
#include <string.h>
#include "parse_xml.h"

#define MAXCHARS 1000000000


int main(int argc, char **argv) {

    char           *filename;
    FILE           *f;
    size_t          size;
    char           *xmltext;
    XML_Parser      parser;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        return (1);
    }
    filename = argv[1];
    parser = XML_ParserCreate(NULL);
    if (parser == NULL) {
        fprintf(stderr, "Parser not created\n");
        return (1);
    }
    /* Tell expat to use functions start() and end() each times it encounters
     * the start or end of an element. */
    XML_SetElementHandler(parser, start, end);
    f = fopen(filename, "r");
    xmltext = malloc(MAXCHARS);
    /* Slurp the XML file in the buffer xmltext */
    size = fread(xmltext, sizeof(char), MAXCHARS, f);
    if (XML_Parse(parser, xmltext, strlen(xmltext), XML_TRUE) ==
        XML_STATUS_ERROR) {
        fprintf(stderr,
            "Cannot parse %s, file may be too large or not well-formed XML\n",
            filename);
        return (1);
    }
    fclose(f);
    XML_ParserFree(parser);
    fprintf(stdout, "Successfully parsed %li characters in file %s\n", size,
        filename);
    return (0);
}
