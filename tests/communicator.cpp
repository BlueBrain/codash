
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

#include "test.h"
#include "serialize.h"

#include <codash/codash.h>
#include <co/co.h>
#include <dash/dash.h>

#ifdef __APPLE__
// following boost include causes shadowing warning. See also:
// https://github.com/BlueBrain/dash/issues/2
#  pragma GCC diagnostic ignored "-Wshadow"
#endif
#include <boost/serialization/vector.hpp>
SERIALIZABLEANY( std::vector< int > )


int codash::test::main( int argc, char **argv )
{
    const codash::UUID nodeID( lunchbox::make_uint128( "codash::test::nodeID" ));
    const codash::UUID lateNodeID( lunchbox::make_uint128( "codash::test::lateNodeID" ));
    dash::Context& mainCtx = dash::Context::getMain( argc, argv );
    {
        lunchbox::RNG rng;
        co::ConnectionDescriptionPtr sendDesc = new co::ConnectionDescription;
        sendDesc->type = co::CONNECTIONTYPE_TCPIP;
        sendDesc->port = (rng.get<uint16_t>() % 60000) + 1024;
        sendDesc->setHostname( "localhost" );

        codash::Sender sender( sendDesc );
        codash::Receiver receiver;

        TEST( receiver.connect( sendDesc ));

        receiver.sync();
        TEST( receiver.getNodes().empty( ));

        dash::NodePtr node = new dash::Node;
        dash::NodePtr lateNode = new dash::Node;

        // register 2, map 1 node
        TEST( sender.registerNode( node, nodeID ));
        TEST( sender.registerNode( lateNode, lateNodeID ));
        sender.send( mainCtx.commit( ));
        receiver.sync();
        dash::NodePtr newNode = receiver.mapNode( nodeID );
        TEST( newNode );
        TEST( receiver.getNodes().size() == 1 );
        TEST( newNode == receiver.getNodes()[0] );
        TEST( *node == *newNode );

        // add attributes
        node->insert( new dash::Attribute( 5 ));
        lateNode->insert( new dash::Attribute( 1 ));
        sender.send( mainCtx.commit( ));
        receiver.sync();
        TEST( receiver.getNodes().size() == 1 );
        TEST( newNode->getNAttributes() == 1 );
        TEST( *newNode->getAttribute( 0 ) == *node->getAttribute( 0 ));
        TEST( newNode->getAttribute( 0 )->get<int>() == 5 );

        // for second node: change lateNode attribute
        lateNode->getAttribute( 0 )->set( 5 );

        // change 1 attribute
        node->getAttribute( 0 )->set( 42 );
        sender.send( mainCtx.commit( ));
        receiver.sync();
        TEST( newNode->getAttribute( 0 )->get<int>() == 42 );

        // change data type of attribute
        std::vector< int > vec( 10, 17 );
        node->getAttribute( 0 )->set( vec );
        sender.send( mainCtx.commit( ));
        receiver.sync();
        TEST( newNode->getAttribute( 0 )->get< std::vector< int > >().size() == 10 );
        TEST( newNode->getAttribute( 0 )->get< std::vector< int > >()[3] == 17 );

        // late map second node
        dash::NodePtr newLateNode = receiver.mapNode( lateNodeID );
        TEST( newLateNode );
        TEST( receiver.getNodes().size() == 2 );
        TEST( newLateNode == receiver.getNode( lateNodeID ));
        TEST( *lateNode == *newLateNode );
        TEST( newLateNode->getAttribute( 0 )->get<int>() == 5 );

        TEST( sender.deregisterNode( nodeID ));
        TEST( sender.deregisterNode( lateNodeID ));
        sender.send( mainCtx.commit( ));
        receiver.sync();
        TEST( receiver.getNodes().empty( ));
    }
    mainCtx.commit();
    delete &mainCtx;

    return EXIT_SUCCESS;
}
