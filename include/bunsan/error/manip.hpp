#pragma once

namespace bunsan
{
    /*!
     * \brief Manipulators allow altering exceptions in streaming interface.
     *
     * User should provide own manipulator specializing this template by Tag.
     * Alternation is done by void operator().
     */
    template <typename Tag>
    struct error_manip
    {
        // inline void operator()(const Error &) const {}
    };

    template <typename Error, typename Tag>
    const Error &operator<<(const Error &error, const error_manip<Tag> &manip)
    {
        manip(error);
        return error;
    }
}
