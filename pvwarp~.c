#include "MSPd.h"
#include "fftease.h"

#if MSP
void *pvwarp_class;
#endif 

#if PD
static t_class *pvwarp_class;
#endif

#define OBJECT_NAME "pvwarp~"

typedef struct _pvwarp
{
#if MSP
	t_pxobject x_obj;
#endif
#if PD
	t_object x_obj;
	float x_f;
#endif
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
	float P;
	int L;
	int first;
	float Iinv;
	float *lastamp;
	float *lastfreq;
	float *windex;
	float *table;
	float myPInc;
	float ffac;
	//
	int lo_bin;
	int hi_bin;
	float lofreq;//user speficied lowest synthfreq
		float hifreq;// user specified highest synthfreq
			float topfreq;
			float synt;
			float myPI;
			float TWOmyPI;
			// for fast fft
			float mult; 
			float *trigland;
			int *bitshuffle;
			//
			short *connections;
			short mute;
			short bypass;
			int pitch_connected;
			int synt_connected;
			float *warpfunc ;
			short please_update;
			short always_update;
			float cf1;
			float bw1;
			float warpfac1;
			float cf2;
			float bw2;
			float warpfac2;
			int funcoff;
			short verbose;
			short automate;
			int overlap;//overlap factor
				int winfac;// window factor
					int vs;//last measurement of vector size
} t_pvwarp;

void *pvwarp_new(t_symbol *s, int argc, t_atom *argv);
//t_int *offset_perform(t_int *w);
t_int *pvwarp_perform(t_int *w);
void pvwarp_dsp(t_pvwarp *x, t_signal **sp, short *count);
void pvwarp_assist(t_pvwarp *x, void *b, long m, long a, char *s);
void pvwarp_bypass(t_pvwarp *x, t_floatarg state);
void pvwarp_mute(t_pvwarp *x, t_floatarg state);
void pvwarp_verbose(t_pvwarp *x, t_floatarg state);
void pvwarp_overlap(t_pvwarp *x, t_floatarg o);
void pvwarp_automate(t_pvwarp *x, t_floatarg state);
void pvwarp_autofunc(t_pvwarp *x, t_floatarg minval, t_floatarg maxval);
void pvwarp_float(t_pvwarp *x, t_float f);
void pvwarp_dsp_free( t_pvwarp *x );
void pvwarp_fftinfo( t_pvwarp *x );
float myrand( float min, float max );
float closestf(float test, float *arr) ;
int freq_to_bin( float target, float fundamental );
void update_warp_function( t_pvwarp *x ) ;
void pvwarp_init(t_pvwarp *x, short initialized);
void pvwarp_bottomfreq(t_pvwarp *x, t_floatarg f);
void pvwarp_topfreq(t_pvwarp *x, t_floatarg f);
void pvwarp_fftinfo(t_pvwarp *x);
void pvwarp_overlap(t_pvwarp *x, t_floatarg f);
void pvwarp_winfac(t_pvwarp *x, t_floatarg f);;

#if MSP
void main(void)
{
	setup((t_messlist **)&pvwarp_class, (method)pvwarp_new, (method)pvwarp_dsp_free, (short)sizeof(t_pvwarp), 
		  0, A_GIMME, 0);
	addmess((method)pvwarp_dsp, "dsp", A_CANT, 0);
	addmess((method)pvwarp_assist,"assist",A_CANT,0);
	addmess((method)pvwarp_bypass,"bypass",A_DEFFLOAT,0);
	addmess((method)pvwarp_mute,"mute",A_DEFFLOAT,0);
	addmess((method)pvwarp_verbose,"verbose",A_DEFFLOAT,0);
	addmess((method)pvwarp_overlap,"overlap",A_FLOAT,0);
	addmess((method)pvwarp_bottomfreq,"bottomfreq",A_FLOAT,0);
	addmess((method)pvwarp_topfreq,"topfreq",A_FLOAT,0);
	addmess((method)pvwarp_fftinfo,"fftinfo",0);
	addmess((method)pvwarp_autofunc,"autofunc",A_DEFFLOAT, A_DEFFLOAT,0);
	addmess((method)pvwarp_overlap,"overlap",A_DEFFLOAT,0);
	addmess((method)pvwarp_winfac,"winfac",A_DEFFLOAT,0);
	addmess((method)pvwarp_fftinfo,"fftinfo",0);
	addfloat((method)pvwarp_float);
	dsp_initclass();
	post("%s %s",OBJECT_NAME,FFTEASE_ANNOUNCEMENT);
}
#endif

