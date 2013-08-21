
/* Copyright (c) 2012-2013, EPFL/Blue Brain Project
 *                          Daniel Nachbaur <daniel.nachbaur@epfl.ch>
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

#ifndef CODASH_TYPES_H
#define CODASH_TYPES_H

#include <lunchbox/types.h>
#include <lunchbox/refPtr.h>

namespace codash
{

class Receiver;
class Sender;

typedef lunchbox::RefPtr< Receiver > ReceiverPtr;

using lunchbox::uint128_t;
using lunchbox::UUID;

}

#endif // CODASH_TYPES_H
