/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
/*
filename: gdcmQueryImage.h
contains: class to construct an image-based query for c-find and c-move
name and date: 15 oct 2010 mmr

*/
#ifndef QUERYIMAGE_H
#define QUERYIMAGE_H
#include "gdcmQueryBase.h"
#include "gdcmDataSet.h"

namespace gdcm{
  namespace network{
    class QueryImage : public QueryBase {
      gdcm::DataSet mCurrentQuery;
    public:
      std::vector<gdcm::Tag> GetRequiredTags(const ERootType& inRootType) const;
      std::vector<gdcm::Tag> GetUniqueTags(const ERootType& inRootType) const;
      std::vector<gdcm::Tag> GetOptionalTags(const ERootType& inRootType) const;

      std::string GetName() const { return "Composite Object Instance (Image)"; }
      DataElement GetQueryLevel() const;
    };
  }
}

#endif //QUERYIMAGE_H