#if PD
/* Pd Initialization */
void pvwarp_tilde_setup(void)
{
	pvwarp_class = class_new(gensym("pvwarp~"), (t_newmethod)pvwarp_new, 
							 (t_method)pvwarp_dsp_free ,sizeof(t_pvwarp), 0,A_GIMME,0);
	CLASS_MAINSIGNALIN(pvwarp_class, t_pvwarp, x_f);
	class_addmethod(pvwarp_class, (t_method)pvwarp_dsp, gensym("dsp"), 0);
	class_addmethod(pvwarp_class, (t_method)pvwarp_mute, gensym("mute"), A_DEFFLOAT,0);
	class_addmethod(pvwarp_class, (t_method)pvwarp_bypass, gensym("bypass"), A_DEFFLOAT,0);
	class_addmethod(pvwarp_class, (t_method)pvwarp_overlap, gensym("overlap"), A_DEFFLOAT,0);
	class_addmethod(pvwarp_class, (t_method)pvwarp_bottomfreq, gensym("bottomfreq"), A_DEFFLOAT,0);
	class_addmethod(pvwarp_class, (t_method)pvwarp_topfreq, gensym("topfreq"), A_DEFFLOAT, 0);
	class_addmethod(pvwarp_class, (t_method)pvwarp_automate, gensym("automate"), A_DEFFLOAT, 0);
	class_addmethod(pvwarp_class, (t_method)pvwarp_autofunc, gensym("autofunc"), A_DEFFLOAT,A_DEFFLOAT,0);
	class_addmethod(pvwarp_class,(t_method)pvwarp_overlap,gensym("overlap"),A_FLOAT,0);
	class_addmethod(pvwarp_class,(t_method)pvwarp_winfac,gensym("winfac"),A_FLOAT,0);
	class_addmethod(pvwarp_class,(t_method)pvwarp_fftinfo,gensym("fftinfo"),0);
	//  addfloat((method)pvwarp_float);
	post("%s %s",OBJECT_NAME,FFTEASE_ANNOUNCEMENT);
}
#endif

void pvwarp_automate(t_pvwarp *x, t_floatarg state)
{
	x->automate = state;
}
void pvwarp_overlap(t_pvwarp *x, t_floatarg f)
{
	int i = (int) f;
	if(!fftease_power_of_two(i)){
		error("%f is not a power of two",f);
		return;
	}
	x->overlap = i;
	pvwarp_init(x,1);
}

void pvwarp_winfac(t_pvwarp *x, t_floatarg f)
{
	int i = (int)f;
	
	if(!fftease_power_of_two(i)){
		error("%f is not a power of two",f);
		return;
	}
	x->winfac = i;
	pvwarp_init(x,2);
}

void pvwarp_fftinfo(t_pvwarp *x)
{
	if( ! x->overlap ){
		post("zero overlap!");
		return;
	}
	post("%s: FFT size %d, hopsize %d, windowsize %d", OBJECT_NAME, x->N, x->N/x->overlap, x->Nw);
}

