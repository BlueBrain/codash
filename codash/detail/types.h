
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

#ifndef CODASH_DETAIL_TYPES_H
#define CODASH_DETAIL_TYPES_H

#include "objectFactory.h"

#include <codash/distributable.h>
#include <codash/types.h>
#include <dash/node.h>

namespace codash
{
namespace detail
{

using lunchbox::make_uint128;

static uint128_t _groupID( make_uint128( "codash::detail::_groupID" ));
static uint128_t _typeInit( make_uint128( "codash::detail::_typeInit" ));
static uint128_t _initCmd( make_uint128( "codash::detail::_initCmd" ));

typedef Distributable< dash::Node > Node;
typedef boost::shared_ptr< Node > NodePtr;

typedef stde::hash_map< uint128_t, uint128_t > IDMap;

}
}

#endif
