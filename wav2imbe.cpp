#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <stdint.h>

#include <sndfile.h>
#include <imbe_vocoder.h>

#define DEBUG


SNDFILE* openWavInFile(char *wav_in_file)
{
  SF_INFO info;
  SNDFILE *ret = sf_open(wav_in_file, SFM_READ, &info);
  if((info.samplerate != 8000) || (info.channels != 1)) {
      std::cerr << "Wrong wav file format!" << std::endl;
  }
  return ret;
}

void closeWavFile (SNDFILE *wav_f)
{
  sf_close(wav_f);
}

int main(int argc, char **argv)
{
    SNDFILE *infile;
    std::fstream outfile;
    imbe_vocoder* vocoder;

    int16_t imbe_frame[8];
    char imbe_bitfield[128];
    std::string imbe_string;
    
    int16_t audio_in[160];
    int stop = 0;
    int i;

    if(argc != 3) {
        std::cerr << "Usage: " << argv[0] << " infile.wav outfile.txt" << std::endl;
        exit(1);
    }

    // open input file
    if((infile = openWavInFile(argv[1])) == NULL) {
        std::cerr <<  "Could not open input file " <<argv [1] << std::endl;
        exit(1);
    }

    // Open output file
    outfile.open(argv[2], std::ios::out);
    if((outfile.fail())) {
        std::cerr << "Could not open output file " << argv[2] << std::endl;
        exit(1);
    }

    // instantiate vocoder
    vocoder = new imbe_vocoder();

    while (stop == 0) {
        std::memset(imbe_bitfield, 0, 128);
        std::memset(imbe_frame, 0, 8 * sizeof(int16_t));

        // voice frame
        
        if(sf_read_short(infile, audio_in, 160) != 160) {
            stop = 1;
        }
        
        // encode audio
        
        vocoder->imbe_encode(imbe_frame, audio_in);
        
        // write data out
        
        for (i = 0; i < 128; i++) {
            imbe_bitfield[i] = ((imbe_frame[i/16] >> (i%16)) & 1);
            
            if(imbe_bitfield[i] == 0) {
                imbe_string += '0';
            } else if(imbe_bitfield[i] == 1) {
                imbe_string += '1';
            } else {
                std::cerr << "Encountered invalid data!" << std::endl;
                std::exit(1);
            }
        }
        outfile << imbe_string << std::endl;
        imbe_string.clear();
    }

    closeWavFile(infile);
    outfile.close();
    return 0;
}
