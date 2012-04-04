
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
#include "distributable.h"
#include "objectFactory.h"

#include <co/connectionDescription.h>
#include <co/init.h>
#include <co/objectMap.h>

#include <dash/Context.h>
#include <dash/Node.h>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/function/function0.hpp>

namespace codash
{

typedef Distributable< dash::Node > NodeDist;
typedef boost::shared_ptr< NodeDist > NodeDistPtr;
typedef boost::shared_ptr< dash::Commit > CommitPtr;
typedef Distributable< dash::Commit, CommitPtr > CommitDist;
typedef boost::shared_ptr< CommitDist > CommitDistPtr;

typedef std::map< dash::NodePtr, NodeDistPtr > NodeMap;
typedef std::map< CommitPtr, CommitDistPtr > CommitMap;

typedef boost::function< void() > WorkFunc;

typedef Distributable< dash::Node > NodeDist;
typedef Distributable< dash::Commit,
                       boost::shared_ptr< dash::Commit > > CommitDist;

static uint128_t groupID_(0x2c5886200d8346a6, 0x9142013e70d0c699);
static uint128_t typeInit_(0xb7d0b4e451094167, 0xb67d43efce1f75d2);


namespace detail
{
class Communicator
{
public:
    Communicator()
        : owner( false )
        , context()
        , localNode()
        , proxyNode()
        , proxyComm( 0 )
        , objectMap( 0 )
        , factory()
        , nodeMap()
        , commitMap()
        , workQueue()
    {}

    ~Communicator()
    {
        if( localNode )
        {
            localNode->deregisterObject( objectMap );

            if( proxyNode )
                localNode->disconnect( proxyNode );
        }

        delete proxyComm;
        delete objectMap;

        if( owner )
        {
            localNode->close();
            co::exit();
        }
    }

    void init()
    {
        objectMap = new co::ObjectMap( localNode, factory );
        localNode->registerObject( objectMap );
        localNode->registerPushHandler( groupID_,
                boost::bind( &Communicator::handleInit, this, _1, _2, _3, _4 ));
    }

    void handleInit( const uint128_t& groupID, const uint128_t& typeID,
                     const uint128_t& objectID, co::DataIStream& istream )
    {
        EQASSERT( groupID == groupID_ );
        EQASSERT( typeID == typeInit_ );

        proxyComm = new codash::Communicator;
        proxyComm->applyInstanceData( istream );
        workQueue.push_back( boost::bind( &co::LocalNode::mapObject,
                      localNode.get(), proxyComm, objectID, co::VERSION_NONE ));
    }

    void processMappings()
    {
        BOOST_FOREACH( const WorkFunc& func, workQueue )
        {
            func();
        }
        workQueue.clear();
    }

    bool owner;

    dash::Context context;

    co::LocalNodePtr localNode;
    co::NodePtr proxyNode;
    codash::Communicator* proxyComm;

    co::ObjectMap* objectMap;
    ObjectFactory factory;

    NodeMap nodeMap;
    CommitMap commitMap;

    std::deque<WorkFunc> workQueue;
};
}

Communicator::Communicator()
    : co::Object()
    , impl_( new detail::Communicator )
{
}

Communicator::Communicator( int argc, char** argv,
                            co::ConnectionDescriptionPtr conn )
    : co::Object()
    , impl_( new detail::Communicator )
{
    if( conn && co::init( argc, argv ))
    {
        impl_->localNode = new co::LocalNode;
        impl_->owner = true;
        impl_->localNode->addConnectionDescription( conn );
        impl_->localNode->listen();
    }
    impl_->localNode->registerObject( this );
    impl_->init();
}

Communicator::Communicator( co::LocalNodePtr localNode )
    : co::Object()
    , impl_( new detail::Communicator )
{
    impl_->localNode = localNode;
    impl_->localNode->registerObject( this );
    impl_->init();
}

Communicator::~Communicator()
{
    if( impl_->localNode )
        impl_->localNode->deregisterObject( this );
    delete impl_;
}

bool Communicator::connect( co::ConnectionDescriptionPtr conn )
{
    if( !conn )
        return false;

    impl_->proxyNode = new co::Node;
    impl_->proxyNode->addConnectionDescription( conn );
    if( !impl_->localNode->connect( impl_->proxyNode ))
        return false;

    co::Nodes nodes;
    nodes.push_back( impl_->proxyNode );
    push( groupID_, typeInit_, nodes );
    return true;
}

dash::Context& Communicator::getContext()
{
    return impl_->context;
}

void Communicator::registerNode( dash::NodePtr node )
{
    dash::Context::getCurrent().map( node, impl_->context );

    NodeDistPtr nodeDist( new NodeDist( node ));
    impl_->nodeMap[node] = nodeDist;
    impl_->objectMap->register_( nodeDist.get(), OBJECTTYPE_NODE );
}

void Communicator::deregisterNode( dash::NodePtr node )
{
    //NodeDistPtr nodeDist = nodeMap_[node];
    //impl_->objectMap->deregister_( nodeDist );
    impl_->nodeMap.erase( node );

    impl_->context.unmap( node );
}

uint128_t Communicator::commit( const uint32_t incarnation )
{
    impl_->processMappings();

    CommitPtr newCommit( new dash::Commit( impl_->context.commit( )));
    CommitDistPtr commitDist( new CommitDist( newCommit ));
    impl_->commitMap[newCommit] = commitDist;
    impl_->objectMap->register_( commitDist.get(), OBJECTTYPE_COMMIT );
    impl_->objectMap->commit();

    return co::Object::commit( incarnation );
}

uint128_t Communicator::sync( const uint128_t& version )
{
    impl_->processMappings();

    impl_->objectMap->sync();

    return co::Object::sync( version );
}

void Communicator::getInstanceData( co::DataOStream& os )
{
    os << impl_->objectMap->getID();
}

void Communicator::applyInstanceData( co::DataIStream& is )
{
    uint128_t id;
    is >> id;
    impl_->workQueue.push_back( boost::bind( &co::LocalNode::mapObject,
            impl_->localNode.get(), impl_->objectMap, id, co::VERSION_OLDEST ));
}

}
