/*
 * Copyright (C) 2005,2006 National Association of REALTORS(R)
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
#ifndef SQLSTATEEXCEPTION_H
#define SQLSTATEEXCEPTION_H

#include "EzRetsException.h"

namespace odbcrets
{

class SqlStateException : public EzRetsException
{
  public:
    SqlStateException(std::string sqlstate, std::string message) throw();

    virtual SqlStateException::~SqlStateException() throw();

    virtual std::string GetSqlState() const throw();

  private:
    std::string mSqlState;
};

}

#endif /* SQLSTATEEXCEPTION_H */

/* Local Variables: */
/* mode: c++ */
/* End: */
