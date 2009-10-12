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
#ifndef ONDEMANDOBJECTRESULTSET_H
#define ONDEMANDOBJECTRESULTSET_H

#include "ResultSet.h"

namespace odbcrets
{

class OnDemandObjectResultSet : public ResultSet
{
  public:
    OnDemandObjectResultSet(EzLoggerPtr logger, MetadataViewPtr metadataView,
                            DataTranslatorSPtr translator, AppRowDesc* ard);

    void setObjectResponse(librets::GetObjectResponse* response);

    // These methods have to deal with processing data, these will
    // be different per resultset
    int rowCount();
    bool isEmpty();
    bool hasNext();

    void processNextRow();

    void getData(SQLUSMALLINT colno, SQLSMALLINT TargetType,
                 SQLPOINTER TargetValue, SQLLEN BufferLength,
                 SQLLEN *StrLenorInd, DataStreamInfo *streamInfo);

  private:
    librets::GetObjectResponse* mResponse;
};

}

#endif /* ONDEMANDOBJECTRESULTSET_H */

/* Local Variables: */
/* mode: c++ */
/* End: */
