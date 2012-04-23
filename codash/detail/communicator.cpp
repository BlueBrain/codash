
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
    , context_()
    , localNode_()
    , objectMap_( 0 )
    , factory_()
    , owner_( true )
{
    if( !co::init( argc, argv )) // exception?
        return;

    localNode_ = new co::LocalNode;
    if( conn )
        localNode_->addConnectionDescription( conn );
    if( !localNode_->listen( ))
        return;

    objectMap_ = new co::ObjectMap( *localNode_, factory_ );
}

Communicator::Communicator( co::LocalNodePtr localNode )
    : co::Serializable()
    , context_()
    , localNode_( localNode )
    , objectMap_( new co::ObjectMap( *localNode_, factory_ ))
    , factory_()
    , owner_( false )
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
        LBASSERT( localNode_->getRefCount() == 1 );
        localNode_ = 0;
        co::exit();
    }
}

}
}
