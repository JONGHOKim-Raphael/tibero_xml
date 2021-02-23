#include <stdio.h>
#include <string.h>
#include <expat.h>
#include "parse_and_insert_xml.h"
#include "insert_xml.h"
#include "debug.h"

#define FOUR_GIBIBYTE             4294967296   // 4 GiB
#define TIBERO6_XMLTYPE_MAXSIZE   FOUR_GIBIBYTE
#define LARGE_BUFFSIZE            FOUR_GIBIBYTE
#define BUFFSIZE                        8192   // 8 KiB

//
#define SPACE                    "  "
#define MEDIAWIKI                "mediawiki"
#define MEDIAWIKI_SUFFIX         "\n</mediawiki>"
#define SITEINFO                 "siteinfo"
#define SITEINFO_SUFFIX          "</siteinfo>\n" SPACE
#define PAGE                     "page"
#define PAGE_SUFFIX              "</page>"


/* Keep track of the current level in the XML tree */
static int             depth = 0;
static size_t          count = 0;
static long            prefix_offset_start,
                       prefix_offset_end,
                       page_offset_start,
                       page_offset_end;
static XML_Parser      parser;
static char           *xmltext;
static char           *xml_buff;
// Read-only pointers.
// The prefix_buff stores <mediawiki> to </siteinfo>. Need </mediawiki>
static char           *prefix_buff, *page_buff;


void start(void *data, const char *el, const char **attr) {
  int             i;

  /*
  for (i = 0; i < depth; i++)
    printf("%s", SPACE);

  printf("%s", el);

  for (i = 0; attr[i]; i += 2) {
    printf(" %s='%s'", attr[i], attr[i + 1]);
  }

  printf("\n");

  depth++;
  */

  if(!strcmp(MEDIAWIKI, el))
    prefix_offset_start = XML_GetCurrentByteIndex(parser);

  if(!strcmp(PAGE, el))
    page_offset_start = XML_GetCurrentByteIndex(parser);
}               /* End of start handler */


void end(void *data, const char *el) {
  /*
  depth--;
  */

  size_t page_len;

  if(!strcmp(SITEINFO, el)) {
    prefix_offset_end = XML_GetCurrentByteIndex(parser);

    strncpy(prefix_buff,
            xmltext + prefix_offset_start,
            prefix_offset_end - prefix_offset_start);

    strcat(prefix_buff, SITEINFO_SUFFIX);
    page_buff = prefix_buff + strlen(prefix_buff);
  }

  if(!strcmp(PAGE, el)) {

    ++count;
    page_offset_end = XML_GetCurrentByteIndex(parser);

    page_len = page_offset_end - page_offset_start;

    strncpy(page_buff,
            xmltext + page_offset_start,
            page_len);

    strcat(xml_buff, PAGE_SUFFIX);
    strcat(xml_buff, MEDIAWIKI_SUFFIX);
    if(count < 10)
      DEBUG_INFO("\nXML WITH A PAGE\n%s\n", xml_buff);

    // Free the page buffer
    memset(page_buff, '\0', page_len);
  }
}               /* End of end handler */


size_t parse_and_insert_xml(const char *filename) {

  FILE           *f;
  size_t          size;

  xmltext       = malloc(LARGE_BUFFSIZE);

  // Initialize static variables
  xml_buff           = malloc(LARGE_BUFFSIZE);
  xml_buff[0]        = '\0';
  prefix_buff        = xml_buff;

  parser = XML_ParserCreate(NULL);
  if (parser == NULL) {
    fprintf(stderr, "Parser not created\n");
    return (1);
  }

  // Connect to Tibero
  connect_tibero();

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

  // Disconnect Tibero
  disconnect_tibero();

  XML_ParserFree(parser);

  DEBUG_INFO("\n\nTotal %zd pages\n", count);
  free(xmltext);
  free(xml_buff);

  return size;
}
