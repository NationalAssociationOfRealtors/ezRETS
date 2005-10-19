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
#include "utils.h"
#include <vector>
#include <boost/format.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "str_stream.h"

using std::string;
namespace b = boost;
namespace o = odbcrets;
/**
 * Copies from std::string into a c string, null terminiating the
 * string.
 */ 
size_t o::copyString(string src, char* dest, size_t length)
{
    if ((dest == NULL) || (length == 0))
    {
        return 0;
    }
    size_t size = src.copy(dest, length);

    if (size == length)
    {
        dest[size - 1] = '\0';
    }
    else
    {
        dest[size] = '\0';
    }

    return size;
}

string o::formatVersionLong(string version)
{
    std::vector<string> verVec;
    b::split(verVec, version, b::is_any_of("."));
    return o::str_stream()
        << b::format("%02s.%02s.%04s") % verVec[0] % verVec[1] % verVec[2];
}
