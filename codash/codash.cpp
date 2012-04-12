
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

#include <dash/Attribute.h>
#include <dash/Node.h>
#include <dash/Commit.h>

#include <dash/detail/Attribute.h>
#include <dash/detail/Node.h>
#include <dash/detail/Change.h>
#include <dash/detail/Commit.h>
#include <dash/detail/Serializable.h>

#include <co/dataOStreamArchive.h>
#include <co/dataIStreamArchive.h>


namespace dash
{

SERIALIZABLEREF( Attribute, co::DataOStreamArchive, co::DataIStreamArchive )
SERIALIZABLEREF( Node, co::DataOStreamArchive, co::DataIStreamArchive )
SERIALIZABLEPLAIN( Commit, co::DataOStreamArchive, co::DataIStreamArchive )

}
