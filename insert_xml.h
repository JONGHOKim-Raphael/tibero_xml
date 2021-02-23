#ifndef _INSERT_XML_H
#define _INSERT_XML_H

static int tibero_connected = 0;
void connect_tibero();
void disconnect_tibero();
void insert_xml(int uk_id, char *xmltext);


#endif   // _INSERT_XML_H
