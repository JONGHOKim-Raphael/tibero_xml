#include <stdio.h>
#include <string.h>
#include <expat.h>
#include "parse_xml.h"

#define FOUR_GIBIBYTE             4294967296   // 4 GiB
#define TIBERO6_XMLTYPE_MAXSIZE   FOUR_GIBIBYTE
#define LARGE_BUFFSIZE            FOUR_GIBIBYTE
#define BUFFSIZE                        8192   // 8 KiB

//
#define SITE_INFO   "siteinfo"
#define PAGE        "page"


/* Keep track of the current level in the XML tree */
static int             depth = 0;
static size_t          count = 0;
static long            siteinfo_offset_start,
                       siteinfo_offset_end,
                       page_offset_start,
                       page_offset_end;
static XML_Parser      parser;
static char           *xmltext,
                      *siteinfo_buff,
                      *page_buff;

void start(void *data, const char *el, const char **attr) {
  int             i;

  for (i = 0; i < depth; i++)
    printf("  ");

  if(!strcmp(SITE_INFO, el))
    siteinfo_offset_start = XML_GetCurrentByteIndex(parser);

  if(!strcmp(PAGE, el))
    page_offset_start = XML_GetCurrentByteIndex(parser);

  printf("%s", el);

  for (i = 0; attr[i]; i += 2) {
    printf(" %s='%s'", attr[i], attr[i + 1]);
  }

  printf("\n");
  depth++;
}               /* End of start handler */


void end(void *data, const char *el) {
  depth--;

  if(!strcmp(SITE_INFO, el)) {
    siteinfo_offset_end = XML_GetCurrentByteIndex(parser);

    strncpy(siteinfo_buff,
            xmltext + siteinfo_offset_start,
            siteinfo_offset_end - siteinfo_offset_start);

    strcat(siteinfo_buff, "</siteinfo>");
    //TODO: Detect <mediawiki> and </mediawiki>
  }

  if(!strcmp(PAGE, el)) {

    ++count;
    page_offset_end = XML_GetCurrentByteIndex(parser);

    strncpy(page_buff,
            xmltext + page_offset_start,
            page_offset_end - page_offset_start);

    strcat(page_buff, "</page>");

    //printf("\nPAGE\n%s\n", page_buff);

    // Free the page buffer
    page_buff[0] = '\0';
  }
}               /* End of end handler */


size_t parse_xml(const char *filename) {

  FILE           *f;
  size_t          size;

  xmltext       = malloc(LARGE_BUFFSIZE);

  // Initialize static variables
  siteinfo_buff      = malloc(LARGE_BUFFSIZE);
  siteinfo_buff[0]   = '\0';
  page_buff          = malloc(LARGE_BUFFSIZE);
  page_buff[0]       = '\0';

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
  size = fread(xmltext, sizeof(char), LARGE_BUFFSIZE, f);
  if (XML_Parse(parser, xmltext, strlen(xmltext), XML_TRUE) ==
      XML_STATUS_ERROR) {
    fprintf(stderr,
            "Cannot parse %s, file may be too large or not well-formed XML\n",
            filename);
    return (1);
  }

  fclose(f);
  XML_ParserFree(parser);

  printf("\nSITEINFO\n%s\n", siteinfo_buff);
  printf("\n\nTotal %zd pages\n", count);

  free(xmltext);
  free(siteinfo_buff);

  return size;
}
