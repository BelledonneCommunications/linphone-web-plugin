// MISSING BOOST FILESYSTEM OPERATION
//  operations.cpp  --------------------------------------------------------------------//

//  Copyright 2002-2009 Beman Dawes
//  Copyright 2001 Dietmar Kuehl

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  See library home page at http://www.boost.org/libs/filesystem

//--------------------------------------------------------------------------------------// 

#pragma once
#ifndef FSOperations_h__
#define FSOperations_h__

#include <boost/filesystem.hpp>

namespace boost {
namespace filesystem {
# ifdef BOOST_WINDOWS_API
  const boost::filesystem::path dot_path(L".");
  const boost::filesystem::path dot_dot_path(L"..");
# else
  const boost::filesystem::path dot_path(".");
  const boost::filesystem::path dot_dot_path("..");
# endif
  BOOST_FILESYSTEM_DECL
  path canonical(const path& p, const path& base = boost::filesystem::current_path(), system::error_code* ec=0)
  {
    path source (p.is_absolute() ? p : absolute(p, base));
    path result;

    boost::system::error_code local_ec;
    file_status stat (status(source, local_ec));

    if (stat.type() == boost::filesystem::file_not_found)
    {
      if (ec == 0)
        BOOST_FILESYSTEM_THROW(filesystem_error(
          "boost::filesystem::canonical", source,
          boost::system::error_code(boost::system::errc::no_such_file_or_directory, boost::system::generic_category())));
      ec->assign(boost::system::errc::no_such_file_or_directory, boost::system::generic_category());
      return result;
    }
    else if (local_ec)
    {
      if (ec == 0)
        BOOST_FILESYSTEM_THROW(filesystem_error(
          "boost::filesystem::canonical", source, local_ec));
      *ec = local_ec;
      return result;
    }

    bool scan (true);
    while (scan)
    {
      scan = false;
      result.clear();
      for (path::iterator itr = source.begin(); itr != source.end(); ++itr)
      {
        if (*itr == dot_path)
          continue;
        if (*itr == dot_dot_path)
        {
          result.remove_filename();
          continue;
        }

        result /= *itr;

        bool is_sym (boost::filesystem::is_symlink(boost::filesystem::symlink_status(result, *ec)));
        if (ec && *ec)
          return path();

        if (is_sym)
        {
          path link(boost::filesystem::read_symlink(result, *ec));
          if (ec && *ec)
            return path();
          result.remove_filename();

          if (link.is_absolute())
          {
            for (++itr; itr != source.end(); ++itr)
              link /= *itr;
            source = link;
          }
          else // link is relative
          {
            path new_source(result);
            new_source /= link;
            for (++itr; itr != source.end(); ++itr)
              new_source /= *itr;
            source = new_source;
          }
          scan = true;  // symlink causes scan to be restarted
          break;
        }
      }
    }
    if (ec != 0)
      ec->clear();
    //BOOST_ASSERT_MSG(result.is_absolute(), "canonical() implementation error; please report");
    return result;
  }
}
}

#endif //FSOperations_h__