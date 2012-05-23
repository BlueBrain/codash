
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

#include "receiver.h"
#include "types.h"

#include <co/connectionDescription.h>
#include <co/global.h>
#include <co/packets.h>
#include <co/objectMap.h>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>


namespace codash
{
namespace detail
{

Receiver::Receiver( int argc, char** argv )
    : Communicator( argc, argv, 0 )
    , proxyNode_()
    , mapQueue_()
    , nodes_()
    , queuedVersions_()
    , objectMapVersion_( co::VERSION_FIRST )
{
    localNode_->registerPushHandler( groupID_,
            boost::bind( &Receiver::handleInit_, this, _1, _2, _3, _4 ));
}

Receiver::Receiver( co::LocalNodePtr localNode )
    : Communicator( localNode )
    , proxyNode_()
    , mapQueue_()
    , nodes_()
    , queuedVersions_()
    , objectMapVersion_( co::VERSION_FIRST )
{
    localNode_->registerPushHandler( groupID_,
            boost::bind( &Receiver::handleInit_, this, _1, _2, _3, _4 ));
}

Receiver::~Receiver()
{
    disconnect();
}

bool Receiver::connect( co::ConnectionDescriptionPtr conn )
{
    if( !conn || isConnected( ))
        return false;

    proxyNode_ = new co::Node;
    proxyNode_->addConnectionDescription( conn );
    if( !localNode_->connect( proxyNode_ ))
        return false;

    co::NodeCommandPacket packet;
    packet.commandID = initCmd_;
    proxyNode_->send( packet );
    initialized_.waitEQ( true );
    processMappings_();
    objectMapVersion_ = objectMap_->getVersion();
    return true;
}

bool Receiver::disconnect()
{
    if( !isConnected() || !localNode_->disconnect( proxyNode_ ))
        return false;

    proxyNode_ = co::NodePtr();
    return true;
}

bool Receiver::isConnected() const
{
    if( !proxyNode_ )
        return false;
    return proxyNode_->isConnected();
}

void Receiver::handleInit_( const uint128_t& groupID, const uint128_t& typeID,
                            const uint128_t& objectID,
                            co::DataIStream& istream )
{
    EQASSERT( groupID == groupID_ );
    EQASSERT( typeID == typeInit_ );

    deserialize( istream, co::Serializable::DIRTY_ALL );
    mapQueue_.push_back( boost::bind( &co::LocalNode::mapObject,
                  localNode_.get(), this, objectID, co::VERSION_NONE ));
    initialized_ = true;
}

void Receiver::processMappings_()
{
    BOOST_FOREACH( const WorkFunc& func, mapQueue_ )
    {
        func();
    }
    mapQueue_.clear();
}

const dash::Nodes& Receiver::getNodes() const
{
    static dash::Nodes nodes;
    nodes.clear();
    BOOST_FOREACH( const uint128_t id, nodes_ )
    {
        Node* node = static_cast< Node* >( objectMap_->get( id ));
        dash::NodePtr dashNode = node->getValue();
        nodes.push_back( dashNode );
    }

    return nodes;
}

bool Receiver::sync()
{
    uint128_t version;
    while( !queuedVersions_.timedPop( co::Global::getKeepaliveTimeout(),
                                      version ))
    {
        if( !isConnected( ))
        {
            LBWARN << "Lost connection to sender while waiting for new data"
                   << std::endl;
            return false;
        }
        else
            LBWARN << "Got timeout while waiting for new data" << std::endl;
    }

    Communicator::sync( version );
    objectMap_->sync( objectMapVersion_ );

    return true;
}

void Receiver::serialize( co::DataOStream& os, const uint64_t dirtyBits )
{
    LBDONTCALL
    Communicator::serialize( os, dirtyBits );
}

void Receiver::deserialize( co::DataIStream& is, const uint64_t dirtyBits )
{
    if( dirtyBits & DIRTY_NODES )
    {
        uint64_t size;
        is >> size;
        nodes_.clear();
        nodes_.reserve( size_t( size ));

        for( uint64_t i = 0; i < size; ++i )
        {
            uint128_t id;
            is >> id;
            nodes_.push_back( id );
        }
    }
    if( dirtyBits & DIRTY_OBJECTMAP )
    {
        co::ObjectVersion ov;
        is >> ov;
        objectMapVersion_ = ov.version;
        if( !objectMap_->isAttached( ))
            mapQueue_.push_back( boost::bind( &co::LocalNode::mapObject,
                                            localNode_.get(), objectMap_, ov ));
    }

    Communicator::deserialize( is, dirtyBits );
}

void Receiver::notifyNewHeadVersion( const uint128_t& version )
{
    if( version > co::VERSION_FIRST )
        queuedVersions_.push( version );
    Communicator::notifyNewHeadVersion( version );
}

}
}
