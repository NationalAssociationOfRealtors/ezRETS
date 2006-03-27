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
#ifndef RESULTSET_H
#define RESULTSET_H

#include <boost/shared_ptr.hpp>
#include <vector>
#include "librets.h"
#include "DataTranslator.h"
#include "Descriptors.h"
#include "Column.h"
#include "ezretsfwd.h"

namespace odbcrets
{
class ResultSet
{
  public:

    ResultSet(EzLoggerPtr logger, MetadataViewPtr metadataView,
              DataTranslatorPtr translator, AppRowDesc* ard);
    
    int rowCount();
    int columnCount();
    bool isEmpty();

    bool hasNext();

    std::string getColumnName(int col);

    void bindColumn(int col, SQLSMALLINT TargetType, SQLPOINTER TargetValue,
                    SQLLEN BufferLength, SQLLEN *StrLenOrInd);

    void unbindColumns();

    void processNextRow();

    DataTranslatorPtr getDataTranslator();

    EzLoggerPtr getLogger();
    void setLogger(EzLoggerPtr logger);
    void setTranslator(DataTranslatorPtr translator);
    void setAPD(AppParamDesc* apd);
    AppRowDesc* getARD();
    MetadataViewPtr getMetadataView();

    void addColumn(std::string name, SQLSMALLINT DefaultType,
                   SQLULEN maxLength = 256);
    void addColumn(std::string name, librets::MetadataTable* table);
    void addRow(librets::StringVectorPtr row);

    ColumnPtr getColumn(int col);

    ColumnVectorPtr getColumns();

    void getData(SQLUSMALLINT colno, SQLSMALLINT TargetType,
                 SQLPOINTER TargetValue, SQLLEN BufferLength,
                 SQLLEN *StrLenorInd);

    void setReportedRowCount(int count);

  private:
    typedef std::vector<librets::StringVectorPtr> StringVectorVector;

    EzLoggerPtr mLogger;
    MetadataViewPtr mMetadataView;
    DataTranslatorPtr mTranslator;
    AppRowDesc* mArdPtr;
    bool mGotFirst;
    ColumnVectorPtr mColumns;
    StringVectorVector mResults;
    StringVectorVector::iterator mResultIterator;
    int mReportedRowCount;
};

}


#endif /* RETSSTMTRESULTS_H */

/* Local Variables: */
/* mode: c++ */
/* End: */
