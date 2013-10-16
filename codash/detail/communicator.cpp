
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

#include "communicator.h"

#include <co/connectionDescription.h>
#include <co/objectMap.h>


namespace codash
{
namespace detail
{

Communicator::Communicator( co::ConnectionDescriptionPtr conn )
    : co::Serializable()
    , _context()
    , _localNode()
    , _objectMap( 0 )
    , _factory()
    , _owner( true )
{
    _localNode = new co::LocalNode;
    if( conn )
        _localNode->addConnectionDescription( conn );
    if( !_localNode->listen( ))
        throw std::runtime_error( "Listening connection setup failed" );

    _objectMap = new co::ObjectMap( *_localNode, _factory );
}

Communicator::Communicator( co::LocalNodePtr localNode )
    : co::Serializable()
    , _context()
    , _localNode( localNode )
    , _objectMap( new co::ObjectMap( *_localNode, _factory ))
    , _factory()
    , _owner( false )
{}

Communicator::~Communicator()
{
    if( _localNode )
    {
        if( _objectMap )
            _localNode->releaseObject( _objectMap );
        _localNode->releaseObject( this );
    }

    delete _objectMap;

    if( _owner )
    {
        _localNode->close();
        LBASSERT( _localNode->getRefCount() == 1 );
        _localNode = 0;
    }
}

}
}
