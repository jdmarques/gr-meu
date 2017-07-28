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

#include <gnuradio/expj.h>
#include <gnuradio/io_signature.h>
#include "phase_finder_vci_impl.h"

#define M_TWOPI (2*M_PI)

static const pmt::pmt_t CARR_OFFSET_KEY = pmt::mp("ofdm_sync_carr_offset");

namespace gr {
  namespace meu {

    phase_finder_vci::sptr
    phase_finder_vci::make(int cp_len, int fft_len)
    {
      return gnuradio::get_initial_sptr
      (new phase_finder_vci_impl(cp_len, fft_len));
    }

    /*
     * The private constructor
     */
    phase_finder_vci_impl::phase_finder_vci_impl(int cp_len, int fft_len)
    : gr::block("phase_finder_vci",
      gr::io_signature::make(1, 1, sizeof (gr_complex) * fft_len),
      gr::io_signature::make(1, 1, sizeof(int))),
    d_cp_len(cp_len),
    d_fft_len(fft_len),
    d_channel_state(fft_len, gr_complex(1, 0))
    { set_tag_propagation_policy(TPP_DONT);}

    /*
     * Our virtual destructor.
     */
    phase_finder_vci_impl::~phase_finder_vci_impl()
    {
    }

    void
    phase_finder_vci_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = noutput_items;
    }


    int
    phase_finder_vci_impl::get_ref_phase(const gr_complex &sample)
    {
       /* since phase_correction is a complex number uses the value of imag and real components to check in which quadrant it is */
      if (sample.imag() >= 0 and sample.real() >= 0) {
        return 1;
      }
      else if (sample.imag() >= 0 and sample.real() < 0) {
        return 0;
      }
      else if (sample.imag() < 0 and sample.real() < 0) {
        return 2;
      }
      else if (sample.imag() < 0 and sample.real() >= 0) {
        return 3;
      }

    }

    int
    phase_finder_vci_impl::general_work (int noutput_items,
     gr_vector_int &ninput_items,
     gr_vector_const_void_star &input_items,
     gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      int *out = (int *) output_items[0];
      int carrier_offset = 0;
      int fft_len = d_fft_len;

      // get tags will get the value of carrier offset that was propagated in the tag 
      std::vector<tag_t> tags;
      get_tags_in_window(tags, 0, 0, 1);
      for (unsigned i = 0; i < tags.size(); i++) {
        if (pmt::symbol_to_string(tags[i].key) == "ofdm_sync_chan_taps") {
          d_channel_state = pmt::c32vector_elements(tags[i].value);
        }
      }

      // Correct the frequency shift on the symbols
     // gr_complex phase_correction;
      for (int i = 0; i < noutput_items; i++) {
      // phase_correction = gr_expj(-M_TWOPI * carrier_offset * d_cp_len / d_fft_len * (i+1));
       // out[i] = get_ref_phase(phase_correction); /* phase reference output*/
       out[i]= get_ref_phase(d_channel_state[12]);
     }

     consume_each (noutput_items);

      // Tell runtime system how many output items we produced.
     return noutput_items;
   }

  } /* namespace meu */
} /* namespace gr */

