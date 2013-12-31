/*ckwg +5
 * Copyright 2011-2013 by Kitware, Inc. All Rights Reserved. Please refer to
 * KITWARE_LICENSE.TXT for licensing information, or contact General Counsel,
 * Kitware, Inc., 28 Corporate Drive, Clifton Park, NY 12065.
 */

#ifndef MAPTK_CORE_CONFIG_IO_H
#define MAPTK_CORE_CONFIG_IO_H

#include "core_config.h"

// This the configuration object and has nothing to do with package
// configuration like core_config.h does
#include "config_block.h"
#include "types.h"

/**
 * \file
 * \brief IO Operation utilities for \c maptk::config
 *
 * \todo Describe format here.
 */

namespace maptk
{

/// Read in a configuration file, producing a \c config_block object
/**
 * \throws file_not_found_exception Thrown when the file could not be found
 *                                  on the file system.
 * \throws file_not_read_exception  Thrown when the file could not be read or
 *                                  parsed for whatever reason.
 * \throws filesystem_error         Boost exception thrown if something goes
 *                                  wrong with the underlying file read.
 *
 * \param file_path The path to the file to read in.
 * \return A \c config_block object representing the contents of the read-in file.
 */
config_block_t  MAPTK_CORE_EXPORT read_config_file(path_t const& file_path);

/// Output to file the given \c config_block object to the specified file path
/**
 * If a file exists at the target location, it will be overwritten.
 *
 * \throws file_write_exception Thrown when something prevents output of the
 *                              file.
 *
 * \param config    The \c config_block object to output.
 * \param file_path The path to output the file to.
 */
void MAPTK_CORE_EXPORT write_config_file(config_block_t const& config,
                                         path_t const& file_path);

}

#endif // MAPTK_CORE_CONFIG_IO_H
