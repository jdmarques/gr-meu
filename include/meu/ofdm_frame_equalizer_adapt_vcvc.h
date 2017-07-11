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


#ifndef INCLUDED_MEU_OFDM_FRAME_EQUALIZER_ADAPT_VCVC_H
#define INCLUDED_MEU_OFDM_FRAME_EQUALIZER_ADAPT_VCVC_H

#include <meu/api.h>
#include <gnuradio/digital/api.h>
#include <gnuradio/tagged_stream_block.h>
#include <gnuradio/digital/ofdm_equalizer_base.h>

namespace gr {
  namespace meu {


   /*!
   * \brief OFDM frame equalizer
   * \ingroup ofdm_blk
   *
   * \details
   * Performs equalization in one or two dimensions on a tagged OFDM frame.
   *
   * This does two things:
   * First, it removes the coarse carrier offset. If a tag is found on the first
   * item with the key 'ofdm_sync_carr_offset', this is interpreted as the coarse
   * frequency offset in number of carriers.
   * Next, it performs equalization in one or two dimensions on a tagged OFDM frame.
   * The actual equalization is done by a ofdm_frame_equalizer object, outside of
   * the block.
   *
   * Note that the tag with the coarse carrier offset is not removed. Blocks
   * downstream from this block must not attempt to also correct this offset.
   *
   * Input: a tagged series of OFDM symbols.
   * Output1: The same as the input, but equalized and frequency-corrected; 
   * Output Phase-ref : determines a reference from the phase correction value determined during the equalization 
  */
    class MEU_API ofdm_frame_equalizer_adapt_vcvc : virtual public gr::tagged_stream_block
    {
     public:
      typedef boost::shared_ptr<ofdm_frame_equalizer_adapt_vcvc> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of ofdmadapt::ofdm_frame_equalizer_adapt_vcvc.
       *
       * To avoid accidental use of raw pointers, ofdmadapt::ofdm_frame_equalizer_adapt_vcvc's
       * constructor is in a private implementation
       * class. ofdmadapt::ofdm_frame_equalizer_adapt_vcvc::make is the public interface for
       * creating new instances.
       */
      static sptr make(        
          gr::digital::ofdm_equalizer_base::sptr equalizer,
          int cp_len,
          const std::string &tsb_key="frame_len",
          bool propagate_channel_state=false,
          int fixed_frame_len=0);
    };

  } // namespace ofdmadapt
} // namespace gr

#endif /* INCLUDED_MEU_OFDM_FRAME_EQUALIZER_ADAPT_VCVC_H */

