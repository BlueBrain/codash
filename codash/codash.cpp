
/* Copyright (c) 2012, EFPL/Blue Brain Project
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

#include "api.h"

#include <dash/attribute.h>
#include <dash/node.h>
#include <dash/commit.h>

#include <dash/detail/attribute.h>
#include <dash/detail/node.h>
#include <dash/detail/change.h>
#include <dash/detail/commit.h>
#include <dash/detail/serializable.h>

#include <co/dataOStreamArchive.h>
#include <co/dataIStreamArchive.h>


namespace dash
{

// TODO: temp workaround for Win32 build
#undef DASH_API
#define DASH_API CODASH_API
SERIALIZABLEREF( Attribute, co::DataOStreamArchive, co::DataIStreamArchive )
SERIALIZABLEREF( Node, co::DataOStreamArchive, co::DataIStreamArchive )
SERIALIZABLEPLAIN( Commit, co::DataOStreamArchive, co::DataIStreamArchive )

}
