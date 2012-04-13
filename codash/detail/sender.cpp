
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

#include "sender.h"

#include <co/connectionDescription.h>
#include <co/objectMap.h>

#include <boost/foreach.hpp>


namespace codash
{
namespace detail
{

Sender::Sender( int argc, char** argv, co::ConnectionDescriptionPtr conn )
    : Communicator( argc, argv, conn )
    , proxyNode_()
    , nodeMap_()
    , commitMap_()
{
    init_();
}

Sender::Sender( co::LocalNodePtr localNode )
    : Communicator( localNode )
    , proxyNode_()
    , nodeMap_()
    , commitMap_()
{
    init_();
}

Sender::~Sender()
{
    localNode_->releaseObject( objectMap_ );
    delete objectMap_;
    objectMap_ = 0;
    nodeMap_.clear();
    commitMap_.clear();

    if( proxyNode_ )
        localNode_->disconnect( proxyNode_ );
}

void Sender::init_()
{
    if( !localNode_ )
        return;

    localNode_->registerObject( this );
    localNode_->registerObject( objectMap_ );
}

bool Sender::connectReceiver( co::ConnectionDescriptionPtr conn )
{
    if( !conn )
        return false;

    proxyNode_ = new co::Node;
    proxyNode_->addConnectionDescription( conn );
    if( !localNode_->connect( proxyNode_ ))
        return false;

    co::Nodes nodes;
    nodes.push_back( proxyNode_ );
    push( groupID_, typeInit_, nodes );
    return true;
}

void Sender::registerNode( dash::NodePtr node )
{
    dash::Context::getCurrent().map( node, context_ );

    NodeDistPtr nodeDist( new NodeDist( node ));
    nodeMap_[node] = nodeDist;
    setDirty( DIRTY_NODES );

    objectMap_->register_( nodeDist.get(), OBJECTTYPE_NODE );
}

void Sender::deregisterNode( dash::NodePtr node )
{
    // TODO: need deregister_ func in objectMap!
    //NodeDistPtr nodeDist = nodeMap_[node];
    //impl_->objectMap->deregister_( nodeDist );
    nodeMap_.erase( node );

    context_.unmap( node );
}

uint128_t Sender::commit( const uint32_t incarnation )
{
    CommitPtr newCommit( new dash::Commit( context_.commit( )));
    CommitDistPtr commitDist( new CommitDist( newCommit ));
    commitMap_[newCommit] = commitDist;
    setDirty( DIRTY_COMMITS );

    objectMap_->register_( commitDist.get(), OBJECTTYPE_COMMIT );
    objectMap_->commit();

    return co::Object::commit( incarnation );
}

void Sender::serialize( co::DataOStream& os, const uint64_t dirtyBits )
{
    if( dirtyBits == co::Serializable::DIRTY_ALL )
    {
        os << objectMap_->getID();
    }

    if( dirtyBits & DIRTY_NODES )
    {
        os << static_cast< uint64_t >( nodeMap_.size( ));
        BOOST_FOREACH( const NodeMap::value_type& entry, nodeMap_ )
        {
            os << entry.second->getID();
        }
    }
    if( dirtyBits & DIRTY_COMMITS )
    {
        os << static_cast< uint64_t >( commitMap_.size( ));
        BOOST_FOREACH( const CommitMap::value_type& entry, commitMap_ )
        {
            os << entry.second->getID();
        }
    }
}

void Sender::deserialize( co::DataIStream& is, const uint64_t dirtyBits )
{
    LBDONTCALL
}

}
}
