#include <iostream>
#include <fstream>
#include <string>
#include <stdint.h>

#include <sndfile.h>
#include <imbe_vocoder.h>

#define DEBUG


SNDFILE* openWavOutFile(char *wav_out_file)
{
  SF_INFO info;
  info.samplerate = 8000;
  info.channels = 1;
  info.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16 | SF_ENDIAN_LITTLE;
  return sf_open(wav_out_file, SFM_WRITE, &info);
}

void closeWavOutFile (SNDFILE *wav_out_f)
{
  sf_close(wav_out_f);
}

int main(int argc, char **argv)
{
    std::fstream infile;
    SNDFILE *outfile;
    imbe_vocoder* vocoder;

    int16_t imbe_frame[8];
    char imbe_bitfield[128];
    std::string imbe_string;
    
    int16_t audio_out[160];

    int i;

    if(argc != 3) {
        std::cerr << "Usage: " << argv[0] << " infile.txt outfile.wav" << std::endl;
        exit(1);
    }

    // Open input file
    infile.open(argv[1], std::ios::in);
    if((infile.fail())) {
        std::cerr << "Could not open input file " << argv[1] << std::endl;
        exit(1);
    }

    // open output file
    if((outfile = openWavOutFile(argv[2])) == NULL) {
        std::cerr <<  "Could not open output file " <<argv [2] << std::endl;
        exit(1);
    }
    
    // instantiate vocoder
    vocoder = new imbe_vocoder();

    while (1) {
        std::memset(imbe_bitfield, 0, 128);
        std::memset(imbe_frame, 0, 8 * sizeof(int16_t));
        
        // voice frame
        
        std::getline(infile, imbe_string);
        if (infile.eof() || (imbe_string.length() != 128)) {
            std::cerr << "End of input file!" << std::endl;
            break;
        }
        
        for (i = 0; i < 128; i++) {
            if(imbe_string[i] == '0') {
                imbe_bitfield[i] = 0;
            } else if(imbe_string[i] == '1') {
                imbe_bitfield[i] = 1;
            } else {
                std::cerr << "Encountered invalid character!" << std::endl;
                std::exit(1);
            }
            imbe_frame[i/16] |= imbe_bitfield[i] << (i%16);
        }
        
        vocoder->imbe_decode(imbe_frame, audio_out);
        
        sf_write_short(outfile, audio_out, 160);
    }

    infile.close();
    closeWavOutFile(outfile);
    return 0;
}