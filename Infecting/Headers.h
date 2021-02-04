#pragma once
#include "Packets.h"

#include <iostream>
#include <winsock2.h>
#include <stdint.h>

struct DHCP_header
{
    uint8_t op;         // 1 is request, 2 is reply
    uint8_t htype;      // type of hardware: 1 is Ether, 2 is wlan
    uint8_t hlen;       // length of the address
    uint8_t hops;       // indicates nomber of hops
    uint32_t xid;       // identification
    uint16_t secs;      // time since discover by the client
    uint16_t flags;     // 1 for a discover packet
    uint32_t ciaddr;    // client's ip if he has one
    uint32_t yiaddr;    // ip set by the server
    uint32_t siaddr;    // server's ip
    uint32_t giaddr;    // gateway's ip
    char chaddr[16];    // client's mac address
    char sname[64];     // server's name or DNS address
    char file[128];     // type of boot
    char magic[4];      // DHCP options
    char opt[55];       // DHCP options
};

struct IP_header
{
    uint8_t ip_header_len:4;        // 4-bit header length (in 32-bit words)
    uint8_t ip_version:4;           // 4-bit IPv4 version
    uint8_t ip_tos;                 // IP type of service
    uint16_t ip_total_length;       // Total length
    uint16_t ip_id;                 // Unique identifier

    uint8_t ip_frag_offset : 5;     // Fragment offset field
    uint8_t ip_more_fragment : 1;
    uint8_t ip_dont_fragment : 1;
    uint8_t ip_reserved_zero : 1;
    uint8_t ip_frag_offset1;        //fragment offset

    uint8_t ip_ttl;                 // Time to live
    uint8_t ip_protocol;            // Protocol(TCP,UDP etc)
    uint16_t ip_checksum;           // IP checksum
    uint32_t ip_srcaddr;            // Source address
    uint32_t ip_destaddr;           // Source address
};

struct UDP_header
{
    uint16_t src_port;      /* Source port */
    uint16_t dst_port;      /* Destination port */
    uint16_t len;           /* Length */
    uint16_t chksum;        /* Checksum */
};

struct DNS_header
{
    uint16_t id;
    uint16_t flags;
    uint16_t questions;
    uint16_t answers;
    uint16_t authority;
    uint16_t additional;
};

struct DNS_question
{
    uint16_t qtype;
    uint16_t qclass;
};

struct DNS_answer
{
    uint16_t type;
    uint16_t _class;
    uint32_t ttl;
    uint16_t data_len;
};

struct DNS_record
{
    uint8_t* name;
    DNS_answer* resource;
    uint8_t* rdata;
};

struct DNS_query
{
    uint8_t* name;
    DNS_question* question;
};

struct Ethernet_header {
    uint8_t dest_addr[6];   /* Destination hardware address */
    uint8_t src_addr[6];    /* Source hardware address */
    uint16_t frame_type;    /* Ethernet frame type */
};

void createDHCPdiscoverHeader(char* packet, size_t pHeader, uint8_t htype);
void createIPv4Header(char* packet, size_t pHeader, uint16_t total_size, uint8_t* src, uint8_t* dst, uint16_t id);
void createUDPHeader(char* packet, size_t pHeader, uint16_t src_port, uint16_t dst_port, uint16_t p_size);
void createEthernetHeader(char* packet, size_t pHeader, uint8_t* dst_mac, uint8_t* src_mac);

void getDHCPheader(char* packet, size_t pHeader, DHCP_header& rDHCP);
void getIPheader(char* packet, size_t pHeader, IP_header& rIP);
void getUDPheader(char* packet, size_t pHeader, UDP_header& rUDP);
void getDNSheader(char* packet, size_t pHeader, DNS_header& rDNS);
void getEthernetheader(char* packet, size_t pHeader, Ethernet_header& rEther);

void createDHCPOfferHeader(char* packet, size_t pHeader, DHCP_header& pDiscover, AddressInfo& info, uint32_t randomIP);
void createDHCPackHeader(char* packet, size_t pHeader, DHCP_header& pRequest, AddressInfo& info);

void createDNSResponseHeader(char* packet, size_t pHeader, DNS_header& rDNS ,uint32_t dstIP);