float myrand ( float min, float max )
{
	return (min + (max-min) * ((float) (rand() % 32768) / (float) 32768.0));
}
void update_warp_function( t_pvwarp *x ) 
{
	int i,j;
	int N2 = x->N2;
	float *warpfunc = x->warpfunc;
	float warpfac1 = x->warpfac1;
	float warpfac2 = x->warpfac2;
	
	float cf1 = x->cf1;
	float cf2 = x->cf2;
	float bw1 = x->bw1;
	float bw2 = x->bw2;
	float c_fundamental = x->c_fundamental;
	float deviation;
	float diff;
	int midbin, lobin, hibin ;
	float hif, lof;
	int bin_extent;
	short verbose = x->verbose;
	
	for( i = 0; i < N2; i++ ){
		warpfunc[i] = 1.0;
	}
	hif = cf1 * (1. + bw1);
	lof = cf1 * (1. - bw1);
	midbin = freq_to_bin( cf1, c_fundamental );
	hibin = freq_to_bin( hif, c_fundamental );
	lobin = freq_to_bin( lof, c_fundamental );
	if( hibin >= N2 - 1 ){
		hibin = N2 - 1;
	}
	if( lobin < 0 ){
		lobin = 0;
	} 
	if( verbose )
		post("bump1: hi %d mid %d lo %d",hibin,midbin,lobin);
	
	warpfunc[midbin] = warpfac1;
	diff = warpfac1 - 1.0 ;
	bin_extent = hibin - midbin ;
	for( i = midbin, j = 0; i < hibin; i++, j++ ){
		deviation = diff * ((float)(bin_extent - j) / (float) bin_extent );
		warpfunc[ i ] += deviation ;
	}
	bin_extent = midbin - lobin ;
	for( i = midbin, j = 0; i > lobin; i--, j++ ){
		deviation = diff * ((float)(bin_extent - j) / (float) bin_extent );
		warpfunc[ i ] += deviation ;
	}
	
	// NOW DO SECOND BUMP
	hif = cf2 * (1. + bw2);
	lof = cf2 * (1. - bw2);
	midbin = freq_to_bin( cf2, c_fundamental );
	hibin = freq_to_bin( hif, c_fundamental );
	lobin = freq_to_bin( lof, c_fundamental );
	if( hibin >= N2 - 1 ){
		hibin = N2 - 1;
	}
	if( lobin < 0 ){
		lobin = 0;
	} 
	if( verbose )
		post("bump2: hi %d mid %d lo %d",hibin,midbin,lobin);
	warpfunc[midbin] = warpfac2;
	diff = warpfac2 - 1.0 ;
	bin_extent = hibin - midbin ;
	for( i = midbin, j = 0; i < hibin; i++, j++ ){
		deviation = diff * ((float)(bin_extent - j) / (float) bin_extent );
		warpfunc[ i ] += deviation ;
	}
	bin_extent = midbin - lobin ;
	for( i = midbin, j = 0; i > lobin; i--, j++ ){
		deviation = diff * ((float)(bin_extent - j) / (float) bin_extent );
		warpfunc[ i ] += deviation ;
	}
	
	x->please_update = 0;
	//	return( x );
	
}
void pvwarp_verbose(t_pvwarp *x, t_floatarg state)
{
	x->verbose = state;	
}

void pvwarp_autofunc(t_pvwarp *x, t_floatarg minval, t_floatarg maxval)
{
	int minpoints, maxpoints, segpoints, i;
	int pointcount = 0;
	float target, lastval;
	float m1, m2;
	int N2 = x->N2;
	float *warpfunc = x->warpfunc;
	/////
	
	minpoints = 0.05 * (float) N2;
	maxpoints = 0.25 * (float) N2;
	if( minval > 1000.0 || minval < .001 ){
		minval = 0.5;
	}
	if( maxval < 0.01 || maxval > 1000.0 ){
		minval = 2.0;
	}
	
	lastval = myrand(minval, maxval);
	// post("automate: min %d max %d",minpoints, maxpoints);
	while( pointcount < N2 ){
		target = myrand(minval, maxval);
		segpoints = minpoints + (rand() % (maxpoints-minpoints));
		if( pointcount + segpoints > N2 ){
			segpoints = N2 - pointcount;
		}
		for( i = 0; i < segpoints; i++ ){
			m2 = (float)i / (float) segpoints ;
			m1 = 1.0 - m2;
			warpfunc[ pointcount + i ] = m1 * lastval + m2 * target;
		}
		lastval = target;
		pointcount += segpoints;
	}
}

void pvwarp_bypass(t_pvwarp *x, t_floatarg state)
{
	x->bypass = state;	
}
void pvwarp_mute(t_pvwarp *x, t_floatarg state)
{
	x->mute = state;	
}

void pvwarp_dsp_free( t_pvwarp *x ){
#if MSP
	dsp_free( (t_pxobject *) x);
#endif
	free(x->c_lastphase_in);
	free(x->c_lastphase_out);
	free(x->trigland);
	free(x->bitshuffle);
	free(x->Wanal);
	free(x->Wsyn);
	free(x->input);
	free(x->Hwin);
	free(x->buffer);
	free(x->channel);
	free(x->output);
	free(x->lastamp);
	free(x->lastfreq);
	free(x->windex);
	free(x->table);
	free(x->warpfunc);
	free(x->connections);
}

