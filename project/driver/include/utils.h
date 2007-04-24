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
#ifndef UTILS_H
#define UTILS_H

#include "ezrets.h"
#include <string>

namespace odbcrets
{
size_t copyString(std::string src, char* dest, size_t length);

/**
 * This formats the version string to a form of 00.00.0000.  Assumes
 * the version is in the format of x.y.z to use in its transformation.
 * In truth, this just pads with 0 to the left of each of x.y.z
 */
std::string formatVersionLong(std::string value);

/**
 * Translates from the SQL Type to a string showing the name.  It'll
 * be useful in about 10,000 places.
 */
std::string getTypeName(SQLSMALLINT type);
}
#endif /* UTILS_H */

/* Local Variables: */
/* mode: c++ */
/* End: */
