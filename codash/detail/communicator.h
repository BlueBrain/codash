
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

#ifndef CODASH_DETAIL_COMMUNICATOR_H
#define CODASH_DETAIL_COMMUNICATOR_H

#include "objectFactory.h"

#include <co/serializable.h>

#include <dash/context.h>

namespace co { class ObjectMap; }

namespace codash
{
namespace detail
{

using lunchbox::uint128_t;

class Communicator : public co::Serializable
{
public:
    Communicator( int argc, char** argv, co::ConnectionDescriptionPtr conn );

    Communicator( co::LocalNodePtr localNode );

    virtual ~Communicator() = 0;

    dash::Context& getContext() { return context_; }

protected:
    virtual ChangeType getChangeType() const { return DELTA; }

    enum DirtyBits
    {
        DIRTY_NODES     = co::Serializable::DIRTY_CUSTOM << 0,
        DIRTY_COMMIT    = co::Serializable::DIRTY_CUSTOM << 1,
        DIRTY_OBJECTMAP = co::Serializable::DIRTY_CUSTOM << 2
    };

    bool owner_;
    dash::Context context_;
    co::LocalNodePtr localNode_;
    co::ObjectMap* objectMap_;
    ObjectFactory factory_;
};

}
}

#endif