void pvwarp_assist (t_pvwarp *x, void *b, long msg, long arg, char *dst)
{
	if (msg==1) {
		switch (arg) {
			case 0:
				sprintf(dst,"(signal) Input ");
				break;
			case 1:
				sprintf(dst,"(signal/float) Center Frequency 1");
				break;
			case 2:
				sprintf(dst,"(signal/float) Bandwidth Factor 1");
				break;
			case 3:
				sprintf(dst,"(signal/float) Warp Factor 1");
				break;
			case 4:
				sprintf(dst,"(signal/float) Center Frequency 2");
				break;
			case 5:
				sprintf(dst,"(signal/float) Bandwidth Factor 2");
				break;
			case 6:
				sprintf(dst,"(signal/float) Warp Factor 2");
				break;
			case 7:
				sprintf(dst,"(signal/float) Function Offset (0.0-1.0) ");
				break;
			case 8:
				sprintf(dst,"(signal/float) Pitch Factor");
				break;
			case 9:
				sprintf(dst,"(signal/float) Synthesis Gate Value");
				break;
				
		}
	} else if (msg==2) {
		sprintf(dst,"(signal) Output");
	}
}

void *pvwarp_new(t_symbol *s, int argc, t_atom *argv)
{
#if MSP
	t_pvwarp *x = (t_pvwarp *)newobject(pvwarp_class);
	dsp_setup((t_pxobject *)x,10);
	outlet_new((t_pxobject *)x, "signal");
#endif
#if PD
	int i;
	t_pvwarp *x = (t_pvwarp *)pd_new(pvwarp_class);
	for(i=0;i<9;i++)
		inlet_new(&x->x_obj, &x->x_obj.ob_pd,gensym("signal"), gensym("signal"));
	outlet_new(&x->x_obj, gensym("signal"));
#endif
	
	x->lofreq = atom_getfloatarg(0,argc,argv);
	x->hifreq = atom_getfloatarg(1,argc,argv);
	x->overlap = atom_getfloatarg(2,argc,argv);
	x->winfac = atom_getfloatarg(3,argc,argv);
	if(!x->overlap)
		x->overlap = 4;
	if(!x->winfac)
		x->winfac = 2;
	x->D = sys_getblksize();  
	x->R = sys_getsr();	
	pvwarp_init(x,0);
	
	return (x);
}

