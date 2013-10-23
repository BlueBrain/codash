
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


#include "sender.h"
#include "detail/communicator.h"
#include "detail/types.h"

#include <co/iCommand.h>
#include <co/connectionDescription.h>
#include <co/customICommand.h>
#include <co/objectMap.h>

#include <lunchbox/hash.h>

#include <boost/foreach.hpp>
#include <boost/bind.hpp>

namespace codash
{
namespace detail
{

typedef lunchbox::RefPtrHash< dash::Node, NodePtr > NodeMap;

class Sender : public Communicator
{
public:
    explicit Sender( co::ConnectionDescriptionPtr conn )
        : Communicator( conn )
        , _nodeMap()
    {
        _init();
    }

    explicit Sender( co::LocalNodePtr localNode )
        : Communicator( localNode )
        , _nodeMap()
    {
        _init();
    }

    ~Sender()
    {
        _localNode->releaseObject( _objectMap );
        delete _objectMap;
        _objectMap = 0;
        LBASSERTINFO( _nodeMap.empty(), "Nodes not deregistered" );
        _nodeMap.clear();
    }

    dash::Nodes getNodes() const
    {
        dash::Nodes nodes;
        BOOST_FOREACH( const NodeMap::value_type& entry, _nodeMap )
        {
            nodes.push_back( entry.first );
        }
        return nodes;
    }

    void registerNode( dash::NodePtr dashNode, const UUID& identifier )
    {
        lunchbox::ScopedFastWrite mutex( _context );

        dash::Context::getCurrent().map( dashNode, *_context );

        NodePtr node( new Node( dashNode ));
        node->setID( identifier );
        _nodeMap[ dashNode ] = node;
        setDirty( DIRTY_NODES );

        _objectMap->register_( node.get(), OBJECTTYPE_NODE );
    }

    void deregisterNode( dash::NodePtr dashNode )
    {
        lunchbox::ScopedFastWrite mutex( _context );

        NodePtr node = _nodeMap[ dashNode ];
        _objectMap->deregister( node.get( ));
        _nodeMap.erase( dashNode );
        setDirty( DIRTY_NODES );

        _context->unmap( dashNode );
    }

    void send( const dash::Commit& cmt )
    {
        lunchbox::ScopedFastWrite mutex( _context );

        dash::Context& appCtx = _context->setCurrent();

        _context->apply( cmt );
        _context->commit();

        const uint128_t oldVersion = _objectMap->getVersion();
        if( _objectMap->commit() > oldVersion )
            setDirty( DIRTY_OBJECTMAP );

        appCtx.setCurrent();

        Communicator::commit();
    }

    virtual void serialize( co::DataOStream& os, const uint64_t dirtyBits )
    {
        if( dirtyBits & DIRTY_NODES )
        {
            IDSet nodes;
            BOOST_FOREACH( const NodeMap::value_type& entry, _nodeMap )
            {
                nodes.insert( entry.second->getID( ));
            }
            os << nodes;
        }
        if( dirtyBits & DIRTY_OBJECTMAP )
            os << co::ObjectVersion( _objectMap );

        Communicator::serialize( os, dirtyBits );
    }

    virtual void deserialize( co::DataIStream& is, const uint64_t dirtyBits )
    {
        LBDONTCALL;
        Communicator::deserialize( is, dirtyBits );
    }

private:
    void _init()
    {
        if( !_localNode )
            throw std::runtime_error( "No localNode for sender init" );

        _localNode->registerObject( _objectMap );
        _localNode->registerObject( this );
        _localNode->registerCommandHandler( _initCmd,
                                  boost::bind( &Sender::_cmdConnect, this, _1 ),
                                  _localNode->getCommandThreadQueue( ));
    }

    bool _cmdConnect( co::CustomICommand& command )
    {
        co::Nodes nodes( 1, command.getNode( ));

        lunchbox::ScopedFastRead mutex( _context );

        _context->setCurrent();
        push( _groupID, _typeInit, nodes );

        return true;
    }

    NodeMap _nodeMap;
};
}

Sender::Sender( co::ConnectionDescriptionPtr conn )
    : _impl( new detail::Sender( conn ))
{
}

Sender::Sender( co::LocalNodePtr localNode )
    : _impl( new detail::Sender( localNode ))
{
}

Sender::~Sender()
{
    delete _impl;
}

co::ConstLocalNodePtr Sender::getLocalNode() const
{
    return _impl->getLocalNode();
}

co::Zeroconf Sender::getZeroconf()
{
    return _impl->getZeroconf();
}

dash::Context& Sender::getContext()
{
    return _impl->getContext();
}

bool Sender::hasPeers() const
{
    co::Nodes peers;
    _impl->getLocalNode()->getNodes( peers, false );
    return !peers.empty();
}

dash::Nodes Sender::getNodes() const
{
    return _impl->getNodes();
}

void Sender::registerNode( dash::NodePtr node, const UUID& identifier )
{
    _impl->registerNode( node, identifier );
}

void Sender::deregisterNode( dash::NodePtr node )
{
    _impl->deregisterNode( node );
}

void Sender::send( const dash::Commit& cmt )
{
    _impl->send( cmt );
}

}
