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
#ifndef TYPEINFOMETADATAQUERY_H
#define TYPEINFOMETADATAQUERY_H

#include "ezrets.h"
#include "librets/std_forward.h"
#include "ezretsfwd.h"
#include "Query.h"

namespace odbcrets
{

class TypeInfoMetadataQuery : public Query
{
  public:
    TypeInfoMetadataQuery(RetsSTMT* stmt, SQLSMALLINT dataType);

    virtual SQLRETURN execute();

    virtual std::ostream & print(std::ostream & out) const;

    virtual void prepareResultSet();

  private:
    librets::StringVectorPtr getSQLGetTypeInfoRow(
        SQLSMALLINT dtype, std::string perc_radix,
        std::string unsigned_att = "", std::string litprefix = "",
        std::string litsuffix = "");
    
    SQLSMALLINT mDataType;
};

}


#endif /* TYPEINFOMETADATAQUERY_H */

/* Local Variables: */
/* mode: c++ */
/* End: */

