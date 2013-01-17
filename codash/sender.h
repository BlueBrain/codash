
/* Copyright (c) 2012, EPFL/Blue Brain Project
 *                     Daniel Nachbaur <daniel.nachbaur@epfl.ch>
 *
 * This file is part of CoDASH <https://github.com/BlueBrain/codash>
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

#ifndef CODASH_SENDER_H
#define CODASH_SENDER_H

#include <codash/api.h>

#include <dash/types.h>

#include <co/connectionDescription.h>

namespace co { class Zeroconf; }


namespace codash
{
namespace detail { class Sender; }

using lunchbox::uint128_t;

/**
 * The sender side of the codash communicator pattern.
 *
 * This sender can use an existing Collage localNode for communication or it
 * creates its own using the provided connection description.
 * This class is intended to propagate dash nodes + changes on them to connected
 * receivers. This sender maintains an own dash context used for tracking
 * changes on the registered nodes.
 */
class Sender
{
public:
    /**
     * Construct a sender with the given connection.
     *
     * The created local node is maintained by the sender. It will listen on the
     * given connection description for incoming receivers. Additionally, the
     * underlying Collage library will be initialized.
     *
     * @param argc the command line argument count.
     * @param argv the command line argument values.
     * @param conn the listening connection of this sender.
     * @version 0.1
     */
    CODASH_API Sender( int argc, char** argv,
                       co::ConnectionDescriptionPtr conn );

    /** Construct a sender with the given localNode. @version 0.1 */
    CODASH_API Sender( co::LocalNodePtr localNode );

    /** Destruct this sender. @version 0.1 */
    CODASH_API ~Sender();

    /** @return the used local node. @version 0.1 */
    CODASH_API co::ConstLocalNodePtr getNode() const;

    /** @return the Zeroconf communicator handle of the local node. @version 0.1 */
    CODASH_API co::Zeroconf getZeroconf();

    /** @return the dash::Context of this sender. @version 0.1 */
    CODASH_API dash::Context& getContext();

    /** @return true if any receiver is connected & listening. @version 0.1 */
    CODASH_API bool hasPeers() const;

    /**
     * Register a dash::Node to this sender.
     *
     * Newly registered nodes will be propagated to connected receivers after
     * the next send().
     *
     * @param node the dash::Node to be registered.
     * @version 0.1
     */
    CODASH_API void registerNode( dash::NodePtr node );

    /**
     * Deregister a dash::Node from this sender.
     *
     * Degistered nodes will be propagated to connected receivers after
     * the next send().
     *
     * @param node the dash::Node to be deregistered.
     * @version 0.1
     */
    CODASH_API void deregisterNode( dash::NodePtr node );

    /**
     * Apply provided changes and propagate them to connected receivers.
     *
     * @param cmt the dash commit containing the changes to apply & send
     *
     * @version 0.1
     */
    CODASH_API void send( const dash::Commit& cmt );

private:
    detail::Sender* const _impl;
};

}

#endif
