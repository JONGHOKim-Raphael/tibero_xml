/*
   A simple test program to parse XML documents with expat
   <http://expat.sourceforge.net/>. It just displays the element
   names.

   On Debian, compile with:

   gcc parse_expat.c -lexpat

   Inspired from <http://www.xml.com/pub/a/1999/09/expat/index.html>
*/

#include <stdio.h>
#include "parse_xml.h"


int main(int argc, char **argv) {

    char           *filename;
    size_t          size;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        return (1);
    }

    filename = argv[1];

    size = parse_xml(filename);

    fprintf(stdout, "Successfully parsed %li characters in file %s\n", size, filename);
    return (0);
}
