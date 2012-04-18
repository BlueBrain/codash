
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

#ifndef CODASH_DETAIL_SENDER_H
#define CODASH_DETAIL_SENDER_H

#include "communicator.h"
#include "types.h"

#include <map>

namespace co { class Command; }

namespace codash
{
namespace detail
{

class Sender : public Communicator
{
public:
    Sender( int argc, char** argv, co::ConnectionDescriptionPtr conn );

    Sender( co::LocalNodePtr localNode );

    ~Sender();

    void registerNode( dash::NodePtr node );

    void deregisterNode( dash::NodePtr node );

    void commit();

protected:
    virtual void serialize( co::DataOStream& os, const uint64_t dirtyBits );
    virtual void deserialize( co::DataIStream& is, const uint64_t dirtyBits );

private:
    void init_();
    bool cmdConnect_( co::Command& command );

    typedef std::map< dash::NodePtr, NodeDistPtr > NodeMap;

    NodeMap nodeMap_;
    CommitDistPtr commit_;
};

}
}

#endif
