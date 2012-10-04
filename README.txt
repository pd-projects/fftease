FFTease 2.5 by Eric Lyon and Christopher Penrose

This is the third release of FFTease, a set of live spectral sound processors for Max/MSP and Pd. This Pd distribution is compiled for Linux, OS X, and Windows. Move the appropriate set of binaries from "bin" to the Pd "extra" folder. Move the contents of "help" to the Pd "doc/5.reference" directory. You are now ready to use FFTease.

Caveat: these objects are CPU intensive. A few of these objects in a patch could push your computer to its limits. Be very careful with playback volume as some of the objects produce dramatically different (lower or higher) overall levels.  

The FFT size, which must be a power of 2, is calculated relative to the Pd block size. It is recommended to use FFTease externals in a sub-patch that contains a block~ object, granting you control of the local block size, and thus FFT size. A larger block size results in a larger FFT size. Don't expect a great savings in CPU usage with smaller block sizes. But do expect increased CPU load as you crank the block size up. A block size of 256, with a default overlap of 4 gives an FFT size of 1024 that  works acceptably well in many cases. With some of these objects, an even smaller block size/FFT size can actually work better. See the abstraction "fftease-system.pd" for more information on how to control these parameters.

Acknowledgements: This work draws heavily on the phase vocoder code presented by F. Richard Moore in his classic "Elements of Computer Music." Additional inspiration was derived from work on cross synthesis and noise reduction by Mark Dolson at CARL in the mid-1980s. We also wish to thank Miller Puckette and David Zicarelli for designing and implementing the framework under which FFTease is presented.

The authors would like to warmly acknowledge the support of the following institutions: Brown University, Dartmouth College, IAMAS, Keio University, the University of Manchester, and Queen's University Belfast.

FFTease is copyright 2000-2009 Eric Lyon and Christopher Penrose. FFTease is released under the MIT license. 

Eric Lyon
e.lyon@qub.ac.uk

Christopher Penrose
penrose@silvertone.princeton.edu
