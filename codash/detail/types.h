
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

#ifndef CODASH_DETAIL_TYPES_H
#define CODASH_DETAIL_TYPES_H

#include <codash/objectFactory.h>


namespace codash
{
namespace detail
{

using lunchbox::uint128_t;

static uint128_t groupID_( 0x2C5886200D8346A6ull, 0x9142013E70D0C699ull );
static uint128_t typeInit_( 0xB7D0B4E451094167ull, 0xB67D43EFCE1F75D2ull );

typedef boost::shared_ptr< NodeDist > NodeDistPtr;
typedef boost::shared_ptr< dash::Commit > CommitPtr;
typedef boost::shared_ptr< CommitDist > CommitDistPtr;

}
}

#endif
