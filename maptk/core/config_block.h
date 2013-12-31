/*ckwg +5
 * Copyright 2011-2013 by Kitware, Inc. All Rights Reserved. Please refer to
 * KITWARE_LICENSE.TXT for licensing information, or contact General Counsel,
 * Kitware, Inc., 28 Corporate Drive, Clifton Park, NY 12065.
 */

#ifndef MAPTK_CORE_CONFIG_H
#define MAPTK_CORE_CONFIG_H

#include "core_config.h"
#include "types.h"
#include "exceptions/config_block.h"

#include <boost/optional/optional.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/noncopyable.hpp>

#include <exception>
#include <map>
#include <set>
#include <string>
#include <typeinfo>
#include <vector>

#include <cstddef>

/**
 * \file
 * \brief Header for \link maptk::config\endlink configuration object
 */

namespace maptk
{

/**
 * \brief Stores configuration values for use within a \ref pipeline.
 */
class MAPTK_CORE_EXPORT config_block
  : public boost::enable_shared_from_this<config_block>,
           boost::noncopyable
{
  public:

    /// Create an empty configuration.
    /**
     * \param name The name of the configuration block.
     * \returns An empty configuration block.
     */
    static config_block_t empty_config(config_block_key_t const& name = config_block_key_t());

    /// Destructor
    ~config_block();

    /// Get a subblock from the configuration.
    /**
     * Retrieve an unlinked configuration subblock from the current
     * configuration. Changes made to it do not affect \c *this.
     *
     * \param key The name of the sub-configuration to retrieve.
     * \returns A subblock with copies of the values.
     */
    config_block_t subblock(config_block_key_t const& key) const;

    /// Get a subblock view into the configuration.
    /**
     * Retrieve a view into the current configuration. Changes made to \c *this
     * are seen through the view and vice versa.
     *
     * \param key The name of the sub-configuration to retrieve.
     * \returns A subblock which links to the \c *this.
     */
    config_block_t subblock_view(config_block_key_t const& key);

    /// Internally cast the value.
    /**
     * \throws no_such_configuration_value_exception Thrown if the requested index does not exist.
     * \throws bad_configuration_cast_exception Thrown if the cast fails.
     *
     * \param key The index of the configuration value to retrieve.
     * \returns The value stored within the configuration.
     */
    template <typename T>
    T get_value(config_block_key_t const& key) const;

    /// Cast the value, returning a default value in case of an error.
    /**
     * \param key The index of the configuration value to retrieve.
     * \param def The value \p key does not exist or the cast fails.
     * \returns The value stored within the configuration, or \p def if something goes wrong.
     */
    template <typename T>
    T get_value(config_block_key_t const& key, T const& def) const MAPTK_NOTHROW;

    /// Set a value within the configuration.
    /**
     * \throws set_on_read_only_value_exception Thrown if \p key is marked as read-only.
     *
     * \postconds
     * \postcond{<code>this->get_value<value_t>(key) == value</code>}
     * \endpostconds
     *
     * \param key The index of the configuration value to set.
     * \param value The value to set for the \p key.
     */
    void set_value(config_block_key_t const& key, config_block_value_t const& value);

    /// Remove a value from the configuration.
    /**
     * \throws unset_on_read_only_value_exception Thrown if \p key is marked as read-only.
     * \throws no_such_configuration_value_exception Thrown if the requested index does not exist.
     *
     * \postconds
     * \postcond{<code>this->get_value<T>(key)</code> throws \c no_such_configuration_value_exception}
     * \endpostconds
     *
     * \param key The index of the configuration value to unset.
     */
    void unset_value(config_block_key_t const& key);

    /// Query if a value is read-only.
    /**
     *
     * \param key The key of the value query.
     * \returns True if \p key is read-only, false otherwise.
     */
    bool is_read_only(config_block_key_t const& key) const;

    /// Set the value within the configuration as read-only.
    /**
     * \postconds
     * \postcond{<code>this->is_read_only(key) == true</code>}
     * \endpostconds
     *
     * \param key The key of the value to mark as read-only.
     */
    void mark_read_only(config_block_key_t const& key);

    /// Merge the values in \p config into the current config.
    /**
     * \note Any values currently set within \c *this will be overwritten if conficts occur.
     *
     * \throws set_on_read_only_value_exception Thrown if \p key is marked as read-only.
     *
     * \postconds
     * \postcond{\c this->available_values() ⊆ \c config->available_values()}
     * \endpostconds
     *
     * \param config The other configuration.
     */
    void merge_config(config_block_t const& config);

    ///Return the values available in the configuration.
    /**
     * \returns All of the keys available within the block.
     */
    config_block_keys_t available_values() const;

    /// Check if a value exists for \p key.
    /**
     * \param key The index of the configuration value to check.
     * \returns Whether the key exists.
     */
    bool has_value(config_block_key_t const& key) const;

    /// The separator between blocks.
    static config_block_key_t const block_sep;
    /// The magic group for global parameters.
    static config_block_key_t const global_value;
  private:
    /// Internal constructor
    MAPTK_NO_EXPORT config_block(config_block_key_t const& name, config_block_t parent);

    /// private helper method to extract a value for a key
    boost::optional<config_block_value_t> find_value(config_block_key_t const& key) const;
    MAPTK_NO_EXPORT config_block_value_t get_value(config_block_key_t const& key) const;

    typedef std::map<config_block_key_t, config_block_value_t> store_t;
    typedef std::set<config_block_key_t> ro_list_t;

    config_block_t m_parent;
    config_block_key_t m_name;
    store_t m_store;
    ro_list_t m_ro_list;
};

/// Default cast handling of configuration values.
/**
 * \note Do not use this in user code. Use \ref config_block_cast instead.
 * \param value The value to convert.
 * \returns The value of \p value in the requested type.
 */
template <typename T>
inline
T
config_block_cast_default(config_block_value_t const& value)
{
  try
  {
    return boost::lexical_cast<T>(value);
  }
  catch (boost::bad_lexical_cast const& e)
  {
    throw bad_config_block_cast(e.what());
  }
}

/// Type-specific casting handling
/**
 * \note Do not use this in user code. Use \ref config_block_cast instead.
 * \param value The value to convert.
 * \returns The value of \p value in the requested type.
 */
template <typename T>
inline
T
config_block_cast_inner(config_block_value_t const& value)
{
  return config_block_cast_default<T>(value);
}

/// Type-specific casting handling, bool specialization
/**
 * This is the \c bool specialization to handle \tt{true} and \tt{false}
 * literals versus just \tt{1} and \tt{0}.
 *
 * \note Do not use this in user code. Use \ref config_block_cast instead.
 * \param value The value to convert.
 * \returns The value of \p value in the requested type.
 */
template <>
MAPTK_CORE_EXPORT bool config_block_cast_inner(config_block_value_t const& value);

/// Cast a configuration value to the requested type.
/**
 * \throws bad_configuration_cast Thrown when the conversion fails.
 * \param value The value to convert.
 * \returns The value of \p value in the requested type.
 */
template <typename T>
inline
T
config_block_cast(config_block_value_t const& value)
{
  return config_block_cast_inner<T>(value);
}

/// Internally cast the value.
template <typename T>
T
config_block
::get_value(config_block_key_t const& key) const
{
  boost::optional<config_block_value_t> value = find_value(key);

  if (!value)
  {
    throw no_such_configuration_value_exception(key);
  }

  try
  {
    return config_block_cast<T>(*value);
  }
  catch (bad_config_block_cast const& e)
  {
    throw bad_config_block_cast_exception(key, *value, typeid(T).name(), e.what());
  }
}

/// Cast the value, returning a default value in case of an error.
template <typename T>
T
config_block
::get_value(config_block_key_t const& key, T const& def) const MAPTK_NOTHROW
{
  try
  {
    return get_value<T>(key);
  }
  catch (...)
  {
    return def;
  }
}

}

#endif // MAPTK_CORE_CONFIG_H
