#include <stdio.h>
#include <stdlib.h>   // exit()
#include <sqlcli.h>
#include "insert_xml.h"
#include "debug.h"

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
  SQLCHAR   *create;

  create = "CREATE TABLE UK( UK_ID NUMBER PRIMARY KEY, KNOWLEDGE sys.XMLType )";

  // Read login information from login_info.txt
  fp = fopen("login_info.txt", "r");
  fscanf(fp, "dbname: %s\nuser: %s\npassword: %s", dbname, user, password);
  DEBUG_INFO("dbname: %s user: %s password: %s", dbname, user, password);
  fclose(fp);

  ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
  if(ret != SQL_SUCCESS) {
    fprintf(stderr, "Error allocating SQL handle");
    exit(1);
  }

  ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER *)SQL_OV_ODBC3, 0);
  ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
  ret = SQLConnect(hdbc, (SQLCHAR *)dbname, SQL_NTS, (SQLCHAR *)user, SQL_NTS, (SQLCHAR *)password, SQL_NTS);

  // Tibero 6 is connected
  tibero_connected = 1;
  DEBUG_INFO("\n=== Tibero 6 connected ===\n");

  //
  ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
  ret = SQLExecDirect(hstmt, create, SQL_NTS);
}


void disconnect_tibero() {

  SQLRETURN ret    = SQL_SUCCESS;

  ret = SQLDisconnect(hdbc);
  ret = SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
  ret = SQLFreeHandle(SQL_HANDLE_ENV, henv);
  //ret = SQLFreeHandle(?, hstmt);

  // Tibero 6 is disconnected
  tibero_connected = 0;
  DEBUG_INFO("\n=== Tibero 6 disconnected ====\n");
}


void insert_xml(const long uk_id, const char *xmltext) {

  SQLRETURN   ret    = SQL_SUCCESS;
  SQLCHAR    *insert = "INSERT INTO UK VALUES(?, sys.XMLType.createXML(?))";


  /*
  ret = SQLBindParameter(hstmt,             //   StatementHandle
                         1,                 //   ParameterNumber
                         SQL_PARAM_INPUT,   //   InputOutputType
                         SQLINTEGER,        //   ValueType
                         SQLINTEGER,        //   ParameterType
                         0,                 //   ColumnSize
                         0,                 //   DecimalDigits
                         &uk_id,            //   ParameterValue
                         255,               //   BufferLength
                         NULL);             //  *StrLen_or_IndPtr

  ret = SQLBindParameter(hstmt,             //   StatementHandle
                         2,                 //   ParameterNumber
                         SQL_PARAM_INPUT,   //   InputOutputType
                         SQL_C_CHAR,        //   ValueType
                         SQLCHAR,           //   ParameterType
                         0,                 //   ColumnSize
                         0,                 //   DecimalDigits
                         xmltext,           //   ParameterValue
                         strlen(xmltext),   //   BufferLength
                         SQL_NTS);          //  *StrLen_or_IndPtr
  */


  ret = SQLPrepare(hstmt, insert, SQL_NTS);

  SQLExecute(hstmt);
}
