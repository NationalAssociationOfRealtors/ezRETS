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

#ifndef EZRETS_SETUP_H
#define EZRETS_SETUP_H

#include "ezrets.h"
#include "commonfwd.h"
#include <string>
#include <map>

namespace odbcrets {

class Setup
{
  public:
    Setup();
    void ConfigDSN(HWND parent, WORD request, LPCSTR driver,
                   LPCSTR attributes);

  private:
    typedef std::map<std::string, std::string> AttributeMap;
    AttributeMap att2map(LPCSTR attributes);
    void ConfigDsn(UWORD configMode);
    void AddDsn(UWORD configMode);
    void RemoveDsn();
    int ShowDialog(DataSourcePtr dataSource);

    HWND mParent;
    std::string mDriver;
    AttributeMap mAttributes;
};

}

#endif

/* Local Variables: */
/* mode: c++ */
/* End: */
