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
#ifndef OBJECTQUERY_H
#define OBJECTQUERY_H

#include <boost/shared_ptr.hpp>
#include "ezretsfwd.h"
#include "librets.h"
#include "Query.h"

namespace odbcrets
{
class ObjectQuery : public Query
{
  public:
    ObjectQuery(RetsSTMT* stmt, librets::GetObjectQueryPtr objectQuery);

    virtual SQLRETURN execute();

    virtual std::ostream & print(std::ostream & out) const;

  private:
    void prepareLocationResultSet();
    void prepareBinaryResultSet();
    
    librets::GetObjectQueryPtr mGetObjectQuery;
};

}

#endif /* OBJECTQUERY_H */

/* Local Variables: */
/* mode: c++ */
/* End: */
