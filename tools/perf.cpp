
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

#include <codash/codash.h>
#include <dash/dash.h>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include <boost/serialization/vector.hpp>
typedef std::vector< int > DataType;
SERIALIZABLEANY( DataType )


const uint16_t port = 4242u;

class Perftool
{
public:
    Perftool()
        : _connectString()
        , _numSends( 0xffffffffu )
        , _dataSize( 10000000u )
        , _mBytesSec( 0.f )
        , _mainCtx( dash::Context::getMain( ))
    {
    }

    ~Perftool()
    {
        _mainCtx.commit();
        delete &_mainCtx;
    }

    int run( int argc, char** argv )
    {
        if( !_parseArg( argc, argv ))
            return EXIT_SUCCESS;

        if( !_connectString.empty( ))
            return _receive();
        return _send();
    }

private:
    bool _parseArg( int argc, char** argv )
    {
        namespace arg = boost::program_options;
        arg::variables_map vm;
        arg::options_description desc( "CODASH perf tool" );
        desc.add_options()
            ( "help,h", "Output this help message")
            ( "version,v", "Show version" )
            ( "connect,c", arg::value< std::string >(),
              "Connect to remote node" )
            ( "dataSize,d", arg::value< uint32_t >()->default_value( _dataSize ),
              "Data size in bytes" )
            ( "numSends,n", arg::value< uint32_t >()->default_value( _numSends ),
              "Number of sends" )
            ;

        try
        {
            arg::store( arg::parse_command_line( argc, argv, desc ), vm );
            arg::notify( vm );
        }
        catch( ... )
        {
            std::cout << desc << std::endl;
            return false;
        }

        if( vm.count( "help" ))
        {
            std::cout << desc << std::endl;
            return false;
        }

        if( vm.count( "version" ))
        {
            std::cout << "CODASH perf tool version "
                      << codash::Version::getString() << std::endl;
            return false;
        }

        if( vm.count( "connect" ))
            _connectString = vm["connect"].as< std::string >();

        if( vm.count( "dataSize" ))
            _dataSize = vm["dataSize"].as< uint32_t >();

        if( vm.count( "numSends" ))
            _numSends = vm["numSends"].as< uint32_t >();

        _mBytesSec = _dataSize / 1024.0f / 1024.0f * 1000.0f;

        return true;
    }

    int _receive()
    {
        co::ConnectionDescriptionPtr remote = new co::ConnectionDescription;
        remote->port = port;
        remote->fromString( _connectString );

        codash::Receiver receiver( 0, 0 );
        if( !receiver.connect( remote ))
            return EXIT_FAILURE;

        dash::NodePtr node = receiver.getNodes()[0];
        {
            receiver.sync();
            dash::AttributePtr attr = node->getAttribute( 0 );
            _numSends = attr->get< uint32_t >();
            attr = node->getAttribute( 1 );
            _dataSize = attr->get< DataType >().size();
            _mBytesSec = _dataSize / 1024.0f / 1024.0f * 1000.0f;
        }

        lunchbox::Clock clock;
        while( receiver.sync( ))
        {
            dash::AttributePtr attr = node->getAttribute( 0 );
            const uint32_t value = attr->get< uint32_t >();

            const float time = clock.getTimef();
            std::cerr << "Recv perf: " << _mBytesSec / time << "MB/s ("
                      << 1 / time * 1000.f  << " receives/s)" << std::endl;
            clock.reset();

            if( value == _numSends )
                break;
        }
        return EXIT_SUCCESS;
    }

    int _send()
    {
        co::ConnectionDescriptionPtr conndesc = new co::ConnectionDescription;
        conndesc->port = port;
        codash::Sender sender( 0, 0, conndesc );

        dash::NodePtr node = new dash::Node;
        sender.registerNode( node );
        node->insert( new dash::Attribute( _numSends ));
        node->insert( new dash::Attribute( DataType( _dataSize )));
        sender.send( _mainCtx.commit( ));

        // wait for connection
        while( !sender.hasPeers( ))
            lunchbox::sleep( 500u );

        lunchbox::Clock clock;
        for( uint32_t i = 0; i < _numSends; ++i )
        {
            node->getAttribute( 0 )->set( i+1 );
            sender.send( _mainCtx.commit( ));

            const float time = clock.getTimef();
            std::cerr << "Send perf: " << _mBytesSec / time << "MB/s ("
                      << 1 / time * 1000.f  << " sends/s)" << std::endl;
            clock.reset();

            if( !sender.hasPeers( ))
                break;
        }

        // wait for receiver completion
        while( sender.hasPeers( ))
            lunchbox::sleep( 500u );

        return EXIT_SUCCESS;
    }

    std::string _connectString;
    uint32_t _numSends;
    uint32_t _dataSize;
    float _mBytesSec;
    dash::Context& _mainCtx;
};

int main( int argc, char** argv )
{
    Perftool perf;
    return perf.run( argc, argv );
}
