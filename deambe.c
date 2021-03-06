#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <mbelib.h>
#include <sndfile.h>

#include "readambefile.h"
#include "dstar_const.h"

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

void writeSynthesizedVoice (SNDFILE *wav_out_f, float * audio_out_temp_buf)
{
  int n;
  short aout_buf[160];

//  for(n=0; n<160; n++)
//    printf("%d ", ((short*)(state->audio_out_temp_buf))[n]);
//  printf("\n");

  for (n = 0; n < 160; n++)
  {
    if (audio_out_temp_buf[n] > (float) 32767)
      {
        audio_out_temp_buf[n] = (float) 32767;
      }
    else if (audio_out_temp_buf[n] < (float) -32768)
      {
        audio_out_temp_buf[n] = (float) -32768;
      }
      aout_buf[n] = (short) audio_out_temp_buf[n];
  }

  sf_write_short(wav_out_f, aout_buf, 160);
}

int main(int argc, char **argv)
{
    FILE *infile;
    SNDFILE *outfile;
    int stop = 0;

    unsigned int majseq, minseq;
    
    char dvtool_header[15];
    char ambe_fr[4][24];
    float audio_out_temp_buf[160];
    int errs, errs2;
    char err_str[64];
    char ambe_d[49];
    char ambe_d_str[50];

    // mbe state vars
    mbe_parms cur_mp;
    mbe_parms prev_mp;
    mbe_parms prev_mp_enhanced;
    int uvquality = 3;
    
    int ret, i, j, bit_count = 0;
    int bit_array[72];

    const int *w, *x;
    
    if(argc != 3) {
        fprintf(stderr, "Usage: %s infile.ambe outfile.wav\n", argv[0]);
        exit(1);
    }
    
    // Open input file
    if((infile = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "Could not open input file %s\n", argv[1]);
        exit(1);
    }
    
    // open output file
    if((outfile = openWavOutFile(argv[2])) == NULL) {
        fprintf(stderr, "Could not open output file %s\n", argv[2]);
        exit(1);
    }

    // It's a .ambe file. That should
	// start with a line containing '#C Version 1.0"
	
	// return to beginning of file
	fseek(infile,0,SEEK_SET);
	
	// read next of line, up to \n, maximum 14 characters
	ret=fread(dvtool_header,1,14,infile);
	
	if (ret < 14) {
		fprintf(stderr,"Error: Could not read header of input file while probing .ambe format! %s\n",argv[1]);
		fclose(infile);
		exit(1);
	}; // end if
    
    if (strncmp(dvtool_header,"#C Version 1.0",14) != 0) {
        fprintf(stderr,"Error: Not an AMBE file! %s\n",argv[1]);
        fclose(infile);
        exit(1);
    };
    
    // generate bit array for interleave
    for (i=0; i<12; i++)
    {
        for (j=0; j<6; j++)
        {
            bit_array[(j*12)+i] = bit_count^7;
            bit_count++;
        }
    }


    mbe_initMbeParms(&cur_mp, &prev_mp, &prev_mp_enhanced);
    
    while (stop == 0) {
        unsigned char ambebuffer[9];
        memset(ambe_fr, 0, 96);
        // voice frame
        w = dW;
        x = dX;

        ret=readambefile(infile,ambebuffer, &majseq, &minseq);
		
		if (ret == 9) {
			// valid frame

		    // deinterleave frame
			for (i = 0; i < 72; i++) {
        			ambe_fr[*w][*x] = (1 & (ambebuffer[i/8] >> (i%8)));
        			w++;
    		      	x++;
    		    }
            // feed into decoder
            mbe_processAmbe3600x2400Framef (audio_out_temp_buf, &errs, &errs2, err_str, ambe_fr, ambe_d, &cur_mp, &prev_mp, &prev_mp_enhanced, uvquality);
            // convert to binary string array
            for(i=0; i<49; i++) {
                ambe_d_str[i] = ambe_d[i] + '0';
            }
            ambe_d_str[49] = '\0';
#ifdef DEBUG
            // print binary string
            printf("%d\t%d\t%s\t", majseq, minseq, ambe_d_str);
            // print error data
            printf("E1: %d; E2: %d; S: %s\n", errs, errs2, err_str);
#endif
            // output into wav file
            writeSynthesizedVoice(outfile, audio_out_temp_buf);
		} else {
			// no frames left to read in AMBE-file
			stop=1;
		}; // end else - if

    }
    
    fclose(infile);
    closeWavOutFile(outfile);
    return 0;
}