
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

#include "receiver.h"
#include "detail/receiver.h"


namespace codash
{

Receiver::Receiver( int argc, char** argv )
    : impl_( new detail::Receiver( argc, argv ))
{
}

Receiver::Receiver( co::LocalNodePtr localNode )
    : impl_( new detail::Receiver( localNode ))
{
}

Receiver::~Receiver()
{
    delete impl_;
}

co::ConstLocalNodePtr Receiver::getNode() const
{
    return impl_->getNode();
}

co::Zeroconf Receiver::getZeroconf()
{
    return impl_->getZeroconf();
}

bool Receiver::connect( co::ConnectionDescriptionPtr conn )
{
    return impl_->connect( conn );
}

bool Receiver::connect( const co::NodeID& nodeID )
{
    return impl_->connect( nodeID );
}

bool Receiver::disconnect()
{
    return impl_->disconnect();
}

bool Receiver::isConnected() const
{
    return impl_->isConnected();
}

co::ConnectionDescriptionPtr Receiver::getConnection() const
{
    return impl_->getConnection();
}

dash::Context& Receiver::getContext()
{
    return impl_->getContext();
}

const dash::Nodes& Receiver::getNodes() const
{
    return impl_->getNodes();
}

bool Receiver::sync()
{
    return impl_->sync();
}

}
