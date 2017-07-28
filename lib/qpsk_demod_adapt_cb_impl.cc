/* -*- c++ -*- */
/* 
 * Copyright 2017 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "qpsk_demod_adapt_cb_impl.h"

namespace gr {
  namespace meu {

    qpsk_demod_adapt_cb::sptr
    qpsk_demod_adapt_cb::make()
    {
      return gnuradio::get_initial_sptr
        (new qpsk_demod_adapt_cb_impl());
    }

    /*
     * The private constructor
     */
    qpsk_demod_adapt_cb_impl::qpsk_demod_adapt_cb_impl()

      : gr::block("qpsk_demod_adapt_cb",
              gr::io_signature::make2(2, 2, sizeof(gr_complex), sizeof(int32_t) ),
              gr::io_signature::make(1, 1, sizeof(char)))
    {set_tag_propagation_policy(TPP_ALL_TO_ALL);}

    /*
     * Our virtual destructor.
     */
    qpsk_demod_adapt_cb_impl::~qpsk_demod_adapt_cb_impl()
    {
    }

    void
    qpsk_demod_adapt_cb_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      // With this forecast the scheduler knows that it will have the number of output items = to the number of inputs on port 0 and 1 input from port 1 
    	unsigned ninputs = ninput_items_required.size ();
     	
      		 ninput_items_required[0] = noutput_items;
           ninput_items_required[1] = noutput_items / noutput_items;
    }
	unsigned char
    qpsk_demod_adapt_cb_impl::get_minimum_distances(const gr_complex &sample, const int32_t &adapt_ref)
    { 	// bit0 = least significant  || bit1 = most significant 
    	// The constellation representation for each reference is showed to simplify the understanding of the code
    	if (adapt_ref == 1)
    	{
			if (sample.imag() >= 0 and sample.real() >= 0) {
				return 0x00;
			}
			else if (sample.imag() >= 0 and sample.real() < 0) {
				return 0x01;
			}
			else if (sample.imag() < 0 and sample.real() < 0) {
	  			return 0x02;
			}
			else if (sample.imag() < 0 and sample.real() >= 0) {
	  			return 0x03;
			}
    	}

    	else if (adapt_ref == 2){

    		if (sample.imag() >= 0 and sample.real() >= 0) {
				return 0x02;
			}
			else if (sample.imag() >= 0 and sample.real() < 0) {
				return 0x00;
			}
			else if (sample.imag() < 0 and sample.real() < 0) {
	  			return 0x03;
			}
			else if (sample.imag() < 0 and sample.real() >= 0) {
	  			return 0x01;
			}

    	}

		else if (adapt_ref == 3){
    		if (sample.imag() >= 0 and sample.real() >= 0) {
				return 0x01;
			}
			else if (sample.imag() >= 0 and sample.real() < 0) {
				return 0x03;
			}
			else if (sample.imag() < 0 and sample.real() < 0) {
	  			return 0x00;
			}
			else if (sample.imag() < 0 and sample.real() >= 0) {
	  			return 0x02;
			}

		}
    	else if (adapt_ref == 4){
    		if (sample.imag() >= 0 and sample.real() >= 0) {
				return 0x03;
			}
			else if (sample.imag() >= 0 and sample.real() < 0) {
				return 0x02;
			}
			else if (sample.imag() < 0 and sample.real() < 0) {
	  			return 0x01;
			}
			else if (sample.imag() < 0 and sample.real() >= 0) {
	  			return 0x00;
			}	  	
    }}



    int
    qpsk_demod_adapt_cb_impl::general_work (int32_t noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const gr_complex *in0 = (const gr_complex *) input_items[0];
      const int *in1 = (const int *) input_items[1];
      unsigned char *out = (unsigned char *) output_items[0];

      // Perform ML decoding over the input iq data to generate alphabets
      for (int i = 0; i < noutput_items; i++)
      {
      	// ML decoder, determine the minimum distance from all constellation points
      	out[i] = get_minimum_distances(in0[i], in1[i]);
      }
      // Tell runtime system how many input items we consumed on
      // each input stream.
      // O consume vai consumir n outputs no porto 0 e apenas 1 no porto 1 (referência)
      consume(0 ,noutput_items);
      consume(1 ,1);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

    

  } /* namespace meu */
} /* namespace gr */