void pvwarp_init(t_pvwarp *x, short initialized)
{
	int i, j;
	float funda, curfreq;
	x->N = x->D * x->overlap;
	x->Nw = x->N * x->winfac;
	limit_fftsize(&x->N,&x->Nw,OBJECT_NAME);
	x->N2 = (x->N)>>1;
	x->Nw2 = (x->Nw)>>1;
	x->c_fundamental =  (float) x->R/(float)( (x->N2)<<1 );
	x->c_factor_in =  (float) x->R/((float)x->D * TWOPI);
	x->c_factor_out = TWOPI * (float)  x->D / (float) x->R;
	x->inCount = -(x->Nw);
	x->mult = 1. / (float) x->N;
	x->Iinv = 1./x->D;
	x->myPInc = x->P*x->L/x->R;// really needed
		x->ffac = x->P * PI/x->N;  
	  	    
		if(!initialized){
			srand(clock());
			x->L = 8192;
			x->P = 1.0 ; //initialization only
			
			x->please_update = 0;
			x->verbose = 0;
			x->bypass = 0;
			x->mute = 0;
			x->topfreq = 3000. ;
			x->always_update = 0;
			x->automate = 0;
			x->warpfac1 = 1.0;
			x->warpfac2 = 1.0;
			x->funcoff = 0;
			x->cf1 = 500.;
			x->cf2 = 3000.;
			x->bw1 = 0.2;
			x->bw2 = 0.2;
			x->synt = .000001;
			
			x->connections = (short *) calloc(16, sizeof(short));	
			x->Wanal = (float *) calloc(MAX_Nw, sizeof(float));	
			x->Wsyn = (float *) calloc(MAX_Nw, sizeof(float));	
			x->input = (float *) calloc(MAX_Nw, sizeof(float));	
			x->Hwin = (float *) calloc(MAX_Nw, sizeof(float));
			x->buffer = (float *) calloc(MAX_N, sizeof(float));
			x->channel = (float *) calloc(MAX_N+2, sizeof(float));
			x->output = (float *) calloc(MAX_Nw, sizeof(float));
			x->bitshuffle = (int *) calloc(MAX_N * 2, sizeof(int));
			x->trigland = (float *) calloc(MAX_N * 2, sizeof(float));
			x->warpfunc = (float *) calloc(MAX_N2, sizeof(float));
			x->lastamp = (float *) calloc(MAX_N+1, sizeof(float));
			x->lastfreq = (float *) calloc(MAX_N+1, sizeof(float));
			x->windex = (float *) calloc(MAX_N+1, sizeof(float));
			x->table = (float *) calloc(x->L, sizeof(float));
			x->c_lastphase_in = (float *) calloc(MAX_N2+1, sizeof(float));
			x->c_lastphase_out = (float *) calloc(MAX_N2+1, sizeof(float));
			
		}
		for (i = 0; i < x->L; i++) {
			x->table[i] = (float) x->N * cos((float)i * TWOPI / (float)x->L);
		}
		
		memset((char *)x->input,0,x->Nw * sizeof(float));
		memset((char *)x->output,0,x->Nw * sizeof(float));
		memset((char *)x->buffer,0,x->N * sizeof(float));
		memset((char *)x->channel,0,(x->N+2) * sizeof(float));
		memset((char *)x->lastfreq,0,(x->N+1) * sizeof(float));
		memset((char *)x->lastamp,0,(x->N+1) * sizeof(float));
		// added
		memset((char *)x->c_lastphase_in,0,(x->N2+1) * sizeof(float));
		memset((char *)x->c_lastphase_out,0,(x->N2+1) * sizeof(float));
		memset((char *)x->windex,0,(x->N+1) * sizeof(float));
		
		if( x->hifreq < x->c_fundamental ) {
			x->hifreq = 1000.0;// arbitrary
		}
		x->hi_bin = 1;  
		curfreq = 0;
		while( curfreq < x->hifreq ) {
			++(x->hi_bin);
			curfreq += x->c_fundamental ;
		}
		if( x->hi_bin >= x->N2 )
			x->hi_bin = x->N2 - 1;
		x->lo_bin = 0;  
		curfreq = 0;
		while( curfreq < x->lofreq ) {
			++(x->lo_bin);
			curfreq += x->c_fundamental ;
		}	
		update_warp_function(x);
		init_rdft(x->N, x->bitshuffle, x->trigland);
		makehanning(x->Hwin, x->Wanal, x->Wsyn, x->Nw, x->N, x->D, 0);
		//  post("FFT size: %d",x->N);
}



void pvwarp_bottomfreq(t_pvwarp *x, t_floatarg f)
{
	int tbin;
	float curfreq;
	float fundamental = x->c_fundamental;
	tbin = 1;  
	curfreq = 0;
	
	if( f < 0 || f > x->R / 2.0 ){
		error("frequency %f out of range", f);
		return;
	}
	
	while( curfreq < f ) {
		++tbin;
		curfreq += fundamental ;
	}
	
	if( tbin < x->hi_bin && tbin >= 0 ){
		x->lo_bin = tbin;
		x->lofreq = f;
	} else {
		error("bin %d out of range", tbin);
	}
	
}

void pvwarp_topfreq(t_pvwarp *x, t_floatarg f)
{
	int tbin;
	float curfreq;
	float fundamental = x->c_fundamental;
	tbin = 1;  
	curfreq = 0;
	
	if( f < 0 || f > x->R / 2.0 ){
		error("frequency %f out of range", f);
		return;
	}
	
	while( curfreq < f ) {
		++tbin;
		curfreq += fundamental ;
	}
	
	if( tbin > x->lo_bin && tbin < x->N2 - 1 ){
		x->hi_bin = tbin;
		x->hifreq = f;
	} else {
		error("bin %d out of range", tbin);
	}
	
}

