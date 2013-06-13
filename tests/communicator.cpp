
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
    dash::Context& mainCtx = dash::Context::getMain( argc, argv );
    {
        lunchbox::RNG rng;
        co::ConnectionDescriptionPtr sendDesc = new co::ConnectionDescription;
        sendDesc->type = co::CONNECTIONTYPE_TCPIP;
        sendDesc->port = (rng.get<uint16_t>() % 60000) + 1024;
        sendDesc->setHostname( "localhost" );

        codash::Sender sender( argc, argv, sendDesc );
        codash::Receiver receiver( argc, argv );

        TEST( receiver.connect( sendDesc ));

        receiver.sync();
        TEST( receiver.getNodes().empty( ));

        dash::NodePtr node = new dash::Node;
        sender.registerNode( node );
        sender.send( mainCtx.commit( ));
        receiver.sync();
        TEST( receiver.getNodes().size() == 1 );
        dash::NodePtr newNode = receiver.getNodes()[0];
        TEST( *node == *newNode );

        node->insert( new dash::Attribute( 5 ));
        sender.send( mainCtx.commit( ));
        receiver.sync();
        TEST( receiver.getNodes().size() == 1 );
        TEST( newNode->getNAttributes() == 1 );
        TEST( *newNode->getAttribute( 0 ) == *node->getAttribute( 0 ));
        TEST( newNode->getAttribute( 0 )->get<int>() == 5 );

        node->getAttribute( 0 )->set( 42 );
        sender.send( mainCtx.commit( ));
        receiver.sync();
        TEST( newNode->getAttribute( 0 )->get<int>() == 42 );

        std::vector< int > vec( 10, 17 );
        node->getAttribute( 0 )->set( vec );
        sender.send( mainCtx.commit( ));
        receiver.sync();
        TEST( newNode->getAttribute( 0 )->get< std::vector< int > >().size() == 10 );
        TEST( newNode->getAttribute( 0 )->get< std::vector< int > >()[3] == 17 );

        sender.deregisterNode( node );
        sender.send( mainCtx.commit( ));
        receiver.sync();
    }
    mainCtx.commit();
    delete &mainCtx;

    return EXIT_SUCCESS;
}
