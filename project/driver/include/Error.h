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
#ifndef ERROR_H
#define ERROR_H

#include <string>

namespace odbcrets
{
class Error
{
  public:
    Error(std::string sqlstate, std::string message);

    std::string getSqlstate();
    std::string getMessage();

    std::string toString();

  private:
    std::string mSqlstate;
    std::string mMessage;
};

}


#endif /* ERROR_H */

/* Local Variables: */
/* mode: c++ */
/* End: */
