#pragma once

#include <bunsan/error/manip.hpp>

#include <boost/exception/all.hpp>
#include <boost/units/detail/utility.hpp>
#include <boost/version.hpp>

#include <exception>

// see bunsan::error::info_name()
#if BOOST_VERSION < 105400
#   include <typeinfo>
#endif

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

        /// Human readable error message
        typedef boost::error_info<struct tag_message, std::string> message;

        typedef boost::errinfo_nested_exception nested_exception;

        template <typename Tag, typename T>
        static inline std::string info_name(const boost::error_info<Tag, T> &x)
        {
    #if BOOST_VERSION >= 105400
            return boost::error_info_name(x);
    #else
            (void) x;
            return boost::units::detail::demangle(typeid(Tag *).name());
    #endif
        }
    };

    template<>
    class error_manip<struct tag_enable_nested>
    {
    public:
        explicit error_manip(const boost::exception_ptr &ptr): m_ptr(ptr) {}

        void operator()(const boost::exception &e) const;

    private:
        boost::exception_ptr m_ptr;
    };
    typedef error_manip<tag_enable_nested> enable_nested;

    template<>
    struct error_manip<struct tag_enable_nested_current>
    {
        void operator()(const boost::exception &e) const;
    };
    typedef error_manip<tag_enable_nested_current> enable_nested_current;
}
