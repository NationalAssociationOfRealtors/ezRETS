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
#ifndef TABLEMETADATAQUERY_H
#define TABLEMETADATAQUERY_H

#include "ezrets.h"
#include <string>
#include "Query.h"

namespace odbcrets
{

class TableMetadataQuery : public Query
{
  public:
    TableMetadataQuery(RetsSTMT* stmt, std::string catalog, std::string schema,
                       std::string table, std::string tableType);

    virtual SQLRETURN execute();

    virtual std::ostream & print(std::ostream & out) const;

    virtual void prepareResultSet();

  private:
    std::string mCatalog;
    std::string mSchema;
    std::string mTable;
    std::string mTableType;
};

}

#endif /* TABLEMETADATAQUERY_H */

/* Local Variables: */
/* mode: c++ */
/* End: */

