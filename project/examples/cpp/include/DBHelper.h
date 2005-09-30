/*
 * Copyright (C) 2005 National Association of REALTORS(R)
 *
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, and/or sell copies of the
 * Software, and to permit persons to whom the Software is furnished
 * to do so, provided that the above copyright notice(s) and this
 * permission notice appear in all copies of the Software and that
 * both the above copyright notice(s) and this permission notice
 * appear in supporting documentation.
 */
#ifndef DBHELPER_H
#define DBHELPER_H

#include "example_sql.h"
#include <string>

#include "ResultColumn.h"

namespace odbcrets
{
namespace test
{

class DBHelper
{
  public:
    DBHelper();

    std::string driverConnect(std::string conString);
    
    void connect(std::string dsn, std::string user = "",
                 std::string passwd = "");
    void disconnect();

    std::string executeQuery(std::string query);
    std::string describeColumn(int num);

    void bindColumn(int num, ResultColumnPtr col);

    bool fetch();

    void getData(int num, ResultColumnPtr col);

    int numResultCols();

    void tables(std::string table = "");
    void columns(std::string table);

    int rowCount();

    void setStmtAttr(SQLINTEGER Attribute, SQLPOINTER Value,
                     SQLINTEGER StringLength);

    void primaryKeys(std::string tableName);

  private:
    void handleResult(SQLSMALLINT HandleType, SQLHANDLE Handle,
                      SQLRETURN result);
    void freeStmt();
    void allocEnvAndDbc();
    void allocStmt();

    SQLHENV mEnv;
    SQLHDBC mDbc;
    SQLHSTMT mStmt;
};

}
}

#endif /* DBHELPER_H */

/* Local Variables: */
/* mode: c++ */
/* End: */
