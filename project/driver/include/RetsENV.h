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
#ifndef RETSENV_H
#define RETSENV_H

#include <iosfwd>
#include <list>
#include <boost/shared_ptr.hpp>
#include "ezretsfwd.h"
#include "AbstractHandle.h"

namespace odbcrets
{
class RetsENV : public AbstractHandle
{
  public:
    RetsENV();
    ~RetsENV();

    SQLRETURN SQLFreeConnect(RetsDBC* dbc);

    SQLRETURN SQLAllocConnect(SQLHDBC *ConnectionHandle);
    SQLRETURN SQLGetEnvAttr(SQLINTEGER Attribute, SQLPOINTER Value,
                            SQLINTEGER BufferLength, SQLINTEGER *StringLength);
    SQLRETURN SQLSetEnvAttr(SQLINTEGER Attribute, SQLPOINTER Value,
                            SQLINTEGER StringLength);

    EzLoggerPtr getLogger();
    void setEzLogFile(std::string logFile);

    bool canBeFreed();
    
  private:
    SQLINTEGER odbcVersion;
    std::list<RetsDBC*> mDatabaseConnections;
    EzLoggerPtr mLogger;
    boost::shared_ptr<std::ostream> mLogFile;
};
}
#endif /* RETSENV_H */

/* Local Variables: */
/* mode: c++ */
/* End: */
