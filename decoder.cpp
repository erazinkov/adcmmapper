#include "decoder.h"

#include <iostream>
#include <bits/stdc++.h>

Decoder::Decoder(const std::string &fileName, const ChannelMap &pre)
    : fileName_{fileName} , pre_{pre}
{
}

std::vector<dec_ev_t> &Decoder::events()
{
    return events_;
}

void Decoder::process()
{
    ifs_.open(fileName_, std::ios::in | std::ios::binary);
    if (!ifs_.is_open())
    {
        std::clog << "Can't open file" << std::endl;
        return;
    }

    counters_.rawhits.resize(pre_.numberOfChannelsAlpha());

    events_.clear();

    stor_packet_hdr_t hdr;
    stor_ev_hdr_t ev;
    adcm_cmap_t cmap;
    adcm_counters_t counters;

    stor_nd_t nd;

    std::vector<long long int> ndV;
    std::vector<double> tsSumV;
    std::vector<double> countersTimeV;

    auto c{false};

    auto isIntegerOverflow = [](long long int currentTs, long long int prevTs, long long int limit = 3'000'000'000){
        return std::abs(currentTs - prevTs) > limit;
    };

    std::vector<dec_ev_t> spillEvents;

    while (ifs_)
    {
        ifs_ >> hdr;

        if (hdr.id == STOR_ID_ND && hdr.size > sizeof(stor_packet_hdr_t))
        {
            ifs_ >> nd;
            continue;
        }
        if (hdr.id == STOR_ID_CMAP && hdr.size > sizeof(stor_packet_hdr_t))
        {
            ifs_ >> cmap;
            c = pre_.isCorrect(cmap.map);

            spillEvents.clear();

            continue;
        }
        if (hdr.id == STOR_ID_EVNT && hdr.size > sizeof(stor_packet_hdr_t))
        {
            if (!c)
            {
                hdr.size -= sizeof(stor_packet_hdr_t);
                ifs_.ignore(hdr.size);
                continue;
            }
            ifs_ >> ev;
            if (ev.np != 2)
            {
                hdr.size -= sizeof(stor_packet_hdr_t);
                hdr.size -= sizeof(stor_ev_hdr_t);
                ifs_.ignore(hdr.size);
                continue;
            }
            stor_puls_t *g = new stor_puls_t();
            stor_puls_t *a = new stor_puls_t();
            ifs_ >> *g >> *a;
            if (g->ch < pre_.map().size() && a->ch < pre_.map().size())
            {
                dec_ev_t event;
                auto numberGamma{pre_.numberByChannel(g->ch)};
                auto numberAlpha{pre_.numberByChannel(a->ch)};
                event.g.index = numberGamma;
                event.g.amp = g->a;
                event.a.index = numberAlpha;
                event.a.amp = g->a;
                event.tdc = g->t - a->t;
                long long int currentTs{static_cast<long long int>(ev.ts)};
                event.ts = currentTs;
                spillEvents.push_back(event);
//                events_.push_back(event);
            }
            delete g;
            delete a;
            continue;
        }
        if (hdr.id == STOR_ID_CNTR && hdr.size > sizeof(stor_packet_hdr_t))
        {
            if (!c)
            {
                hdr.size -= sizeof(stor_packet_hdr_t);
                ifs_.ignore(hdr.size);
            }
            ifs_ >> counters;
            for (size_t i{0}; i < pre_.map().size(); ++i)
            {
                auto number{pre_.numberByChannel(i)};
                auto type{pre_.typeByChannel(i)};
                switch (type)
                {
                    case ALPHA:
                    {
                        counters_.rawhits[number] += counters.rawhits[i];
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }
            }
            counters_.time += counters.time;
            if (spillEvents.size())
            {

                for (size_t i{1}; i < spillEvents.size(); ++i)
                {
                    while (isIntegerOverflow(spillEvents[i].ts, spillEvents[0].ts) && spillEvents.size()) {
                        spillEvents[i].ts += UINT32_MAX;
                    }
                }
                auto tsOffset{spillEvents[0].ts};
                for (size_t i{0}; i < spillEvents.size(); ++i)
                {
                    spillEvents[i].ts -= tsOffset;
                }
                for (size_t i{0}; i < spillEvents.size(); ++i)
                {
                    auto dateInNanoSec{nd.time * 1'000'000 + spillEvents[i].ts * 10 - spillEvents[spillEvents.size() - 1].ts * 10};
                    spillEvents[i].ts = dateInNanoSec;
                }

                events_.insert(events_.cend(), spillEvents.cbegin(), spillEvents.cend());
            }
            continue;
        }
        ifs_.seekg(1 - static_cast<long long>(sizeof(stor_packet_hdr_t)), std::ios_base::cur);
    }
    ifs_.close();
}

std::vector<long> Decoder::positionsOfCMAPHeaders()
{
    std::vector<long> pos{};
    ifs_.open(fileName_, std::ios::in | std::ios::binary);
    if (!ifs_.is_open())
    {
        std::clog << "Can't open file" << std::endl;
        return pos;
    }

    stor_packet_hdr_t hdr;
    adcm_cmap_t cmap;
    adcm_counters_t counters;

    auto c{false};
    long currentPosition{0};
    while (ifs_)
    {
        ifs_ >> hdr;
        currentPosition += sizeof(stor_packet_hdr_t);
        if (hdr.id == STOR_ID_CMAP && hdr.size > sizeof(stor_packet_hdr_t))
        {
            currentPosition = ifs_.tellg();
            ifs_ >> cmap;
            c = pre_.isCorrect(cmap.map);
            if (c)
            {
                currentPosition -= sizeof(stor_packet_hdr_t);
                pos.push_back(currentPosition);
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

            hdr.size -= sizeof(stor_packet_hdr_t);
            ifs_.ignore(hdr.size);
            continue;
        }
        ifs_.seekg(1 - static_cast<long long>(sizeof(stor_packet_hdr_t)), std::ios_base::cur);
    }
    ifs_.close();

    return pos;
}

std::vector<long> Decoder::positionsOfNDHeaders()
{
    std::vector<long> pos{};
    ifs_.open(fileName_, std::ios::in | std::ios::binary);
    if (!ifs_.is_open())
    {
        std::clog << "Can't open file" << std::endl;
        return pos;
    }

    stor_packet_hdr_t hdr;
    stor_nd_t nd;
    adcm_counters_t counters;

    long currentPosition{0};
    while (ifs_)
    {
        ifs_ >> hdr;
        currentPosition += sizeof(stor_packet_hdr_t);
        if (hdr.id == STOR_ID_ND && hdr.size > sizeof(stor_packet_hdr_t))
        {
            currentPosition = ifs_.tellg();
            ifs_ >> nd;
            currentPosition -= sizeof(stor_packet_hdr_t);
            pos.push_back(currentPosition);
            continue;
        }
        if (hdr.id == STOR_ID_CMAP && hdr.size > sizeof(stor_packet_hdr_t))
        {
            hdr.size -= sizeof(stor_packet_hdr_t);
            ifs_.ignore(hdr.size);
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

            hdr.size -= sizeof(stor_packet_hdr_t);
            ifs_.ignore(hdr.size);
            continue;
        }
        ifs_.seekg(1 - static_cast<long long>(sizeof(stor_packet_hdr_t)), std::ios_base::cur);
    }
    ifs_.close();

    return pos;
}

void Decoder::positionsOfCMAPHeadersAndNDHeaders()
{

    ifs_.open(fileName_, std::ios::in | std::ios::binary);
    if (!ifs_.is_open())
    {
        std::clog << "Can't open file" << std::endl;
        return;
    }

    stor_packet_hdr_t hdr;
    stor_nd_t nd;
    adcm_cmap_t cmap;
    adcm_counters_t counters;

    long long currentPosition{0};
    std::vector<long long> data;
    std::vector<std::vector<long long>> dataV;
    auto c{false};

    while (ifs_)
    {
        ifs_ >> hdr;
        currentPosition += sizeof(stor_packet_hdr_t);
        if (hdr.id == STOR_ID_ND && hdr.size > sizeof(stor_packet_hdr_t))
        {
            currentPosition = ifs_.tellg();
            ifs_ >> nd;
            currentPosition -= sizeof(stor_packet_hdr_t);
            data.push_back(nd.time);
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
                if (!data.size())
                {
                    data.push_back(-1);
                }
                data.push_back(currentPosition);
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
            if (data.size() == 2)
            {
                dataV.push_back(data);
                std::cout << data.at(0) << " " << data.at(1) << std::endl;
            }
            data.clear();
            hdr.size -= sizeof(stor_packet_hdr_t);
            ifs_.ignore(hdr.size);
            continue;
        }
        ifs_.seekg(1 - static_cast<long long>(sizeof(stor_packet_hdr_t)), std::ios_base::cur);
    }
    ifs_.close();
    std::cout << dataV.size() << std::endl;
}

