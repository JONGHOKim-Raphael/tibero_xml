#include <stdio.h>
#include <stdlib.h>   // exit()
#include <sqlcli.h>
#include "insert_xml.h"

extern int        tibero_connected;
static SQLHENV    henv               = SQL_NULL_HENV;
static SQLHDBC    hdbc               = SQL_NULL_HDBC;
static SQLHSTMT   hstmt              = SQL_NULL_HSTMT;


#define BUFFSIZE   1024


void connect_tibero() {
  SQLRETURN  ret    = SQL_SUCCESS;
  int        i      = 0;
  char       dbname[BUFFSIZE], user[BUFFSIZE], password[BUFFSIZE];
  FILE      *fp;

  // Read login information from login_info.txt
  fp = fopen("login_info.txt", "r");
  fscanf(fp, "dbname: %s\nuser: %s\npassword: %s", dbname, user, password);
  fclose(fp);

  ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
  if(ret != SQL_SUCCESS) {
    fprintf(stderr, "Error allocating SQL handle");
    exit(1);
  }

  //ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER *)SQL_OV_ODBC3, 0);
  ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
  ret = SQLConnect(hdbc, (SQLCHAR *)dbname, SQL_NTS, (SQLCHAR *)user, SQL_NTS, (SQLCHAR *)password, SQL_NTS);
  tibero_connected = 1;
  ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
}


void disconnect_tibero() {

  SQLRETURN ret    = SQL_SUCCESS;

  ret = SQLDisconnect(hdbc);
  ret = SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
  ret = SQLFreeHandle(SQL_HANDLE_ENV, henv);

  tibero_connected = 0;
}


void insert_xml(const char *xmltext) {
}
