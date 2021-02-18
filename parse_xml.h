#ifndef _PARSE_XML_H
#define _PARSE_XML_H

size_t parse_xml(const char *filename);
void start(void *data, const char *el, const char **attr);
void end(void *data, const char *el);

#endif   //_PARSE_XML_H
