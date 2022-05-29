#
# (C) 2020-2021 Bernd Porr, mail@berndporr.me.uk
# Apache 2.0 license
#
import numpy as np
import unittest

class IIR2_filter:
    """2nd order IIR filter"""

    def __init__(self,s):
        """Instantiates a 2nd order IIR filter
        s -- numerator and denominator coefficients
        """
        self.numerator0 = s[0]
        self.numerator1 = s[1]
        self.numerator2 = s[2]
        self.denominator1 = s[4]
        self.denominator2 = s[5]
        self.buffer1 = 0
        self.buffer2 = 0

    def filter(self,v):
        """Sample by sample filtering
        v -- scalar sample
        returns filtered sample
        """
        input = v - (self.denominator1 * self.buffer1) - (self.denominator2 * self.buffer2)
        output = (self.numerator1 * self.buffer1) + (self.numerator2 * self.buffer2) + input * self.numerator0
        self.buffer2 = self.buffer1
        self.buffer1 = input
        return output

class IIR_filter:
    """IIR filter"""
    def __init__(self,sos):
        """Instantiates an IIR filter of any order
        sos -- array of 2nd order IIR filter coefficients
        """
        self.cascade = []
        for s in sos:
            self.cascade.append(IIR2_filter(s))

    def filter(self,v):
        """Sample by sample filtering
        v -- scalar sample
        returns filtered sample
        """
        for f in self.cascade:
            v = f.filter(v)
        return v







class TestFilters(unittest.TestCase):

    coeff1 = [
        [ 0.02008337,  0.04016673,  0.02008337,
          1.,         -1.56101808,  0.64135154]
    ]

    input1 = [
	-1.0,
	0.5,
        1.0
    ]

    result1 = [
	-2.00833656e-02,
	-6.14755450e-02,
	-6.30005740e-02
    ]
    
    coeff2 = [
        [1.78260999e-03,  3.56521998e-03,  1.78260999e-03,
         1.00000000e+00,  -1.25544047e+00, 4.09013783e-01],
        [1.00000000e+00,  2.00000000e+00,  1.00000000e+00,
         1.00000000e+00,  -1.51824184e+00, 7.03962657e-01]
    ]
    
    input2 = [
	-1,0.5,-1,0.5,-0.3,3,-1E-5
    ]

    result2 = [
	-0.00178261, -0.01118353, -0.03455084, -0.07277369, -0.11973872, -0.158864,
	-0.15873629
    ]

    def test1(self):
        f = IIR_filter(self.coeff1)
        for i,r in zip(self.input1,self.result1):
            self.assertAlmostEqual(r,f.filter(i))

    def test2(self):
        f = IIR_filter(self.coeff2)
        for i,r in zip(self.input2,self.result2):
            self.assertAlmostEqual(r,f.filter(i))

    def test3(self):
        f = IIR2_filter(self.coeff1[0])
        for i,r in zip(self.input1,self.result1):
            self.assertAlmostEqual(r,f.filter(i))


            
if __name__ == '__main__':
    unittest.main()
