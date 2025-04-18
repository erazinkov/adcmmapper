#include "decoder.h"

#include <iostream>
#include <bits/stdc++.h>

Decoder::Decoder(const std::string &inputPath, const std::string &outputPath, const ChannelMap &pre)
    : inputPath_{inputPath}, outputPath_{outputPath}, pre_{pre}
{
}

void Decoder::process()
{
    ifs_.open(inputPath_, std::ios::in | std::ios::binary);
    if (!ifs_.is_open())
    {
        std::cout << "Can't open file " << inputPath_ << std::endl;
        return;
    }
    ofs_.open(outputPath_);
    if (!ofs_.is_open())
    {
        std::cout << "Can't open file" << outputPath_ << std::endl;
        return;
    }

    stor_packet_hdr_t hdr;
    stor_nd_t nd;
    adcm_cmap_t cmap;
    adcm_counters_t counters;

    long long currentPosition{0};
    std::pair<long long, long long> modTimePosCmap{-1, -1};
    auto c{false};

    while (ifs_)
    {
        ifs_ >> hdr;
        currentPosition += sizeof(stor_packet_hdr_t);
        if (hdr.id == STOR_ID_ND && hdr.size > sizeof(stor_packet_hdr_t))
        {
            currentPosition = ifs_.tellg();
            ifs_ >> nd;
            modTimePosCmap.first = nd.time;
            currentPosition -= sizeof(stor_packet_hdr_t);
            continue;
        }
        if (hdr.id == STOR_ID_CMAP && hdr.size > sizeof(stor_packet_hdr_t))
        {
            currentPosition = ifs_.tellg();
            ifs_ >> cmap;
            c = pre_.isCorrect(cmap.map);
            if (c)
            {
                currentPosition -= sizeof(stor_packet_hdr_t);
                modTimePosCmap.second = currentPosition;
            }
            continue;
        }
        if (hdr.id == STOR_ID_EVNT && hdr.size > sizeof(stor_packet_hdr_t))
        {
            hdr.size -= sizeof(stor_packet_hdr_t);
            ifs_.ignore(hdr.size);
            continue;
        }
        if (hdr.id == STOR_ID_CNTR && hdr.size > sizeof(stor_packet_hdr_t))
        {
            if (modTimePosCmap.first != -1 && modTimePosCmap.second != -1)
            {
                ofs_ << modTimePosCmap.first << " " << modTimePosCmap.second << "\n";
                std::cout << modTimePosCmap.first << " " << modTimePosCmap.second << std::endl;
                modTimePosCmap.first = -1;
                modTimePosCmap.second = -1;
            }
            hdr.size -= sizeof(stor_packet_hdr_t);
            ifs_.ignore(hdr.size);
            continue;
        }
        ifs_.seekg(1 - static_cast<long long>(sizeof(stor_packet_hdr_t)), std::ios_base::cur);
    }
    ifs_.close();
    ofs_.close();
}

