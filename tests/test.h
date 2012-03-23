
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

#ifndef CODASHTEST_TEST_H
#define CODASHTEST_TEST_H

//#include <dash/detail/types.h>
#include <stdlib.h>
#include <lunchbox/log.h>
#include <lunchbox/sleep.h>
#include <lunchbox/thread.h>
#include <fstream>

#define OUTPUT co::base::Log::instance( __FILE__, __LINE__ )

#define TEST( x )                                                       \
    {                                                                   \
        EQVERB << "Test " << #x << std::endl;                           \
        if( !(x) )                                                      \
        {                                                               \
            OUTPUT << #x << " failed" << std::endl;                     \
            ::exit( EXIT_FAILURE );                                     \
        }                                                               \
    }

#define TESTINFO( x, info )                                             \
    {                                                                   \
        EQVERB << "Test " << #x << ": " << info << std::endl;           \
        if( !(x) )                                                      \
        {                                                               \
            OUTPUT << #x << " failed: " << info << std::endl;           \
            ::exit( EXIT_FAILURE );                                     \
        }                                                               \
    }

namespace
{
class Watchdog : public co::base::Thread
{
public:
    Watchdog( const std::string& name ) : _name( name ) {}

    virtual void run()
        {
            co::base::Thread::setName( "Watchdog" );
#ifdef DASH_TEST_RUNTIME
            co::base::sleep( DASH_TEST_RUNTIME * 1000 );
            TESTINFO( false, 
                      "Watchdog triggered - " << _name <<
                      " did not terminate within " << DASH_TEST_RUNTIME << "s" );
#else
            co::base::sleep( 60000 );
            TESTINFO( false, 
                      "Watchdog triggered - " << _name <<
                      " did not terminate within 1 minute" );
#endif
        }

private:
    const std::string _name;
};
}

int main( int argc, char **argv )
{
#ifndef CODASH_TEST_NO_WATCHDOG
    Watchdog watchdog( argv[0] );
    watchdog.start();
#endif

    const int result = codash::test::main( argc, argv );
    if( result != EXIT_SUCCESS )
        return result;

#ifndef CODASH_TEST_NO_WATCHDOG
    watchdog.cancel();
    co::base::sleep( 10 ); // give watchdog time to terminate
#endif
    return EXIT_SUCCESS;
}

#endif // CODASHTEST_TEST_H

