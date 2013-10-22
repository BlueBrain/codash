
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

#ifndef CODASH_RECEIVER_H
#define CODASH_RECEIVER_H

#include <codash/api.h>
#include <codash/types.h>
#include <dash/types.h>

#include <co/connectionDescription.h>


namespace codash
{
namespace detail { class Receiver; }


/** The receiver side of the codash communicator pattern.
 *
 * This receiver can use an existing Collage localNode for communication or it
 * creates its own using the provided connection description.
 * This class is intended to receive dash nodes + changes on them from a
 * connected sender. This receiver maintains an own dash context used for
 * holding received nodes.
 */
class Receiver : public lunchbox::Referenced
{
public:
    /** Construct a receiver with the given connection.
     *
     * The created local node is maintained by the receiver. It will listen on
     * the given connection description for an incoming sender. Note that the
     * Collage library has to be initialized beforehand.
     * @version 0.1
     */
    CODASH_API Receiver();

    /** Construct a receiver with the given localNode. @version 0.1 */
    CODASH_API explicit Receiver( co::LocalNodePtr localNode );

    /** Destruct this receiver. @version 0.1 */
    CODASH_API ~Receiver();

    /** Create a managed receiver mapped to the identifier. @version 0.1 */
    CODASH_API static ReceiverPtr create( const std::string& identifier,
                                          co::LocalNodePtr localNode = 0 );

    /** Destroy a managed receiver by its identifier. @version 0.1 */
    CODASH_API static void destroy( const std::string& identifier );

    /** Destroy a managed receiver. @version 0.1 */
    CODASH_API static void destroy( ReceiverPtr receiver );

    /** @return the used local node. @version 0.1 */
    CODASH_API co::ConstLocalNodePtr getNode() const;

    /** @return the Zeroconf communicator handle of the local node. @version 0.1 */
    CODASH_API co::Zeroconf getZeroconf();

    /** Connect to the given sender.
     *
     * @param conn the connection of the sender.
     * @return true if connect to sender was successful, false otherwise
     * @version 0.1
     */
    CODASH_API bool connect( co::ConnectionDescriptionPtr conn );

    /** Connect to the given sender.
     *
     * @param nodeID the ID of the localNode of the sender.
     * @return true if connect to sender was successful, false otherwise
     * @version 0.1
     */
    CODASH_API bool connect( const co::NodeID& nodeID );

    /** Disconnect from a connected sender.
     *
     * @return true if disconnect from sender was successful, false otherwise
     * @version 0.1
     */
    CODASH_API bool disconnect();

    /** @return true if connected to a sender. @version 0.1 */
    CODASH_API bool isConnected() const;

    /** @return the connection description of the sender. @version 0.1 */
    CODASH_API co::ConstConnectionDescriptionPtr getConnection() const;

    /** @return the dash::Context of this receiver. @version 0.1 */
    CODASH_API dash::Context& getContext();

    /**
     * @return the list of all mapped dash::Nodes, ordered by their identifier
     * @version 0.1
     */
    CODASH_API dash::Nodes getNodes() const;

    /** Map a dash node to the master instance on the sender side.
     *
     * If the node was already mapped, the already mapped instance will be
     * returned. If the given identifier was not registered on the sender, an
     * empty dash::Node will be returned.
     *
     * @param identifier the node was registered with this identifier on the
     *                   sender side
     * @return the mapped dash::Node with its latest version if mapping was
     *         successful, 0 otherwise
     * @version 0.1
     */
    CODASH_API dash::NodePtr mapNode( const uint32_t identifier );

    /** Receive one new change from the connected sender.
     *
     * It will apply the next version on all mapped dash::Nodes. The sync call
     * will block until there is a new version coming from the sender or if the
     * timeout was reached.
     * @sa co::Global::getKeepaliveTimeout()
     *
     * @return true if data was received from sender, false otherwise
     * @version 0.1
     */
    CODASH_API bool sync();

    /** Register a handler for new available versions.
     *
     * On every incoming new version sent by the connected sender, the handlers
     * will be called instantly from a separate thread. The notification can be
     * used to schedule calls to sync() accordingly.
     *
     * @param func the handler function to be called on each new version
     * @version 0.1
     */
    CODASH_API void registerNewVersionHandler( const VersionHandler& func );

private:
    detail::Receiver* const _impl;
};

}

#endif
