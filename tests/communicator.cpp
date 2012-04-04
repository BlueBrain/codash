
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
        co::ConnectionDescriptionPtr connDesc = new co::ConnectionDescription;

        lunchbox::RNG rng;
        connDesc->type = co::CONNECTIONTYPE_TCPIP;
        connDesc->port = (rng.get<uint16_t>() % 60000) + 1024;
        connDesc->setHostname( "localhost" );

        codash::Communicator sender;
        TEST( sender.init( argc, argv, connDesc ));

        co::ConnectionDescriptionPtr connDesc2 = new co::ConnectionDescription;
        connDesc2->type = co::CONNECTIONTYPE_TCPIP;
        connDesc2->setHostname( "localhost" );
        codash::Communicator receiver;
        TEST( receiver.init( argc, argv, connDesc2 ));
        TEST( receiver.connect( connDesc ));

        dash::NodePtr node = new dash::Node;
        sender.registerNode( node );
        sender.commit();

        receiver.sync();

        TEST( receiver.finish( ));
        TEST( sender.finish( ));
    }

    delete &mainCtx;

    return EXIT_SUCCESS;
}
