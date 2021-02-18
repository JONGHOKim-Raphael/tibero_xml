#include <stdio.h>
#include <string.h>
#include <expat.h>
#include "parse_xml.h"

#define FOUR_GIBIBYTE 4294967296   //  4 GiB
#define TIBERO6_XMLTYPE_MAXSIZE   FOUR_GIBIBYTE   // 4 GiB
#define BUFFSIZE   FOUR_GIBIBYTE


/* Keep track of the current level in the XML tree */
static int         depth = 0;


size_t parse_xml(const char *filename) {

  FILE           *f;
  size_t          size;
  char           *xmltext;
  XML_Parser      parser;

  xmltext=malloc(BUFFSIZE);

  parser = XML_ParserCreate(NULL);
  if (parser == NULL) {
    fprintf(stderr, "Parser not created\n");
    return (1);
  }

  /* Tell expat to use functions start() and end() each times it encounters
   * the start or end of an element. */
  XML_SetElementHandler(parser, start, end);
  f = fopen(filename, "r");

  /* Slurp the XML file in the buffer xmltext */
  size = fread(xmltext, sizeof(char), BUFFSIZE, f);
  if (XML_Parse(parser, xmltext, strlen(xmltext), XML_TRUE) ==
      XML_STATUS_ERROR) {
    fprintf(stderr,
            "Cannot parse %s, file may be too large or not well-formed XML\n",
            filename);
    return (1);
  }

  fclose(f);
  XML_ParserFree(parser);

  free(xmltext);

  return size;
}

void start(void *data, const char *el, const char **attr) {
    int             i;

    for (i = 0; i < depth; i++)
        printf("  ");

    printf("%s", el);

    for (i = 0; attr[i]; i += 2) {
        printf(" %s='%s'", attr[i], attr[i + 1]);
    }

    printf("\n");
    depth++;
}               /* End of start handler */

void end(void *data, const char *el) {
    depth--;
}               /* End of end handler */
