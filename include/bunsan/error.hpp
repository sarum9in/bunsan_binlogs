#pragma once

#include <boost/exception/all.hpp>
#include <boost/units/detail/utility.hpp>
#include <boost/version.hpp>

#include <exception>

namespace bunsan
{
    struct error: virtual std::exception, virtual boost::exception
    {
        /*!
         * \brief Create error object with stacktrace tag.
         *
         * \see stacktrace
         */
        error();

        /*!
         * \brief Create error object with message and stacktrace tags.
         *
         * \see message
         * \see stacktrace
         */
        error(const std::string &message_);

        const char *what() const noexcept override;

        /*!
         * \tparam ErrorInfo boost::error_info instantiation
         * \return pointer to data associated with ErrorInfo
         */
        template <typename ErrorInfo>
        const typename ErrorInfo::value_type *get() const noexcept
        {
            return boost::get_error_info<ErrorInfo>(*this);
        }

        /*!
         * \copydoc get
         */
        template <typename ErrorInfo>
        typename ErrorInfo::value_type *get() noexcept
        {
            return boost::get_error_info<ErrorInfo>(*this);
        }

        error &enable_nested(const boost::exception_ptr &ptr);
        error &enable_nested_current();

        /// Human readable error message
        typedef boost::error_info<struct tag_message, std::string> message;

        typedef boost::errinfo_nested_exception nested_exception;

        template <typename Tag, typename T>
        static inline std::string info_name(const boost::error_info<Tag, T> &x)
        {
    #if BOOST_VERSION >= 105400
            return boost::error_info_name(x);
    #elif BOOST_VERSION >= 103700
            return boost::units::detail::demangle(x.tag_typeid_name());
    #else
            return boost::units::detail::demangle(x.tag_typeid().name());
    #endif
        }
    };
}
