/* -*- c++ -*- */

#define MEU_API

%include "gnuradio.i"
%include "digital_swig0.i"			// the common stuff

//load generated python docstrings
%include "meu_swig_doc.i"

%{
#include "meu/qpsk_demod_adapt_cb.h"
#include "meu/qpsk_modulator_adapt_cb.h"
#include "meu/phase_finder_vci.h"
%}


%include "meu/qpsk_demod_adapt_cb.h"
GR_SWIG_BLOCK_MAGIC2(meu, qpsk_demod_adapt_cb);
%include "meu/qpsk_modulator_adapt_cb.h"
GR_SWIG_BLOCK_MAGIC2(meu, qpsk_modulator_adapt_cb);


%include "meu/phase_finder_vci.h"
GR_SWIG_BLOCK_MAGIC2(meu, phase_finder_vci);

