
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
#include "objectMap.h"

#include <co/command.h>
#include <co/connectionDescription.h>
#include <co/objectMap.h>

#include <boost/foreach.hpp>
#include <boost/bind.hpp>


namespace codash
{
namespace detail
{

Sender::Sender( int argc, char** argv, co::ConnectionDescriptionPtr conn )
    : Communicator( argc, argv, conn )
    , nodeMap_()
    , commit_()
{
    init_();
}

Sender::Sender( co::LocalNodePtr localNode )
    : Communicator( localNode )
    , nodeMap_()
    , commit_()
{
    init_();
}

Sender::~Sender()
{
    localNode_->releaseObject( objectMap_ );
    delete objectMap_;
    objectMap_ = 0;
    nodeMap_.clear();
}

void Sender::init_()
{
    if( !localNode_ )
        return;

    localNode_->registerObject( this );
    localNode_->registerObject( objectMap_ );
    localNode_->registerCommandHandler( initCmd_,
                                  boost::bind( &Sender::cmdConnect_, this, _1 ),
                                  localNode_->getCommandThreadQueue( ));
}

bool Sender::cmdConnect_( co::Command& command )
{
    co::Nodes nodes;
    nodes.push_back( command.getNode( ));
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
    //objectMap_->deregister( nodeDist.get( ));
    nodeMap_.erase( node );

    context_.unmap( node );
}

uint128_t Sender::commit( const uint32_t incarnation )
{
    CommitPtr newCommit( new dash::Commit( context_.commit( )));
    if( !commit_ )
    {
        commit_.reset( new CommitDist( newCommit ));
        objectMap_->register_( commit_.get(), OBJECTTYPE_COMMIT );
        setDirty( DIRTY_COMMIT );
    }
    else
        commit_->setValue( newCommit );

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
    if( dirtyBits & DIRTY_COMMIT )
    {
        os << (commit_ ? commit_->getID() : uint128_t::ZERO);
    }
    Communicator::serialize( os, dirtyBits );
}

void Sender::deserialize( co::DataIStream& is, const uint64_t dirtyBits )
{
    LBDONTCALL;
    Communicator::deserialize( is, dirtyBits );
}

}
}
