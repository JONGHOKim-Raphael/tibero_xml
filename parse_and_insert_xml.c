#include <stdio.h>
#include <stdlib.h>   // atol()
#include <string.h>
#include <expat.h>
#include "parse_and_insert_xml.h"
#include "insert_xml.h"
#include "debug.h"

#define FOUR_GIBIBYTE             4294967296   // 4 GiB
#define TIBERO6_XMLTYPE_MAXSIZE   FOUR_GIBIBYTE
#define LARGE_BUFFSIZE            FOUR_GIBIBYTE
#define BUFFSIZE                        8192   // 8 KiB

// Progress
#define PROGRESS_UNIT            100

//
#define SPACE                    "  "
#define MEDIAWIKI                "mediawiki"
#define MEDIAWIKI_SUFFIX         "\n</mediawiki>"
#define SITEINFO                 "siteinfo"
#define SITEINFO_SUFFIX          "</siteinfo>\n" SPACE
#define TITLE                    "title"
#define TITLE_TAGLEN             strlen("<title>")
#define PAGE                     "page"
#define PAGE_SUFFIX              "</page>"
#define TEXT                     "text"
#define TEXT_ENDTAGLEN           strlen("</text>\n")
#define BYTES                    "bytes"


/* Keep track of the current level in the XML tree */
static size_t          count = 0;
static size_t          prefixlen, max_pagelen;
static long            prefix_offset_start,
                       prefix_offset_end,
                       page_offset_start,
                       page_offset_end,
                       title_offset_start,
                       title_offset_end,
                       text_offset_start,
                       text_offset_end;
//static long            text_bytes;
static XML_Parser      parser;
static char           *xmltext;
static char           *xml_buff;


// Read-only pointers.
// The prefix_buff stores <mediawiki> to </siteinfo>. Need </mediawiki>
static char           *prefix_buff, *page_buff, *title;


void start(void *data, const char *el, const char **attr) {
  int             i;

  if(!strcmp(MEDIAWIKI, el))
    prefix_offset_start   = XML_GetCurrentByteIndex(parser);

  if(!strcmp(PAGE, el)) {
    ++count;
    //text_bytes = 0;
    page_offset_start     = XML_GetCurrentByteIndex(parser);
    text_offset_start     = page_offset_start;
  }

  if(!strcmp(TITLE, el))
    title_offset_start    = XML_GetCurrentByteIndex(parser);

  if(!strcmp(TEXT, el)) {
    text_offset_start     = XML_GetCurrentByteIndex(parser);

    /*
    for(i=0; attr[i]; i+=2) {
      if(!strcmp(BYTES, attr[i])) {
        printf("\nbytes: %s", attr[i+1]);
        text_bytes = atol(attr[i+1]);
      }
    }
    */
  }
}               /* End of start handler */


void end(void *data, const char *el) {

  if(!strcmp(SITEINFO, el)) {
    prefix_offset_end = XML_GetCurrentByteIndex(parser);

    prefixlen = prefix_offset_end - prefix_offset_start;
    max_pagelen = LARGE_BUFFSIZE - prefixlen;
    strncpy(prefix_buff,
            xmltext + prefix_offset_start,
            prefixlen);

    strcat(prefix_buff, SITEINFO_SUFFIX);
    page_buff = prefix_buff + strlen(prefix_buff);
  }

  if(!strcmp(PAGE, el)) {
    if(count > 5) {
      disconnect_tibero();
      exit(0);
    }

    // We should show progress
    if(count % PROGRESS_UNIT == 0 && count != 0)
      fprintf(stderr, "\n%zd rows inserted", count);

    page_offset_end = XML_GetCurrentByteIndex(parser);

    // If <text> tag does not exist, length of offset should be 0
    if(page_offset_start > text_offset_end) {
      text_offset_start   = page_offset_start;
      text_offset_end     = page_offset_start;
      goto copy_page_after_text;
    }

    DEBUG_INFO("\n--- OFFSETS ---");
    DEBUG_INFO("\npage_offset_start: %ld", page_offset_start);
    DEBUG_INFO("\ntext_offset_start: %ld", text_offset_start);
    DEBUG_INFO("\ntext_offset_end:   %ld", text_offset_end);
    DEBUG_INFO("\npage_offset_end:   %ld", page_offset_end);
    DEBUG_INFO("\n---------------\n");

    strncpy(page_buff,
            xmltext + page_offset_start,
            text_offset_start - page_offset_start);

copy_page_after_text:

    strncpy(page_buff + text_offset_start - page_offset_start,
            xmltext + text_offset_end,
            page_offset_end - text_offset_end);

    strcat(xml_buff, PAGE_SUFFIX);
    strcat(xml_buff, MEDIAWIKI_SUFFIX);

    // Insert XML using tbCLI
    insert_xml(count, title, xml_buff);
    //insert_xml(count, title, page_buff);

    // Free the page buffer and title buffer
    memset(page_buff, '\0', max_pagelen);
    memset(title, '\0', BUFFSIZE);
  }

  if(!strcmp(TITLE, el)) {
    title_offset_end = XML_GetCurrentByteIndex(parser);
    strncpy(title,
            xmltext          + title_offset_start + TITLE_TAGLEN,
            title_offset_end - title_offset_start - TITLE_TAGLEN);

    DEBUG_INFO("\n%zd-th TITLE: %s", count, title);
  }

  if(!strcmp(TEXT, el)) {
    text_offset_end = XML_GetCurrentByteIndex(parser) + TEXT_ENDTAGLEN;
  }
}               /* End of end handler */


size_t parse_and_insert_xml(const char *filename) {

  FILE           *f;
  size_t          size;

  xmltext       = malloc(LARGE_BUFFSIZE);

  // Initialize static variables
  xml_buff           = malloc(LARGE_BUFFSIZE);
  memset(xml_buff, '\0', LARGE_BUFFSIZE);
  prefix_buff        = xml_buff;
  title              = malloc(BUFFSIZE);
  memset(title, '\0', BUFFSIZE);

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
  free(title);

  return size;
}
