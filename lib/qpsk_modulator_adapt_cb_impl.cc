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

#include <cmath>
#include <gnuradio/io_signature.h>
#include "qpsk_modulator_adapt_cb_impl.h"

namespace gr {
  namespace meu {

    qpsk_modulator_adapt_cb::sptr
    qpsk_modulator_adapt_cb::make()
    {
      return gnuradio::get_initial_sptr
        (new qpsk_modulator_adapt_cb_impl());
    }

    /*
     * The private constructor
     */
    qpsk_modulator_adapt_cb_impl::qpsk_modulator_adapt_cb_impl()
      : gr::block("qpsk_modulator_adapt_cb",
              gr::io_signature::make2(2, 2, sizeof(char), sizeof(int32_t) ),
              gr::io_signature::make(1, 1, sizeof(gr_complex)))
    {}

    /*
     * Our virtual destructor.
     */
    qpsk_modulator_adapt_cb_impl::~qpsk_modulator_adapt_cb_impl()
    {
    }

    void
    qpsk_modulator_adapt_cb_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
    	unsigned ninputs = ninput_items_required.size ();
     	for(unsigned i = 0; i < ninputs; i++)
      		 ninput_items_required[i] = noutput_items;
    }

    gr_complex
    qpsk_modulator_adapt_cb_impl::map_into_constellation(const char &sample, const int32_t &adapt_ref)
    {
    	gr_complex tmp;
    	if (adapt_ref == 0)
    	{
    		if (sample == 0)
    		{
    			tmp.imag() = sqrt(2)/2;
    			tmp.real() = sqrt(2)/2;
    			return tmp;
    		}
    		else if (sample == 1)
    		{
    			tmp.imag() = sqrt(2)/2;
    			tmp.real() = -sqrt(2)/2;
    			return tmp;
    		}
    		else if (sample == 2)
    		{
    			tmp.imag() = -sqrt(2)/2;
    			tmp.real() = -sqrt(2)/2;
    			return tmp;
    		}
    		else if (sample == 3)
    		{
    			tmp.imag() = -sqrt(2)/2;
    			tmp.real() = sqrt(2)/2;
    			return tmp;
    		}
    	}
    	  	else if (adapt_ref == 1)
    	{
    		if (sample == 3)
    		{
    			tmp.imag() = sqrt(2)/2;
    			tmp.real() = sqrt(2)/2;
    			return tmp;
    		}
    		else if (sample == 2)
    		{
    			tmp.imag() = sqrt(2)/2;
    			tmp.real() = -sqrt(2)/2;
    			return tmp;
    		}
    		else if (sample == 0)
    		{
    			tmp.imag() = -sqrt(2)/2;
    			tmp.real() = -sqrt(2)/2;
    			return tmp;
    		}
    		else if (sample == 1)
    		{
    			tmp.imag() = -sqrt(2)/2;
    			tmp.real() = sqrt(2)/2;
    			return tmp;
    		}
    	}
    	  	else if (adapt_ref == 2)
    	{
    		if (sample == 1)
    		{
    			tmp.imag() = sqrt(2)/2;
    			tmp.real() = sqrt(2)/2;
    			return tmp;
    		}
    		else if (sample == 0)
    		{
    			tmp.imag() = sqrt(2)/2;
    			tmp.real() = -sqrt(2)/2;
    			return tmp;
    		}
    		else if (sample == 3)
    		{
    			tmp.imag() = -sqrt(2)/2;
    			tmp.real() = -sqrt(2)/2;
    			return tmp;
    		}
    		else if (sample == 2)
    		{
    			tmp.imag() = -sqrt(2)/2;
    			tmp.real() = sqrt(2)/2;
    			return tmp;
    		}
    	}
    	  else if (adapt_ref == 3)
    	{
    		if (sample == 2)
    		{
    			tmp.imag() = sqrt(2)/2;
    			tmp.real() = sqrt(2)/2;
    			return tmp;
    		}
    		else if (sample == 3)
    		{
    			tmp.imag() = sqrt(2)/2;
    			tmp.real() = -sqrt(2)/2;
    			return tmp;
    		}
    		else if (sample == 1)
    		{
    			tmp.imag() = -sqrt(2)/2;
    			tmp.real() = -sqrt(2)/2;
    			return tmp;
    		}
    		else if (sample == 0)
    		{
    			tmp.imag() = -sqrt(2)/2;
    			tmp.real() = sqrt(2)/2;
    			return tmp;
    		}
    	}
    }


    int
    qpsk_modulator_adapt_cb_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const char *in0 = (const char *) input_items[0];
      const int *in1 = (const int32_t *) input_items[1];
       gr_complex *out = ( gr_complex*) output_items[0];
      
      for (int i = 0; i < noutput_items; i++)
      {
      	out[i] = map_into_constellation(in0[i], in1[i]);
      }

      consume_each (noutput_items);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace meu */
} /* namespace gr */

