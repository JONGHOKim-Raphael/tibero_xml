#include <stdio.h>
#include "parse_xml.h"

/* Keep track of the current level in the XML tree */
static int         depth = 0;


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
