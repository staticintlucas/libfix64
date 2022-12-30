from mpmath import mp as _mp, mpf as _mpf
import numpy as _np

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

    # @staticmethod
    # def _diff(coefs):
    #     # differentiate a polynomial
    #     N = len(coefs) - 1
    #     return [c * (N - i) for i, c in enumerate(coefs)][:-1]

    # @staticmethod
    # def _roots(coefs):
    #     # x-coords of the roots of a polynomial
    #     return _mp.polyroots(coefs)

    @staticmethod
    def _extrema(func, ival):
        # x-coords of the extrema of the polynomial's error

        def maximum(func, ival, neg=False):
            if neg:
                old = func
                func = lambda x: -old(x)

            x = _mp.linspace(*ival, 11)
            y = [func(xi) for xi in x]
            while max(y) - min(y) > _mp.power(2, -0.75*_mp.prec):
                i = y.index(max(y))
                ival = (x[i-1], x[i+1])
                x = _mp.linspace(*ival, 11)
                y = [func(xi) for xi in x]
            return x[i]

        def approx_extr(func, ival, pts):
            x = _mp.linspace(*ival, pts+1)
            y = [func(xi) for xi in x]
            dy = [y1 - y0 for y0, y1 in zip(y, y[1:])]
            dyneg = [dyi < 0 for dyi in dy]
            cross = [dy0 != dy1 for dy0, dy1 in zip(dyneg, dyneg[1:])]
            idx = [i for i, x in enumerate(cross) if x]
            return [x[i] for i in idx]

        pts = 1000
        dx = 2 * (ival[1] - ival[0]) / pts
        approx = approx_extr(func, ival, pts)
        ivals = [(a - dx, a + dx) for a in approx]
        neg = [func(a) < func(a - dx) for a in approx]
        return (maximum(func, ival, n) for ival, n in zip(ivals, neg))

    @staticmethod
    def _remez(func, coefs, ival):
        efunc = lambda x: _mp.polyval(coefs, x) - func(x)
        extrema = _mp.matrix([ival[0], *Poly._extrema(efunc, ival), ival[1]])

        N = len(coefs) - 1
        # for some functions (e.g. sin) we get an extra extreme due to the curvature of sin itself
        extrema = extrema[:N+2]
        esign = -1 if efunc(extrema[0]) < efunc(extrema[1]) else 1 # if the first error term is negative
        vandermonde = []

        for i in range(N + 2):
            vandermonde.append([
                *(extrema[i]**j for j in range(N, -1, -1)),
                esign
            ])
            esign = -esign

        vandermonde = _mp.matrix(vandermonde)
        yvec = _mp.matrix([func(x) for x in extrema])
        ans = _mp.lu_solve(vandermonde, yvec)

        coefs = ans[:-1]
        esterr = ans[-1]
        return coefs, esterr

    def coefs(self):
        for N in range(6, 1000):
            an, chebyerr = _mp.chebyfit(self.func, self.ival, N, error=True)

            # Remez gives at best a ~2.2x improvement, so only run it if we're off by less
            if chebyerr < 2.5 * self.tol:
                print(f"{self.name}: Chebyshev polynomial of degree {N}; e_max = {_mp.nstr(chebyerr, strip_zeros=False)}")

                # 5 iterations already gives near-optimal results
                for j in range(5):
                    an, _remezerr = Poly._remez(self.func, an, self.ival)
                    efunc = lambda x: _mp.polyval(an, x) - self.func(x)
                    err = max(abs(efunc(x)) for x in Poly._extrema(efunc, self.ival))

                print(f"{self.name}: Remez algorithm ({j+1} iters); e_max = {_mp.nstr(err, strip_zeros=False)}",
                    f"({_mp.nstr(chebyerr/err, strip_zeros=False)}x smaller)")

                if err < self.tol:
                    break
        return an
