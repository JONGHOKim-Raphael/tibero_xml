Tibero-XML
==
What this project do?
--
1. Parse large wikipedia XML file into multiple XML files by page
2. Insert XML files into Tibero 6

How to build?
--
Install dependencies
 * expat: An XML parsing library
 
~~~shell
git clone https://github.com/libexpat/libexpat
cd libexpat/expat
./buildconf.sh
./configure
make
sudo make install 
~~~


Build this project:
~~~shell
make
~~~



How to use?
--
Because XML files for tests are large,
you should download XML files on your own.

If you have XML files for tests,
just build and run:
~~~shell
./tibero_xml <file name>
~~~
