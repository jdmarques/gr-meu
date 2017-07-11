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
#include <gnuradio/digital/ofdm_equalizer_base.h>
#include "ofdm_frame_equalizer_adapt_vcvc_impl.h"

#define M_TWOPI (2*M_PI)

static const pmt::pmt_t CARR_OFFSET_KEY = pmt::mp("ofdm_sync_carr_offset");
static const pmt::pmt_t CHAN_TAPS_KEY = pmt::mp("ofdm_sync_chan_taps");

namespace gr {
  namespace meu {

    ofdm_frame_equalizer_adapt_vcvc::sptr
    ofdm_frame_equalizer_adapt_vcvc::make(  
  gr::digital::ofdm_equalizer_base::sptr equalizer,
  int cp_len,
  const std::string &tsb_key,
  bool propagate_channel_state,
  int fixed_frame_len)
    {
      return gnuradio::get_initial_sptr
        (new ofdm_frame_equalizer_adapt_vcvc_impl(equalizer, cp_len, tsb_key, propagate_channel_state, fixed_frame_len));
    }

    /*
     * The private constructor
     */
    ofdm_frame_equalizer_adapt_vcvc_impl::ofdm_frame_equalizer_adapt_vcvc_impl(gr::digital::ofdm_equalizer_base::sptr equalizer,
      int cp_len,
      const std::string &tsb_key,
      bool propagate_channel_state,
      int fixed_frame_len)
      : gr::tagged_stream_block("ofdm_frame_equalizer_adapt_vcvc",
              gr::io_signature::make(1, 1, sizeof (gr_complex) * equalizer->fft_len()),
              gr::io_signature::make2(2, 2, sizeof (gr_complex) * equalizer->fft_len(), sizeof(int32_t)), tsb_key) ,  /* add a output (int) that's gonna be my reference */ 
      d_fft_len(equalizer->fft_len()),
      d_cp_len(cp_len),
      d_eq(equalizer),
      d_propagate_channel_state(propagate_channel_state),
      d_fixed_frame_len(fixed_frame_len),
      d_channel_state(equalizer->fft_len(), gr_complex(1, 0))
    {
      if (tsb_key.empty() && fixed_frame_len == 0) {
      throw std::invalid_argument("Either specify a TSB tag or a fixed frame length!");
      }
      if (d_fixed_frame_len < 0) {
        throw std::invalid_argument("Invalid frame length!");
      }
      if (d_fixed_frame_len) {
        set_output_multiple(d_fixed_frame_len);
      }
      set_relative_rate(1.0);
      // Really, we have TPP_ONE_TO_ONE, but the channel state is not propagated
      set_tag_propagation_policy(TPP_DONT);
    }

    /*
     * Our virtual destructor.
     */
    ofdm_frame_equalizer_adapt_vcvc_impl::~ofdm_frame_equalizer_adapt_vcvc_impl()
    {
    }

    void
    ofdm_frame_equalizer_adapt_vcvc_impl::parse_length_tags(
        const std::vector<std::vector<tag_t> > &tags,
        gr_vector_int &n_input_items_reqd
    ) {
      if (d_fixed_frame_len) {
        n_input_items_reqd[0] = d_fixed_frame_len;
      } else {
        for (unsigned k = 0; k < tags[0].size(); k++) {
          if (tags[0][k].key == pmt::string_to_symbol(d_length_tag_key_str)) {
            n_input_items_reqd[0] = pmt::to_long(tags[0][k].value);
          }
        }
      }
    }

    int
    ofdm_frame_equalizer_adapt_vcvc_impl::get_ref_phase(const gr_complex &sample)
    {
       /* since phase_correction is a complex number uses the value of imag and real components to check in which quadrant it is */
      if (sample.imag() >= 0 and sample.real() >= 0) {
        return 0;
      }
      else if (sample.imag() >= 0 and sample.real() < 0) {
        return 1;
      }
      else if (sample.imag() < 0 and sample.real() < 0) {
        return 2;
      }
      else if (sample.imag() < 0 and sample.real() >= 0) {
        return 3;
      }

    }

    int
    ofdm_frame_equalizer_adapt_vcvc_impl::work (int noutput_items,
    gr_vector_int &ninput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];
       gr_complex *out1 = (gr_complex *) output_items[1];
      int carrier_offset = 0;
      int frame_len = 0;
      if (d_fixed_frame_len) {
  frame_len = d_fixed_frame_len;
      } else {
  frame_len = ninput_items[0];
      }

      std::vector<tag_t> tags;
      get_tags_in_window(tags, 0, 0, 1);
      for (unsigned i = 0; i < tags.size(); i++) {
        if (pmt::symbol_to_string(tags[i].key) == "ofdm_sync_chan_taps") {
          d_channel_state = pmt::c32vector_elements(tags[i].value);
        }
        if (pmt::symbol_to_string(tags[i].key) == "ofdm_sync_carr_offset") {
          carrier_offset = pmt::to_long(tags[i].value);
        }
      }

      // Copy the frame and the channel state vector such that the symbols are shifted to the correct position
      if (carrier_offset < 0) {
  memset((void *) out, 0x00, sizeof(gr_complex) * (-carrier_offset));
  memcpy(
      (void *) &out[-carrier_offset], (void *) in,
      sizeof(gr_complex) * (d_fft_len * frame_len + carrier_offset)
  );
      } else {
  memset((void *) (out + d_fft_len * frame_len - carrier_offset), 0x00, sizeof(gr_complex) * carrier_offset);
  memcpy(
      (void *) out, (void *) (in+carrier_offset),
      sizeof(gr_complex) * (d_fft_len * frame_len - carrier_offset)
  );
      }

      // Correct the frequency shift on the symbols
      gr_complex phase_correction;
      for (int i = 0; i < frame_len; i++) {
  phase_correction = gr_expj(-M_TWOPI * carrier_offset * d_cp_len / d_fft_len * (i+1));
  for (int k = 0; k < d_fft_len; k++) {
    out[i*d_fft_len+k] *= phase_correction;
    out1[i*d_fft_len+k] = get_ref_phase(phase_correction); /* phase reference output*/
  }
      }

      // Do the equalizing
      d_eq->reset();
      d_eq->equalize(out, frame_len, d_channel_state);
      d_eq->get_channel_state(d_channel_state);

      // Update the channel state regarding the frequency offset
      phase_correction = gr_expj(M_TWOPI * carrier_offset * d_cp_len / d_fft_len * frame_len);
      for (int k = 0; k < d_fft_len; k++) {
        d_channel_state[k] *= phase_correction;
      }

      // Propagate tags (except for the channel state and the TSB tag)
      get_tags_in_window(tags, 0, 0, frame_len);
      for (size_t i = 0; i < tags.size(); i++) {
        if (tags[i].key != CHAN_TAPS_KEY
            && tags[i].key != pmt::mp(d_length_tag_key_str)) {
          add_item_tag(0, tags[i]);
        }
      }

      // Housekeeping
      if (d_propagate_channel_state) {
  add_item_tag(0, nitems_written(0),
      pmt::string_to_symbol("ofdm_sync_chan_taps"),
      pmt::init_c32vector(d_fft_len, d_channel_state));
      }

      if (d_fixed_frame_len && d_length_tag_key_str.empty()) {
  consume_each(frame_len);
      }

      return frame_len;
    }

  } /* namespace digital */
} /* namespace gr */