t_int *pvwarp_perform(t_int *w)
{
	int 	i,j, in,on;
	int    amp,freq,chan,NP,L = 8192;
	
	float    a,ainc,f,finc,address;
	int breaker = 0;
	t_pvwarp *x = (t_pvwarp *) (w[1]);
	
	t_float *inbuf = (t_float *)(w[2]);
	t_float *in1 = (t_float *)(w[3]);
	t_float *in2 = (t_float *)(w[4]);
	t_float *in3 = (t_float *)(w[5]);
	t_float *in4 = (t_float *)(w[6]);
	t_float *in5 = (t_float *)(w[7]);
	t_float *in6 = (t_float *)(w[8]);
	t_float *in7 = (t_float *)(w[9]);
	t_float *in8 = (t_float *)(w[10]);
	t_float *in9 = (t_float *)(w[11]);
	t_float *outbuf = (t_float *)(w[12]);
	t_int n = w[13];
	
	int D = x->D;
	int I = D;
	int R = x->R;
	int Nw = x->Nw;
	int N = x->N ;
	int N2 = x-> N2;
	int Nw2 = x->Nw2;
	float fundamental = x->c_fundamental;
	float factor_in =  x->c_factor_in;
	float factor_out = x->c_factor_out;
	int *bitshuffle = x->bitshuffle;
	float *trigland = x->trigland;
	float mult = x->mult;	
	float warpfac1 = x->warpfac1;
	float warpfac2 = x->warpfac2;
	int funcoff;
	float P;
	float synt;	
	
	float Iinv = 1./x->D;
	float myPInc = x->myPInc;
	
	/* assign pointers */
	
	float *table = x->table;
	float *lastamp = x->lastamp ;
	float *lastfreq = x->lastfreq ;
	float *windex = x->windex;
	float *lastphase_in = x->c_lastphase_in;
	float *lastphase_out = x->c_lastphase_out;
	
	float *Wanal = x->Wanal;
	float *Wsyn = x->Wsyn;
	float *input = x->input;
	float *Hwin = x->Hwin;
	float *buffer = x->buffer;
	float *channel = x->channel;
	float *output = x->output;
	short *connections = x->connections;
	float *warpfunc;
	
	int hi_bin = x->hi_bin;
	int lo_bin = x->lo_bin;
	
	if(!x->automate) {
		if( connections[0] ) {
			x->cf1  = *in1 ; 
		}
		if ( connections[1] ) {
			x->bw1 = *in2 ;
		}
		if ( connections[2] ) {
			x->warpfac1 = *in3 ;
		}
		if( connections[3] ) {
			x->cf2  = *in4 ; 
		}
		if ( connections[4] ) {
			x->bw2 = *in5 ;
		}
		if ( connections[5] ) {
			x->warpfac2 = *in6 ;
		}
	}  
	
	if( connections[6] ) {
		f  = *in7 ; 
		if( f < 0 ) {
			f = 0.0;
		} else if (f > 1.0 ){
			f = 1.0;
		}
		x->funcoff = f * (float) (N2 - 1);
	}
	if ( connections[7] ) {
		x->P = *in8 ;
		x->myPInc = x->P*x->L/x->R;
	}
	if ( connections[8] ) {
		x->synt = *in9 ;
	}
	P= x->P;
	synt = x->synt;
	funcoff = x->funcoff;  	
	
	
	if( (x->please_update || x->always_update)  && ! x->automate){
		update_warp_function(x);
	}
	warpfunc = x->warpfunc;
	
	in = on = x->inCount ;
	if( x->mute) {
		for ( j = 0; j < n; j++ ){
			outbuf[j] = 0.;
		}
		return (w+14);
	}	
	else if( x->bypass ) {
		for ( j = 0; j < n; j++ ){
			outbuf[j] = *inbuf++;
		}
		return (w+14);
	} else {
		
		in = on = x->inCount ;
		
		
		in += D;
		on += I;
		
		for ( j = 0 ; j < (Nw - D) ; j++ ){
			input[j] = input[j+D];
		}
		for ( j = (Nw-D), i = 0 ; j < Nw; j++, i++ ) {
			input[j] = *inbuf++;
		}
		
		fold( input, Wanal, Nw, buffer, N, in );   
		rdft( N, 1, buffer, bitshuffle, trigland );
		convert( buffer, channel, N2, lastphase_in, fundamental, factor_in );
		
		
		// start osc bank 
		
		for ( chan = lo_bin; chan < hi_bin; chan++ ) {
			
			freq = ( amp = ( chan << 1 ) ) + 1;
			if ( channel[amp] < synt ){ 
				breaker = 1;
			}
			if( breaker ) {
				breaker = 0 ;
			} else {
				
				/* HERE IS WHERE WE WARP THE SPECTRUM */
				channel[freq] *= myPInc * warpfunc[(chan + funcoff) % N2];
				finc = ( channel[freq] - ( f = lastfreq[chan] ) )*Iinv;
				ainc = ( channel[amp] - ( a = lastamp[chan] ) )*Iinv;
				address = windex[chan];
				for ( n = 0; n < I; n++ ) {
					output[n] += a*table[ (int) address ];
				    
					address += f;
					while ( address >= L )
						address -= L;
					while ( address < 0 )
						address += L;
					a += ainc;
					f += finc;
				}
				lastfreq[chan] = channel[freq];
				lastamp[chan] = channel[amp];
				windex[chan] = address;
			}
		}
		
		
		for ( j = 0; j < D; j++ ){
			*outbuf++ = output[j] * mult;
		}
		for ( j = 0; j < Nw - D; j++ ){
			output[j] = output[j+D];
		}
		
		for ( j = Nw - D; j < Nw; j++ ){
			output[j] = 0.;
		}	
		
		
		
		/* restore state variables */
		x->inCount = in % Nw;
		return (w+14);
	}
	
}		


