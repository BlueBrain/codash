
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

#include "communicator.h"

#include <co/connectionDescription.h>
#include <co/init.h>
#include <co/objectMap.h>

#include <dash/Node.h>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

namespace codash
{

static uint128_t groupID_(0x2c5886200d8346a6, 0x9142013e70d0c699);
static uint128_t typeInit_(0xb7d0b4e451094167, 0xb67d43efce1f75d2);

typedef Distributable< dash::Node > NodeDist;
typedef Distributable< dash::Commit,
                       boost::shared_ptr< dash::Commit > > CommitDist;


Communicator::Communicator()
    : co::Object()
    , proxyComm_( 0 )
{
}

bool Communicator::init( int argc, char** argv,
                         co::ConnectionDescriptionPtr conn )
{
    if( !conn || !co::init( argc, argv ))
        return false;

    localNode_ = new co::LocalNode;
    localNode_->addConnectionDescription( conn );
    if( !localNode_->listen( ))
        return false;

    localNode_->registerObject( this );
    objectMap_ = new co::ObjectMap( localNode_, factory_ );
    localNode_->registerObject( objectMap_ );
    localNode_->registerPushHandler( groupID_,
               boost::bind( &Communicator::_handleInit, this, _1, _2, _3, _4 ));

    return true;
}

bool Communicator::connect( co::ConnectionDescriptionPtr conn )
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

bool Communicator::finish()
{
    if( !localNode_ )
        return true;

    localNode_->deregisterObject( this );
    localNode_->deregisterObject( objectMap_ );
    delete objectMap_;
    objectMap_ = 0;
    delete proxyComm_;
    proxyComm_ = 0;

    if( proxyNode_ )
    {
        if( !localNode_->disconnect( proxyNode_ ))
            return false;
    }

    if( !localNode_->close( ))
        return false;

    return co::exit();
}

void Communicator::registerNode( dash::NodePtr node )
{
    dash::Context::getCurrent().map( node, context_ );

    NodeDistPtr nodeDist( new NodeDist( node ));
    nodeMap_[node] = nodeDist;
    objectMap_->register_( nodeDist.get(), OBJECTTYPE_NODE );
}

void Communicator::deregisterNode( dash::NodePtr node )
{
    NodeDistPtr nodeDist = nodeMap_[node];
    //objectMap_->deregister_( nodeDist );
    nodeMap_.erase( node );

    context_.unmap( node );
}

uint128_t Communicator::commit( const uint32_t incarnation )
{
    _processMappings();

    CommitPtr newCommit( new dash::Commit( context_.commit( )));
    CommitDistPtr commitDist( new CommitDist( newCommit ));
    commitMap_[newCommit] = commitDist;
    objectMap_->register_( commitDist.get(), OBJECTTYPE_COMMIT );
    objectMap_->commit();

    return co::Object::commit( incarnation );
}

uint128_t Communicator::sync( const uint128_t& version )
{
    _processMappings();

    objectMap_->sync();

    return co::Object::sync( version );
}

void Communicator::getInstanceData( co::DataOStream& os )
{
    os << objectMap_->getID();
}

void Communicator::applyInstanceData( co::DataIStream& is )
{
    uint128_t id;
    is >> id;
    workQueue_.push_back( boost::bind( &co::LocalNode::mapObject,
                        localNode_.get(), objectMap_, id, co::VERSION_OLDEST ));
}

void Communicator::_handleInit( const uint128_t& groupID,
                                const uint128_t& typeID,
                                const uint128_t& objectID,
                                co::DataIStream& istream )
{
    EQASSERT( groupID == groupID_ );
    EQASSERT( typeID == typeInit_ );

    proxyComm_ = new Communicator;
    proxyComm_->applyInstanceData( istream );
    workQueue_.push_back( boost::bind( &co::LocalNode::mapObject,
                    localNode_.get(), proxyComm_, objectID, co::VERSION_NONE ));
}

void Communicator::_processMappings()
{
    BOOST_FOREACH( const WorkFunc& func, workQueue_ )
    {
        func();
    }
    workQueue_.clear();
}

}
