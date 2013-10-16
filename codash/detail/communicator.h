
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

#ifndef CODASH_DETAIL_COMMUNICATOR_H
#define CODASH_DETAIL_COMMUNICATOR_H

#include "objectFactory.h"

#include <co/serializable.h>
#include <co/zeroconf.h>

#include <dash/context.h>

namespace co { class ObjectMap; }

namespace codash
{
namespace detail
{

typedef lunchbox::Lockable< dash::Context, lunchbox::SpinLock > Context;

class Communicator : public co::Serializable
{
public:
    explicit Communicator( co::ConnectionDescriptionPtr conn );

    explicit Communicator( co::LocalNodePtr localNode );

    virtual ~Communicator() = 0;

    co::ConstLocalNodePtr getNode() const { return _localNode; }

    co::Zeroconf getZeroconf() { return _localNode->getZeroconf(); }

    dash::Context& getContext() { return *_context; }

protected:
    virtual ChangeType getChangeType() const { return DELTA; }

    enum DirtyBits
    {
        DIRTY_NODES     = co::Serializable::DIRTY_CUSTOM << 0,
        DIRTY_OBJECTMAP = co::Serializable::DIRTY_CUSTOM << 1
    };

    Context _context;
    co::LocalNodePtr _localNode;
    co::ObjectMap* _objectMap;
    ObjectFactory _factory;
    const bool _owner;
};

}
}

#endif
