
/* Copyright (c) 2012, EFPL/Blue Brain Project
 *                     Daniel Nachbaur <danie.nachbaur@epfl.ch>
 *
 * This file is part of DASH <https://github.com/BlueBrain/dash>
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

        dash::NodePtr node = new dash::Node;
        sender.registerNode( node );
        sender.commit();
        receiver.sync();
        TEST( receiver.getNodes().size() == 1 );
        dash::NodePtr newNode = receiver.getNodes()[0];
        TEST( *node == *newNode );

        node->insert( new dash::Attribute( 5 ));
        sender.commit();
        receiver.sync();
        TEST( receiver.getNodes().size() == 1 );
        TEST( newNode->getNAttributes() == 1 );
        TEST( *newNode->getAttribute( 0 ) == *node->getAttribute( 0 ));

        *node->getAttribute( 0 ) = 42;
        sender.commit();
        receiver.sync();
        TEST( newNode->getAttribute( 0 )->get<int>() == 42 );
    }
    mainCtx.commit();
    delete &mainCtx;

    return EXIT_SUCCESS;
}
