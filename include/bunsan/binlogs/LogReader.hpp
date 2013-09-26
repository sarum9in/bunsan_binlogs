#pragma once

#include <bunsan/binlogs/BaseReader.hpp>
#include <bunsan/binlogs/MessageTypePool.hpp>
#include <bunsan/binlogs/MessageType.hpp>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/message.h>

#include <memory>

namespace bunsan {
namespace binlogs {

class LogReader: public BaseReader {
public:
    /// Allocate and read message of appropriate type.
    std::unique_ptr<google::protobuf::Message> read();

    /// False means EOF.
    bool read(const std::unique_ptr<google::protobuf::Message> &message);

    /// False means EOF.
    virtual bool read(google::protobuf::Message &message)=0;

    /*!
     * \brief What message will be read next (if available)?
     *
     * \warning Pointer is only valid until next call to read().
     * \note Subsequent calls to nextMessageType() will return the same pointer.
     *
     * \return nullptr on EOF
     */
    virtual const MessageType *nextMessageType()=0;

    /// \warning May change during log reading.
    virtual const MessageTypePool &messageTypePool() const=0;

    /*!
     * \brief Check stream's format in fastest way possible.
     *
     * \note Implementation may omit messages consistency check
     * and treat them as byte arrays.
     *
     * Consumes entire stream.
     *
     * \note Default implementation reads all messages to the EOF.
     * Should be reimplemented in subclass for faster implementation.
     */
    virtual void fastCheck();
};

}
}
