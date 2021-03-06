/* 
 * File:   KnxPacket.cpp
 * Author: Joshua Johannson
 *
 * 
 * ---------------------------------------
 * KNXPACKET     CLASS
 * basic knxIp packet
 * --------------------------------------
 */


#include "KnxPacket.h"
#include "TunnelingResponse.h"
#include "TunnelingRequest.h"
#include "DisconnectRespose.h"
#include "DisconnectRequest.h"
home::Log KnxPacket::log;


// -- CREATE OBJECT ---------------------

KnxPacket::KnxPacket() {log.setLogName("KNXP", "KnxPacket");}
KnxPacket::KnxPacket(KnxConnection* connection) : KnxPacket::KnxPacket()
{
    this->connection = connection;
    
    // default parameter 
    // when sending the packet
    headerLenght             = 0x06; // 6 bytes
    headerVersion            = 0x10; // KNXnet version (1.0)
}


// -- SET VAR FROM BYTES ----------------
void KnxPacket::fromBytes(char bytes[]) 
{    
    headerLenght             = bytes[0];
    headerVersion            = bytes[1];
    headerTypeHi             = bytes[2];
    headerTypeLo             = bytes[3];
    headerTotalLengthHi     = bytes[4];
    headerTotalLengthLo     = bytes[5];
}


// -- GET BYTES FROM VAR ----------------
void KnxPacket::toBytes(char bytes[]) 
{
    bytes[0] = headerLenght;
    bytes[1] = headerVersion;
    bytes[2] = headerTypeHi;
    bytes[3] = headerTypeLo;
    bytes[4] = headerTotalLengthHi;
    bytes[5] = headerTotalLengthLo;
}


// -- GET PACKET BY TYPE ----------------
KnxPacket* KnxPacket::getPacket(char bytes[], KnxConnection* connection)
{
    short type = (short)( bytes[3] | (bytes[2] << 8) );
    
    // which type
    switch (type)
    {
        case KNX_PACKET:
            log.trace("make packet type packet '"+ to_string(type) +"'");
            return new KnxPacket(connection);
            
        case KNX_PACKET_TUNNELING_REQUEST:
            log.trace(" make packet type tunneling request '"+ to_string(type) +"'");
            return new TunnelingRequest(connection, bytes);
            
        case KNX_PACKET_TUNNELING_RESPONSE:
            log.trace(" make packet type tunneling response '"+ to_string(type) +"'");
            return new TunnelingResponse(connection, bytes);

        case KNX_PACKET_DISCONNECT_REQUEST:
            log.trace("make packet type disconnect request '"+ to_string(type) +"'");
            return new DisconnectRequest(connection, bytes);

        case KNX_PACKET_DISCONNECT_RESPONSE:
            log.trace("make packet type disconnect response '"+ to_string(type) +"'");
            return new DisconnectRespose(connection, bytes);
            
        default:
            log.err("unknown packet type '"+ to_string(type) +"'");
    }
}


// -- SET GET ---------------------------   
void KnxPacket::setType(short type) 
{
    // set type
    headerTypeLo = type;
    headerTypeHi = (type >> 8);
}


short KnxPacket::getType() 
{
    // get type
    return  (short)(headerTypeLo | (headerTypeHi << 8));
}

void KnxPacket::setTotalLength(short length) 
{
    // set length
    headerTotalLengthLo = length;
    headerTotalLengthHi = length >> 8;
}


short KnxPacket::getTotalLength() 
{
    // get length
    return (short)(headerTotalLengthLo | (headerTotalLengthHi << 8));
}



// -- SEND ------------------------------
void KnxPacket::send() 
{
    char bytes[getTotalLength()];
    toBytes(bytes);
    connection->sendBytes(bytes, getTotalLength());
}



// -- DESTROY ---------------------------
KnxPacket::~KnxPacket() 
{
    
}

