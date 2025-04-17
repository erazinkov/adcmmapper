#ifndef DECODER_H
#define DECODER_H

#include "adcm_df.h"
#include "channelmap.h"

class Decoder
{
public:
    Decoder(const std::string &, const ChannelMap &);
    std::vector<dec_ev_t> & events();
    void process();
    std::vector<long> positionsOfCMAPHeaders();
    std::vector<long> positionsOfNDHeaders();
    void positionsOfCMAPHeadersAndNDHeaders();
private:

    std::string fileName_;
    std::ifstream ifs_;
    ChannelMap pre_;
    std::vector<dec_ev_t> events_;
    dec_cnt_t counters_;
};


#endif // DECODER_H
