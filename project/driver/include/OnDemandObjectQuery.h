/*
 * Copyright (C) 2009 National Association of REALTORS(R)
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
#ifndef ONDEMANDOBJECTQUERY_H
#define ONDEMANDOBJECTQUERY_H

#include "librets/sql_forward.h"
#include "librets/protocol_forward.h"
#include "ezretsfwd.h"
#include "Query.h"

namespace odbcrets
{
class OnDemandObjectQuery : public Query
{
  public:
    OnDemandObjectQuery(RetsSTMT* stmt,
                        librets::GetObjectQueryPtr objectQuery);

    virtual ~OnDemandObjectQuery();

    virtual SQLRETURN execute();

    virtual std::ostream & print(std::ostream & out) const;

    virtual void prepareResultSet();

  protected:
    librets::GetObjectQueryPtr mGetObjectQuery;
    librets::GetObjectRequestAPtr mObjectRequest;
    librets::GetObjectResponseAPtr mObjectResponse;
};

}

#endif /* ONDEMANDOBJECTQUERY_H */

/* Local Variables: */
/* mode: c++ */
/* End: */
