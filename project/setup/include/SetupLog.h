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

#ifndef EZRETS_SETUP_LOG_H
#define EZRETS_SETUP_LOG_H

#include "ezrets.h"
#include "commonfwd.h"
#include <string>
#include <map>

#include <fstream>

namespace odbcrets {

class SetupLog
{
  public:
    SetupLog()
    {
        mDebugEnabled = false;
    }

    void enableDebug(bool enabled = true)
    {
        mDebugEnabled = enabled;
        if (mDebugEnabled)
        {
            out.open(LOGFILE);
        }
    }
    
    void debug(std::string message)
    {
        if (mDebugEnabled)
        {
            out << message << std::endl;
        }
    }

  private:
    bool mDebugEnabled;
    static const char * LOGFILE;
    std::ofstream out;
};

extern SetupLog slog;

}

#endif

/* Local Variables: */
/* mode: c++ */
/* End: */
