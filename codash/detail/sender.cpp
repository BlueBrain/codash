
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

    localNode_->registerObject( objectMap_ );
    localNode_->registerObject( this );
    localNode_->registerCommandHandler( initCmd_,
                                  boost::bind( &Sender::cmdConnect_, this, _1 ),
                                  localNode_->getCommandThreadQueue( ));
}

bool Sender::cmdConnect_( co::Command& command )
{
    co::Nodes nodes( 1, command.getNode( ));
    push( groupID_, typeInit_, nodes );
    return true;
}

void Sender::registerNode( dash::NodePtr dashNode )
{
    dash::Context::getCurrent().map( dashNode, context_ );

    NodePtr node( new Node( dashNode ));
    nodeMap_[ dashNode ] = node;
    setDirty( DIRTY_NODES );

    objectMap_->register_( node.get(), OBJECTTYPE_NODE );
}

void Sender::deregisterNode( dash::NodePtr dashNode )
{
    // TODO: need deregister_ func in objectMap!
    //NodePtr node = nodeMap_[ dashNode ];
    //objectMap_->deregister( node.get( ));
    nodeMap_.erase( dashNode );

    context_.unmap( dashNode );
}

void Sender::commit()
{
    dash::CommitPtr newCommit( new dash::Commit( context_.commit( )));
    if( !commit_ )
    {
        commit_.reset( new Commit( newCommit ));
        objectMap_->register_( commit_.get(), OBJECTTYPE_COMMIT );
        setDirty( DIRTY_COMMIT );
    }
    else
        commit_->setValue( newCommit );

    const uint128_t oldVersion = objectMap_->getVersion();
    if( objectMap_->commit() > oldVersion )
        setDirty( DIRTY_OBJECTMAP );

    Communicator::commit();
}

void Sender::serialize( co::DataOStream& os, const uint64_t dirtyBits )
{
    if( dirtyBits & DIRTY_NODES )
    {
        os << static_cast< uint64_t >( nodeMap_.size( ));
        BOOST_FOREACH( const NodeMap::value_type& entry, nodeMap_ )
        {
            os << entry.second->getID();
        }
    }
    if( dirtyBits & DIRTY_COMMIT )
        os << (commit_ ? commit_->getID() : uint128_t::ZERO);
    if( dirtyBits & DIRTY_OBJECTMAP )
        os << co::ObjectVersion( objectMap_ );

    Communicator::serialize( os, dirtyBits );
}

void Sender::deserialize( co::DataIStream& is, const uint64_t dirtyBits )
{
    LBDONTCALL;
    Communicator::deserialize( is, dirtyBits );
}

}
}
