
/* Copyright (c) 2012, EFPL/Blue Brain Project
 *                     Daniel Nachbaur <daniel.nachbaur@epfl.ch>
 *
 * This file is part of CODASH <https://github.com/BlueBrain/codash>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *  
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef CODASH_RECEIVER_H
#define CODASH_RECEIVER_H

#include <codash/api.h>

#include <dash/types.h>

#include <co/connectionDescription.h>


namespace codash
{
namespace detail { class Receiver; }

using lunchbox::uint128_t;

/**
 * The receiver side of the codash communicator pattern.
 *
 * This receiver can use an existing Collage localNode for communication or it
 * creates its own using the provided connection description.
 * This class is intended to receive dash nodes + changes on them from a
 * connected sender. This receiver maintains an own dash context used for
 * holding received nodes.
 */
class Receiver
{
public:
    /**
     * Construct a receiver with the given connection.
     *
     * The created local node is maintained by the receiver. It will listen on
     * the given connection description for an incoming sender. Additionally,
     * the underlying Collage library will be initialized.
     *
     * @param argc the command line argument count.
     * @param argv the command line argument values.
     * @version 0.1
     */
    CODASH_API Receiver( int argc, char** argv );

    /** Construct a receiver with the given localNode. @version 0.1 */
    CODASH_API Receiver( co::LocalNodePtr localNode );

    /** Destruct this receiver. @version 0.1 */
    CODASH_API ~Receiver();

    /** @return the used local node. @version 0.1 */
    CODASH_API const co::LocalNodePtr getNode() const;

    /** @return a Zeroconf communicator handle for this node. @version 0.1 */
    CODASH_API co::Zeroconf getZeroconf();

    /**
     * Connect to the given sender.
     *
     * @param conn the connection of the sender.
     * @return true if connect to sender was successful, false otherwise
     * @version 0.1
     */
    CODASH_API bool connect( co::ConnectionDescriptionPtr conn );

    /**
     * Connect to the given sender.
     *
     * @param nodeID the ID of the localNode of the sender.
     * @return true if connect to sender was successful, false otherwise
     * @version 0.1
     */
    CODASH_API bool connect( const co::NodeID& nodeID );

    /**
     * Disconnect from a connected sender.

     * @return true if disconnect from sender was successful, false otherwise
     * @version 0.1
     */
    CODASH_API bool disconnect();

    /** @return true if connected to a sender. @version 0.1 */
    CODASH_API bool isConnected() const;

    /** @return the dash::Context of this receiver. @version 0.1 */
    CODASH_API dash::Context& getContext();

    /** @return the list of all received dash::Nodes. @version 0.1 */
    CODASH_API const dash::Nodes& getNodes() const;

    /**
     * Receive new changes from the connected sender.
     *
     * It will apply the latest version on all dash::Nodes. The sync call
     * will block until there is a new version coming from the sender or if the
     * timeout was reached.
     * @sa co::Global::getKeepaliveTimeout()
     *
     * @return true if data was received from sender, false otherwise
     * @version 0.1
     */
    CODASH_API bool sync();

private:
    detail::Receiver* const impl_;
};

}

#endif
