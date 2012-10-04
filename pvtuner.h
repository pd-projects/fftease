#define MAXTONES (1024)
#define BASE_FREQ (27.5)	/* low A  */
#define DIATONIC 0
#define EASTERN 1
#define MINOR 2
#define EQ12 3
#define PENTATONIC 4
#define MAJOR_ADDED_SIXTH 5
#define MINOR_ADDED_SIXTH 6
#define MAJOR_SEVENTH_CHORD 7
#define MINOR_SEVENTH_CHORD 8
#define DOMINANT_SEVENTH_CHORD 9
#define EQ8 10
#define PENTACLUST 11
#define QUARTERCLUST 12
#define EQ5 13
#define SLENDRO 14
#define PELOG 15
#define IMPORTED_SCALE 16

#define DEFAULT_VECTOR_SIZE 512
static t_class *pvtuner_class;

typedef struct _pvtuner
{
  t_object x_obj;
  t_float x_f;
  int R;
  int	N;
  int	N2;
  int	Nw;
  int	Nw2; 
  int	D; 
  int	i;
  int	inCount;
  float *Wanal;	
  float *Wsyn;	
  float *input;	
  float *Hwin;
  float *buffer;
  float *channel;
  float *output;
  // for convert
  float *c_lastphase_in;
  float *c_lastphase_out;
  float c_fundamental;
  float c_factor_in;
  float c_factor_out;

  // for oscbank
  int NP;
  float P;
  int L;
  int first;
  float Iinv;
  float *lastamp;
  float *lastfreq;
  float *bindex;
  float *table;
  float myPInc;
  float ffac;
  //
  int lo_bin;
  int hi_bin;
  float topfreq;
  float synt;
  float myPI;
  float TWOmyPI;
  // for fast fft
  float mult; 
  float *trigland;
  int *bitshuffle;
  //
  float *prebuffer;
  float *postbuffer;
  //
  int bypass_state;
  int pitch_connected;
  int synt_connected;
  // TUNING
  float *pitchgrid ;
  float pbase ;
  int scale_steps;
  short current_scale;
  short mute;
  //
  float TWOPIoL;
  float user_lofreq;
  float user_hifreq;
  int vector_size;
  float funda;
  float curfreq;
  int overlap;
  int window_factor;
  float tabscale;
  int quality;
  int scale_len;
} t_pvtuner;


float closestf(float test, float *arr) ;
void pvtuner_diatonic( t_pvtuner *x );
void pvtuner_eastern( t_pvtuner *x );
void pvtuner_minor( t_pvtuner *x );
void pvtuner_eq12( t_pvtuner *x );
void pvtuner_pentatonic( t_pvtuner *x );
void pvtuner_major_added_sixth( t_pvtuner *x );
void pvtuner_minor_added_sixth( t_pvtuner *x );
void pvtuner_major_seventh_chord( t_pvtuner *x );
void pvtuner_minor_seventh_chord( t_pvtuner *x );
void pvtuner_dominant_seventh_chord( t_pvtuner *x );
void pvtuner_eq8( t_pvtuner *x );
void pvtuner_pentaclust( t_pvtuner *x );
void pvtuner_quarterclust( t_pvtuner *x );
void pvtuner_eq5( t_pvtuner *x );
void pvtuner_slendro( t_pvtuner *x );
void pvtuner_pelog( t_pvtuner *x );
void pvtuner_update_imported( t_pvtuner *x );


