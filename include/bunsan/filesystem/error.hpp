#pragma once

#include <bunsan/error.hpp>

#include <boost/filesystem/path.hpp>

namespace bunsan{namespace filesystem
{
    struct error: virtual bunsan::error
    {
        typedef boost::error_info<struct tag_path, boost::filesystem::path> path;
        typedef boost::error_info<struct tag_source_path, boost::filesystem::path> source_path;
        typedef boost::error_info<struct tag_destination_path, boost::filesystem::path> destination_path;

        typedef boost::error_info<struct tag_openmode, std::ios_base::openmode> openmode;
    };
}}
