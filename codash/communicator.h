
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

#ifndef CODASH_COMMUNICATOR_H
#define CODASH_COMMUNICATOR_H

#include <codash/objectFactory.h>
#include <codash/distributable.h>

#include <co/object.h>

#include <dash/Context.h>

#include <boost/function/function0.hpp>

namespace co { class ObjectMap; }

namespace codash
{

using lunchbox::uint128_t;

/** */
class Communicator : public co::Object
{
public:
    Communicator();

    bool init( int argc, char** argv, co::ConnectionDescriptionPtr conn );

    bool finish();

    bool connect( co::ConnectionDescriptionPtr conn );

    dash::Context& getContext() { return context_; }

    void registerNode( dash::NodePtr node );

    void deregisterNode( dash::NodePtr node );

    virtual uint128_t commit( const uint32_t incarnation = CO_COMMIT_NEXT );

    virtual uint128_t sync( const uint128_t& version = co::VERSION_HEAD );

    virtual void nofifyNewNode( dash::NodePtr ) {}
    virtual void nofifyNewCommit( dash::Context& ) {}

protected:
    virtual void getInstanceData( co::DataOStream& os );
    virtual void applyInstanceData( co::DataIStream& is );
    virtual ChangeType getChangeType() const { return UNBUFFERED; }

private:
    void _handleInit( const uint128_t& groupID, const uint128_t& typeID,
                      const uint128_t& objectID, co::DataIStream& istream );

    void _processMappings();

    friend class Client;

    dash::Context context_;

    co::LocalNodePtr localNode_;
    co::NodePtr proxyNode_;
    Communicator* proxyComm_;

    co::ObjectMap* objectMap_;
    ObjectFactory factory_;

    typedef Distributable< dash::Node > NodeDist;
    typedef boost::shared_ptr< NodeDist > NodeDistPtr;
    typedef boost::shared_ptr< dash::Commit > CommitPtr;
    typedef Distributable< dash::Commit, CommitPtr > CommitDist;
    typedef boost::shared_ptr< CommitDist > CommitDistPtr;

    typedef std::map< dash::NodePtr, NodeDistPtr > NodeMap;
    typedef std::map< CommitPtr, CommitDistPtr > CommitMap;

    NodeMap nodeMap_;
    CommitMap commitMap_;

    typedef boost::function< void() > WorkFunc;
    std::deque<WorkFunc> workQueue_;
};

}

#endif
