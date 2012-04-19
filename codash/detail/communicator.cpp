
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


namespace codash
{
namespace detail
{

Communicator::Communicator( int argc, char** argv,
                            co::ConnectionDescriptionPtr conn )
    : co::Serializable()
    , owner_( false )
    , context_()
    , localNode_()
    , objectMap_( 0 )
    , objectMapVersion_( co::VERSION_FIRST )
    , factory_()
{
    if( conn && co::init( argc, argv ))
    {
        localNode_ = new co::LocalNode;
        owner_ = true;
        localNode_->addConnectionDescription( conn );
        localNode_->listen();
        objectMap_ = new co::ObjectMap( *localNode_, factory_ );
    }
}

Communicator::Communicator( co::LocalNodePtr localNode )
    : co::Serializable()
    , owner_( false )
    , context_()
    , localNode_( localNode )
    , objectMap_( new co::ObjectMap( *localNode_, factory_ ))
    , objectMapVersion_( co::VERSION_FIRST )
    , factory_()
{}

Communicator::~Communicator()
{
    if( localNode_ )
    {
        if( objectMap_ )
            localNode_->releaseObject( objectMap_ );
        localNode_->releaseObject( this );
    }

    delete objectMap_;

    if( owner_ )
    {
        localNode_->close();
        co::exit();
    }
}

}
}
