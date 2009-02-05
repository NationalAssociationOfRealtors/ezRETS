/*
 * Copyright (C) 2005-2009 National Association of REALTORS(R)
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

#include <vector>
#include "librets/std_forward.h"
#include "librets/metadata_forward.h"
#include "ezrets.h"
#include "ezretsfwd.h"

namespace odbcrets
{
class ResultSet
{
  public:
    ResultSet(EzLoggerPtr logger, MetadataViewPtr metadataView,
              DataTranslatorSPtr translator, AppRowDesc* ard);

    // Column related methods are the same no matter the type of
    // result set.
    int columnCount();
    std::string getColumnName(int col);
    ColumnPtr getColumn(int col);
    ColumnVectorPtr getColumns();
    void bindColumn(int col, SQLSMALLINT TargetType, SQLPOINTER TargetValue,
                    SQLLEN BufferLength, SQLLEN *StrLenOrInd);
    void unbindColumns();
    void addColumn(std::string name, SQLSMALLINT DefaultType,
                   SQLULEN maxLength = 256);
    void addColumn(std::string name, librets::MetadataTable* table,
                   bool useCompactFormat);

    // This are helper methods that all the resultsets need/use
    EzLoggerPtr getLogger();
    void setLogger(EzLoggerPtr logger);
    void setTranslator(DataTranslatorSPtr translator);
    void setAPD(AppParamDesc* apd);
    AppRowDesc* getARD();
    MetadataViewPtr getMetadataView();
    DataTranslatorSPtr getDataTranslator();
    
    // These methods have to deal with processing data, these will
    // be different per resultset
    virtual int rowCount() = 0;
    virtual bool isEmpty() = 0;
    virtual bool hasNext() = 0;

    virtual void processNextRow() = 0;

    virtual void getData(SQLUSMALLINT colno, SQLSMALLINT TargetType,
                         SQLPOINTER TargetValue, SQLLEN BufferLength,
                         SQLLEN *StrLenorInd, DataStreamInfo *streamInfo) = 0;

    // This method is SHOULD be unique to the BulkResultSet...
    // However, I need to have it declared here during the refactoring.
    // TODO: Remove his from the base class, it should NOT be needed in
    // the future.
    void addRow(librets::StringVectorPtr row);

  protected:
    typedef std::vector<librets::StringVectorPtr> StringVectorVector;

    EzLoggerPtr mLogger;
    MetadataViewPtr mMetadataView;
    DataTranslatorSPtr mTranslator;
    AppRowDesc* mArdPtr;
    bool mGotFirst;
    ColumnVectorPtr mColumns;
};

class DummyResultSet : public ResultSet
{
  public:
    DummyResultSet(EzLoggerPtr logger, MetadataViewPtr metadataView,
                   DataTranslatorSPtr translator, AppRowDesc* ard);

    // These methods have to deal with processing data, these will
    // be different per resultset
    int rowCount();
    bool isEmpty();
    bool hasNext();

    void processNextRow();

    void getData(SQLUSMALLINT colno, SQLSMALLINT TargetType,
                 SQLPOINTER TargetValue, SQLLEN BufferLength,
                 SQLLEN *StrLenorInd, DataStreamInfo *streamInfo);
};

class BulkResultSet : public ResultSet
{
  public:
    BulkResultSet(EzLoggerPtr logger, MetadataViewPtr metadataView,
                  DataTranslatorSPtr translator, AppRowDesc* ard);
    
    // These methods have to deal with processing data, these will
    // be different per resultset
    int rowCount();
    bool isEmpty();
    bool hasNext();

    void processNextRow();

    void getData(SQLUSMALLINT colno, SQLSMALLINT TargetType,
                 SQLPOINTER TargetValue, SQLLEN BufferLength,
                 SQLLEN *StrLenorInd, DataStreamInfo *streamInfo);

    // This method is unique to the BulkResultSet...
    void addRow(librets::StringVectorPtr row);
    
  private:
    bool mGotFirst;
    StringVectorVector mResults;
    StringVectorVector::iterator mResultIterator;
};

}


#endif /* RETSSTMTRESULTS_H */

/* Local Variables: */
/* mode: c++ */
/* End: */
