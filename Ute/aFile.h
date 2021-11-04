#ifndef AFILE_H_
#define AFILE_H_

//
// DST Ute Library (Utilities Library)
//
// Defence Science and Technology (DST) Group
// Department of Defence, Australia.
// 506 Lorimer St
// Fishermans Bend, VIC
// AUSTRALIA, 3207
//
// Copyright 2005-2018 Commonwealth of Australia
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this
// software and associated documentation files (the "Software"), to deal in the Software
// without restriction, including without limitation the rights to use, copy, modify,
// merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be included in all copies
// or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
// OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//

//
// Title:      File Information Class
// Class:      aFile
//
// Description:
//  Retrieve information about files on the system.
//
// Written by: Shane Hill  (Shane.Hill@dsto.defence.gov.au)
//
// First Date: 24/11/2011
//

// Local Includes
#include "aString.h"

// C++ Includes
#include <vector>
#include <dirent.h>
#include <sys/stat.h>

namespace dstoute {

  class aFile : public aFileString
  {
   public:
    aFile() : aFileString(), status_(0) {}
    aFile( const char* str) : aFileString( str), status_(0) {}
    aFile( const std::string &str) : aFileString( str), status_(0) {}

    bool exists()       { refresh(); return ( status_ != -1);}
    bool isDir()        { refresh(); return  S_ISDIR( fileInfo_.st_mode);}
    bool isFile()       { refresh(); return ( exists() && !S_ISDIR( fileInfo_.st_mode));}
    bool isExecutable() { refresh(); return ( ( fileInfo_.st_mode & S_IEXEC) != 0);}

   private:
    void refresh() { status_ = stat( c_str(), &fileInfo_);}

    struct stat fileInfo_;
    int status_;
  };

  typedef std::vector< aFile > aFileList;

} // End namespace dstoute.

#endif // End header AFILE_H_
