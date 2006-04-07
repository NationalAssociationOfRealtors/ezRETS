/*
 * Copyright (C) 2006 National Association of REALTORS(R)
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
#ifndef DATASTREAMINFO_H
#define DATASTREAMINFO_H

#include "ezrets.h"
#include <cstddef>

namespace odbcrets
{
class DataStreamInfo
{
  public:
    enum FileStatus { NO_DATA_RETRIEVED, HAS_MORE_DATA, NO_MORE_DATA };

    DataStreamInfo();

    void reset();

    SQLUSMALLINT column;
    FileStatus status;
    std::size_t offset;
};

}

#endif /* DATASTREAMINFO_H */

/* Local Variables: */
/* mode: c++ */
/* End: */

