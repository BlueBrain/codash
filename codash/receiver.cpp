
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

#include "receiver.h"
#include "detail/communicator.h"
#include "detail/types.h"

#include <co/connectionDescription.h>
#include <co/customOCommand.h>
#include <co/global.h>
#include <co/objectMap.h>

#include <lunchbox/mtQueue.h>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>


namespace codash
{

typedef stde::hash_map< std::string, ReceiverPtr > Receivers;
typedef Receivers::iterator ReceiversIter;
typedef Receivers::const_iterator ReceiversCIter;
static lunchbox::Lockable< Receivers > _receivers;

namespace detail
{

typedef boost::function< void() > WorkFunc;

class Receiver : public Communicator
{
public:
    Receiver()
        : Communicator( co::ConnectionDescriptionPtr( ))
        , _proxyNode()
        , _mapQueue()
        , _allNodes()
        , _queuedVersions()
        , _objectMapVersion( co::VERSION_FIRST )
    {
        _init();
    }

    explicit Receiver( co::LocalNodePtr localNode )
        : Communicator( localNode )
        , _proxyNode()
        , _mapQueue()
        , _allNodes()
        , _queuedVersions()
        , _objectMapVersion( co::VERSION_FIRST )
    {
        _init();
    }

    ~Receiver()
    {
        _objectMap->clear();
        _nodes.clear();
        disconnect();
    }

    bool connect( co::ConnectionDescriptionPtr conn )
    {
        if( !conn || isConnected( ))
            return false;

        _proxyNode = new co::Node;
        _proxyNode->addConnectionDescription( conn );
        if( !_localNode->connect( _proxyNode ))
            return false;

        return _connect();
    }

    bool connect( const co::NodeID& nodeID )
    {
        if( isConnected( ))
            return false;

        _proxyNode = _localNode->connect( nodeID );
        if( !_proxyNode )
            return false;

        return _connect();
    }

    bool disconnect()
    {
        if( !isConnected() || !_localNode->disconnect( _proxyNode ))
            return false;

        _proxyNode = co::NodePtr();
        return true;
    }

    bool isConnected() const
    {
        if( !_proxyNode )
            return false;
        return _proxyNode->isConnected();
    }

    co::ConstConnectionDescriptionPtr getConnection()
    {
        return _proxyNode && _proxyNode->isConnected()
                    ? _proxyNode->getConnection()->getDescription()
                    : co::ConstConnectionDescriptionPtr();
    }

    dash::Nodes getNodes() const
    {
        dash::Nodes nodes;
        BOOST_FOREACH( const NodeMap::value_type& entry, _nodes )
        {
            nodes.push_back( entry.second->getValue( ));
        }
        return nodes;
    }

    dash::NodePtr getNode( const UUID& identifier ) const
    {
        NodeMap::const_iterator i = _nodes.find( identifier );
        return i != _nodes.end() ? i->second->getValue() : dash::NodePtr();
    }

    dash::NodePtr mapNode( const UUID& identifier )
    {
        IDSet::const_iterator i = _allNodes.find( identifier );
        if( i == _allNodes.end( ))
            return dash::NodePtr();

        NodeMap::const_iterator j = _nodes.find( identifier );
        if( j != _nodes.end( ))
            return j->second->getValue();

        Node* node = static_cast< Node* >( _objectMap->map( identifier ));
        _nodes.insert( std::make_pair( identifier, node ));
        return node->getValue();
    }

