/*
 * Copyright (C) 2007 National Association of REALTORS(R)
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
#ifndef DEFINITIONGENERATOR_H
#define DEFINITIONGENERATOR_H

#include <iosfwd>
#include "MetadataView.h"
#include "DataTranslator.h"
#include "librets/metadata_forward.h"

namespace ezhelper
{

class DefinitionGenerator
{
  public:
    DefinitionGenerator(bool standardNames, librets::RetsMetadata* metadata);

    std::ostream& createHTML(std::ostream& out);

  private:
    std::ostream& doTables(std::ostream& out,
                           odbcrets::ResourceClassPairPtr rcPair);
    std::ostream& doLookup(std::ostream& out, std::string resID,
                           librets::MetadataTable* table);

    librets::RetsMetadata* mMetadata;
    bool mStandardNames;
    std::auto_ptr<odbcrets::MetadataView> mMetadataView;
    std::auto_ptr<odbcrets::DataTranslator> mDataTranslator;
};

}

#endif /* DEFINITIONGENERATOR_H */

/* Local Variables: */
/* mode: c++ */
/* End: */
