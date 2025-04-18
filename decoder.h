#ifndef DECODER_H
#define DECODER_H

#include "adcm_df.h"
#include "channelmap.h"

class Decoder
{
public:
    Decoder(const std::string &, const std::string &, const ChannelMap &);
    void process();

private:
//    std::string fileName_;
    std::string inputPath_;
    std::string outputPath_;
    std::ifstream ifs_;
    std::ofstream ofs_;
    ChannelMap pre_;
};


#endif // DECODER_H