int freq_to_bin( float target, float fundamental ){
	float lastf = 0.0;
	float testf = 0.0;
	int thebin = 0;
	while( testf < target ){
		++thebin;
		lastf = testf;
		testf += fundamental;
	}
	
	if(fabs(target - testf) < fabs(target - lastf) ){
		return thebin;
	} else {
		return (thebin - 1);
	}
}
#if MSP
void pvwarp_float(t_pvwarp *x, t_float f) // Look at floats at inlets
{
	int inlet = ((t_pxobject*)x)->z_in;
	int N2 = x->N2;
	if (inlet == 1){
		x->cf1 = f;
		x->please_update = 1;
	} else if (inlet == 2) {
		x->bw1 = f;
		x->please_update = 1;
	} else if (inlet == 3) {
		x->warpfac1 = f;
		x->please_update = 1;
	} else if (inlet == 4) {
		x->cf2 = f;
		x->please_update = 1;
		
	} else if (inlet == 5) {
		x->bw2 = f;
		x->please_update = 1;
		
	} else if (inlet == 6) {
		x->warpfac2 = f;
		x->please_update = 1;
	} else if (inlet == 7) {
		if( f < 0 ) {
			f = 0.0;
		} else if (f > 1.0 ){
			f = 1.0;
		}
		x->funcoff = f * (float) (x->N2 - 1);
	} else if (inlet == 8) {
		x->P = f;
		x->myPInc = x->P*x->L/x->R;
		
	} else if (inlet == 9)
    {
		x->synt = f;
    }
}
#endif

void pvwarp_dsp(t_pvwarp *x, t_signal **sp, short *count)
{
	long i;
	x->always_update = 0;
#if MSP
	for( i = 1; i < 10; i++ ){
		// only the first 6 inlets alter warp function
		if( i < 6 ){
			x->always_update += count[i];
		}
		x->connections[i-1] = count[i];
		// post("connection %d: %d", i-1, count[i]);
	}
#endif
#if PD
	x->always_update = 1;
	for(i=0;i<10;i++){
		x->connections[i] = 1;
	}
#endif
	
	if(x->D != sp[0]->s_n || x->R != sp[0]->s_sr){
		x->D = sp[0]->s_n;
		x->R = sp[0]->s_sr;
		pvwarp_init(x,1);
	}
	dsp_add(pvwarp_perform, 13, x, 
			sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[3]->s_vec, 
			sp[4]->s_vec, sp[5]->s_vec, sp[6]->s_vec, sp[7]->s_vec, 
			sp[8]->s_vec, sp[9]->s_vec, sp[10]->s_vec,
			sp[0]->s_n);
}

