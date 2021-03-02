#!/bin/bash
URL=https://dumps.wikimedia.org/enwiki/20210201/enwiki-20210201-pages-articles-multistream.xml.bz2
ZIPFILE=data/enwiki-20210201-pages-articles-multistream.xml.bz2
XMLFILE=data/enwiki-20210201-pages-articles-multistream.xml

CMD1="curl -o $ZIPFILE $URL"
CMD2="bzip2 -dk $ZIPFILE > $XMLFILE"

echo $0: $CMD1
$($CMD1)

echo $0: $CMD2
$($CMD2)