    virtual bool syncOne()
    {
        uint128_t version;
        while( !_queuedVersions.timedPop( co::Global::getKeepaliveTimeout(),
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
        _objectMap->sync( _objectMapVersion );

        _processUnmappings();

        return true;
    }

    virtual void serialize( co::DataOStream& os, const uint64_t dirtyBits )
    {
        LBDONTCALL
        Communicator::serialize( os, dirtyBits );
    }

    virtual void deserialize( co::DataIStream& is, const uint64_t dirtyBits )
    {
        if( dirtyBits & DIRTY_NODES )
        {
            IDSet newNodes;
            is >> newNodes;

            LBASSERT( _nodesToUnmap.empty( ));
            std::set_difference( _allNodes.begin(), _allNodes.end(),
                                 newNodes.begin(), newNodes.end(),
                                 std::inserter( _nodesToUnmap,
                                                _nodesToUnmap.end( )));
            _allNodes = newNodes;
        }
        if( dirtyBits & DIRTY_OBJECTMAP )
        {
            co::ObjectVersion ov;
            is >> ov;
            _objectMapVersion = ov.version;
            if( !_objectMap->isAttached( ))
            {
                _mapQueue.push_back( boost::bind( &co::LocalNode::mapObject,
                                                  _localNode.get(), _objectMap,
                                                  ov ));
            }
        }

        Communicator::deserialize( is, dirtyBits );
    }

    virtual void notifyNewHeadVersion( const uint128_t& version )
    {
        _queuedVersions.push( version );
        std::for_each( _handlers.begin(), _handlers.end(),
                    boost::bind( &VersionHandlers::value_type::operator(), _1));

        Communicator::notifyNewHeadVersion( version );
    }

    void registerNewVersionHandler( const VersionHandler& func )
    {
        LBASSERT( !isConnected( ));
        _handlers.push_back( func );
    }

    virtual uint64_t getMaxVersions() const
    {
        return 50;
    }

private:
    void _init()
    {
        _localNode->registerPushHandler( _groupID,
                   boost::bind( &Receiver::_handleInit, this, _1, _2, _3, _4 ));
    }

    bool _connect()
    {
        _proxyNode->send( _initCmd );
        if( !_initialized.timedWaitEQ( true, co::Global::getKeepaliveTimeout()))
            return false;
        _processMappings();
        _objectMapVersion = _objectMap->getVersion();
        return true;
    }

    void _handleInit( const uint128_t& groupID, const uint128_t& typeID,
                      const UUID& objectID, co::DataIStream& istream )
    {
        if( groupID != _groupID || typeID != _typeInit )
            return;

        deserialize( istream, co::Serializable::DIRTY_ALL );
        _mapQueue.push_back( boost::bind( &co::LocalNode::mapObject,
                                          _localNode.get(), this, objectID,
                                          co::VERSION_NONE ));
        _initialized = true;
    }

    void _processMappings()
    {
        BOOST_FOREACH( const WorkFunc& func, _mapQueue )
        {
            func();
        }
        _mapQueue.clear();
    }

    void _processUnmappings()
    {
        BOOST_FOREACH( const IDSet::value_type& entry, _nodesToUnmap )
        {
            NodeMap::const_iterator i = _nodes.find( entry );
            _objectMap->unmap( i->second );
            _nodes.erase( i );
        }
        _nodesToUnmap.clear();
    }

    typedef stde::hash_map< UUID, Node* > NodeMap;
    typedef std::vector< VersionHandler > VersionHandlers;

    co::NodePtr _proxyNode;
    std::deque< WorkFunc > _mapQueue;
    IDSet _allNodes;
    IDSet _nodesToUnmap;
    NodeMap _nodes;
    lunchbox::MTQueue< uint128_t > _queuedVersions;
    uint128_t _objectMapVersion;
    lunchbox::Monitor<bool> _initialized;
    VersionHandlers _handlers;
};
}

Receiver::Receiver()
    : _impl( new detail::Receiver )
{
}

Receiver::Receiver( co::LocalNodePtr localNode )
    : _impl( new detail::Receiver( localNode ))
{
}

Receiver::~Receiver()
{
    delete _impl;
}

ReceiverPtr Receiver::create( const std::string& identifier,
                              co::LocalNodePtr localNode )
{
    lunchbox::ScopedWrite mutex( _receivers );
    ReceiversCIter i = _receivers->find( identifier );
    if( i != _receivers->end( ))
        return i->second;
    ReceiverPtr receiver = localNode ? new Receiver( localNode ) :
                                       new Receiver;
    (*_receivers)[identifier] = receiver;
    return receiver;
}

void Receiver::destroy( const std::string& identifier )
{
    lunchbox::ScopedWrite mutex( _receivers );
    ReceiversIter i = _receivers->find( identifier );
    if( i != _receivers->end( ))
        _receivers->erase( i );
}

void Receiver::destroy( ReceiverPtr receiver )
{
    lunchbox::ScopedWrite mutex( _receivers );
    for( ReceiversIter i = _receivers->begin(); i != _receivers->end(); ++i )
    {
        if( i->second == receiver )
        {
            _receivers->erase( i );
            break;
        }
    }
}

co::ConstLocalNodePtr Receiver::getLocalNode() const
{
    return _impl->getLocalNode();
}

co::Zeroconf Receiver::getZeroconf()
{
    return _impl->getZeroconf();
}

bool Receiver::connect( co::ConnectionDescriptionPtr conn )
{
    return _impl->connect( conn );
}

bool Receiver::connect( const co::NodeID& nodeID )
{
    return _impl->connect( nodeID );
}

bool Receiver::disconnect()
{
    return _impl->disconnect();
}

bool Receiver::isConnected() const
{
    return _impl->isConnected();
}

co::ConstConnectionDescriptionPtr Receiver::getConnection() const
{
    return _impl->getConnection();
}

dash::Context& Receiver::getContext()
{
    return _impl->getContext();
}

dash::Nodes Receiver::getNodes() const
{
    return _impl->getNodes();
}

dash::NodePtr Receiver::getNode( const UUID& identifier ) const
{
    return _impl->getNode( identifier );
}

dash::NodePtr Receiver::mapNode( const UUID& identifier )
{
    return _impl->mapNode( identifier );
}

bool Receiver::sync()
{
    return _impl->syncOne();
}

void Receiver::registerNewVersionHandler( const VersionHandler& func )
{
    return _impl->registerNewVersionHandler( func );
}

}
