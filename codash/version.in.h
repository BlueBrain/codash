
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

#ifndef CODASH_VERSION_H
#define CODASH_VERSION_H

#include <codash/api.h>
#include <lunchbox/types.h>
#include <string>

namespace codash
{
    // CoDASH version macros and functions
    /** The current major version. @version 0.1 */
#   define CODASH_VERSION_MAJOR @VERSION_MAJOR@

    /** The current minor version. @version 0.1 */
#   define CODASH_VERSION_MINOR @VERSION_MINOR@

    /** The current patch level. @version 0.1 */
#   define CODASH_VERSION_PATCH @VERSION_PATCH@

    /** The git wc hash revision, may be 0. @version 0.1 */
#   define CODASH_VERSION_REVISION @GIT_REVISION@

    /** The current DSO binary revision. @version 0.1 */
#   define CODASH_VERSION_ABI @VERSION_ABI@

    /** The Collage version used to compile codash. @version 0.1 */
#   define CODASH_COLLAGE_VERSION @COLLAGE_VERSION@

    /** The Dash version used to compile codash. @version 0.1 */
#   define CODASH_DASH_VERSION @DASH_VERSION@

/** True if the current version is newer than the given one. @version 0.1 */
#   define CODASH_VERSION_GT( MAJOR, MINOR, PATCH )                   \
    ( (CODASH_VERSION_MAJOR>MAJOR) ||                                 \
      (CODASH_VERSION_MAJOR==MAJOR &&                                 \
       (CODASH_VERSION_MINOR>MINOR ||                                 \
        (CODASH_VERSION_MINOR==MINOR && CODASH_VERSION_PATCH>PATCH))))

/** True if the current version is equal or newer to the given. @version 0.1 */
#   define CODASH_VERSION_GE( MAJOR, MINOR, PATCH )                         \
    ( (CODASH_VERSION_MAJOR>MAJOR) ||                                       \
      (CODASH_VERSION_MAJOR==MAJOR &&                                 \
       (CODASH_VERSION_MINOR>MINOR ||                                 \
        (CODASH_VERSION_MINOR==MINOR && CODASH_VERSION_PATCH>=PATCH))))

/** True if the current version is older than the given one. @version 0.1 */
#   define CODASH_VERSION_LT( MAJOR, MINOR, PATCH )                         \
    ( (CODASH_VERSION_MAJOR<MAJOR) ||                                       \
      (CODASH_VERSION_MAJOR==MAJOR &&                                 \
       (CODASH_VERSION_MINOR<MINOR ||                                 \
        (CODASH_VERSION_MINOR==MINOR && CODASH_VERSION_PATCH<PATCH))))

/** True if the current version is older or equal to the given. @version 0.1 */
#   define CODASH_VERSION_LE( MAJOR, MINOR, PATCH )                         \
    ( (CODASH_VERSION_MAJOR<MAJOR) ||                                       \
      (CODASH_VERSION_MAJOR==MAJOR &&                                 \
       (CODASH_VERSION_MINOR<MINOR ||                                 \
        (CODASH_VERSION_MINOR==MINOR && CODASH_VERSION_PATCH<=PATCH))))

    /** Information about the current CoDASH version. */
    class CODASH_API Version
    {
    public:
        /** @return the current major version of CoDASH. @version 0.1 */
        static uint32_t getMajor();

        /** @return the current minor version of CoDASH. @version 0.1 */
        static uint32_t getMinor();

        /** @return the current patch level of CoDASH. @version 0.1 */
        static uint32_t getPatch();

        /** @return the current revision of CoDASH. @version 0.1 */
        static std::string getRevision();

        /** @return the current DSO binary revision. @version 0.1 */
        static uint32_t getABI();

        /** @return the current CoDASH version (MMmmpp). @version 0.1 */
        static uint32_t getInt();

        /** @return the current CoDASH version (MM.mmpp). @version 0.1 */
        static float    getFloat();

        /**
         * @return the current CoDASH version (MM.mm.pp[.rr]).
         * @version 1.1
         */
        static std::string getString();

        /**
         * @return true if the link-time and compile-time DSO versions are
         *         compatible.
         */
        static bool check()
        { return getMajor()==CODASH_VERSION_MAJOR &&
                 getMinor()==CODASH_VERSION_MINOR; }
    };
}

#endif //CODASH_VERSION_H
