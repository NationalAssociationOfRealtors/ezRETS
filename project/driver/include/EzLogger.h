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
#ifndef EZ_LOGGER_H
#define EZ_LOGGER_H

#include <string>
#include "ezretsfwd.h"

namespace odbcrets
{
class EzLogger
{
  public:
    enum Level
    {
        DEBUG,
        INFO,
        WARN,
        ERRORS
    };

    EzLogger();

    virtual ~EzLogger();

    void setLogLevel(Level level);

    virtual void log(Level level, std::string data) = 0;

    void debug(std::string data);
    void info(std::string data);
    void warn(std::string data);
    void error(std::string data);

  protected:
    Level mLevel;
};

class NullEzLogger : public EzLogger
{
  public:
    static EzLoggerPtr GetInstance();

    void log(Level level, std::string data);

  private:
    static EzLoggerPtr sInstance;
};

}

#endif /* EZ_LOGGER_H */

/* Local Variables: */
/* mode: c++ */
/* End: */
