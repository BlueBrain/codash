
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

#ifndef CODASH_OBJECTFACTORY_H
#define CODASH_OBJECTFACTORY_H

#include <codash/distributable.h>

#include <co/objectFactory.h>

#include <dash/node.h>


namespace codash
{
namespace detail
{

enum ObjectType
{
    OBJECTTYPE_NODE = co::OBJECTTYPE_CUSTOM
};

typedef Distributable< dash::Node > Node;

class ObjectFactory : public co::ObjectFactory
{
public:
    virtual co::Object* createObject( const uint32_t type )
    {
        switch( type )
        {
        case OBJECTTYPE_NODE:
            return new Node;
        default:
            return 0;
        }
    }
};

}
}

#endif
