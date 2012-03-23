
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

#ifndef CODASH_API_H
#define CODASH_API_H

/**
 * @file codash/api.h
 *
 * Defines shared library symbol visibility macros.
 */

#if defined(_MSC_VER) || defined(__declspec)
#  define CODASH_DLLEXPORT __declspec(dllexport)
#  define CODASH_DLLIMPORT __declspec(dllimport)
#else // _MSC_VER
#  define CODASH_DLLEXPORT
#  define CODASH_DLLIMPORT
#endif // _MSC_VER

#if defined(CODASH_STATIC)
#  define CODASH_API
#elif defined(CODASH_SHARED)
#  define CODASH_API CODASH_DLLEXPORT
#else
#  define CODASH_API CODASH_DLLIMPORT
#endif

#endif // CODASH_API_H

