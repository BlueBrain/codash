
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

#ifndef CODASH_COMMUNICATOR_H
#define CODASH_COMMUNICATOR_H

#include <co/object.h>

#include <dash/types.h>


namespace codash
{
namespace detail { class Communicator;}

using lunchbox::uint128_t;

/** */
class Communicator : public co::Object
{
public:
    Communicator();
    Communicator( int argc, char** argv, co::ConnectionDescriptionPtr conn );
    Communicator( co::LocalNodePtr localNode );
    ~Communicator();

    bool connect( co::ConnectionDescriptionPtr conn );

    dash::Context& getContext();

    void registerNode( dash::NodePtr node );

    void deregisterNode( dash::NodePtr node );

    virtual uint128_t commit( const uint32_t incarnation = CO_COMMIT_NEXT );

    virtual uint128_t sync( const uint128_t& version = co::VERSION_HEAD );

    virtual void nofifyNewNode( dash::NodePtr ) { EQINFO << "new node" << std::endl;}
    virtual void nofifyNewCommit( dash::Context& ) {}

protected:
    virtual void getInstanceData( co::DataOStream& os );
    virtual void applyInstanceData( co::DataIStream& is );
    virtual ChangeType getChangeType() const { return UNBUFFERED; }

private:
    friend class detail::Communicator;
    detail::Communicator* const impl_;
};

}

#endif
