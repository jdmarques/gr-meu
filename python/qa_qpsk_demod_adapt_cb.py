#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2017 <+YOU OR YOUR COMPANY+>.
# 
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

from gnuradio import gr, gr_unittest
from gnuradio import blocks
import meu_swig as meu
from numpy import array

class qa_qpsk_demod_adapt_cb (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_t_ref_0 (self):
        # set up fg
        ref=array([0,0,0,0])

        Iphase=array([1,-1,-1,1])
        Qphase=array([1,1,-1,-1])
        src_data = Iphase + 1j*Qphase;
        
        expected_result=(0,1,3,2)
        src=blocks.vector_source_c(src_data)
        ref=blocks.vector_source_c(ref)
        qpsk_demod = meu.qpsk_demod_adapt_cb()
        dst=blocks.vector_sink_b();

        self.tb.connect(src,(qpsk_demod,0))
        self.tb.connect(ref,(qpsk_demod,1))
        self.tb.connect(qpsk_demod,dst)

        self.tb.run ()
        # check data
        result_data=dst.data()
        self.assertTupleEqual(expected_result,result_data)
        self.assertEqual(len(expected_result),len(result_data))


if __name__ == '__main__':
    gr_unittest.run(qa_qpsk_demod_adapt_cb, "qa_qpsk_demod_adapt_cb.xml")
