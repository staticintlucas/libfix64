from mpmath import mp as _mp, mpf as _mpf

_mp.prec = 256

zero = _mp.zero
one = _mp.one
two = _mpf(2)
half = _mp.one / 2

pi = _mp.pi
e = _mp.e
log2e = 1 / _mp.ln(2)
log10e = _mp.log10(e)
ln2 = _mp.ln2
ln10 = _mp.ln10
pi_2 = pi / 2
pi_4 = pi / 4
c_1_pi = 1 / pi
c_2_pi = 2 / pi
c_2_sqrtpi = 2 / _mp.sqrt(pi)
sqrt2 = _mp.sqrt(2)
sqrt1_2 = sqrt2 / 2

class Poly:
    def __init__(self, name, func, ival, tol):
        self.name = name
        self.func = func
        self.ival = ival
        self.tol = tol

    def coefs(self):
        N = 6
        maxerr = _mp.inf

        while maxerr > self.tol:
            an, maxerr = _mp.chebyfit(self.func, self.ival, N, error=True)
            N += 1

        print("Using", len(an), "polynomial terms for", self.name)
        return an # reverse to make C implementation slightly simpler
