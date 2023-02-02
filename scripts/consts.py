import gmpy2 as _
from mpmath import mp as _mp, mpf as _mpf, libmp as _libmp

assert _libmp.BACKEND == "gmpy"
_mp.prec = 128

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
    def __init__(self, name, func, ival, tol, proportional=False):
        self.name = name
        self.proportional = proportional
        if proportional:
            self.orig_func = func
            self.func = lambda x: (func(x) / x) if x else func(2**-_mp.prec) / (2**-_mp.prec)
        else:
            self.orig_func = func
            self.func = func
        self.ival = ival
        self.tol = tol

    @staticmethod
    def _extrema(func, ival):
        # returns the x-coords of the extrema of the polynomial's error

        def approx_extr(func, ival, pts):
            # Finds the approximate extrema by interpolating the function at pts points
            x = _mp.linspace(*ival, pts+1)
            y = [func(xi) for xi in x]

            dy = [y1 - y0 for y0, y1 in zip(y, y[1:])] # the difference between successive y points
            dyneg = [dyi < 0 for dyi in dy] # find where difference is negative
            # find where difference swaps from pos->neg and vice versa
            cross = [dy0 != dy1 for dy0, dy1 in zip(dyneg, dyneg[1:])]
            idx = [i for i, x in enumerate(cross) if x] # find indeces of the crossings

            return [x[i] for i in idx] # return the x-coords

        pts = 1000
        dx = 2 * (ival[1] - ival[0]) / pts # delta between points
        approx = approx_extr(func, ival, pts) # find approx extrema

        dfunc = lambda x: _mp.diff(func, x) # differentiate function
        solver = "pegasus" # seems to give correct roots at the best speed
        return (_mp.findroot(dfunc, (a-dx, a+dx), solver) for a in approx)

    @staticmethod
    def _remez(func, coefs, ival):
        efunc = lambda x: _mp.polyval(coefs, x) - func(x)
        extrema = _mp.matrix([ival[0], *Poly._extrema(efunc, ival), ival[1]])

        N = len(coefs) - 1
        # for some functions (e.g. sin) we get an extra extreme due to the curvature of sin itself
        extrema = extrema[:N+2]
        esign = -1 if efunc(extrema[0]) < efunc(extrema[1]) else 1 # if the first error term is negative

        # populate the vandermonde matrix
        # see: https://2π.com/22/approximation/
        vandermonde = []
        for i in range(N + 2):
            vandermonde.append([
                *(extrema[i]**j for j in range(N, -1, -1)),
                esign
            ])
            esign = -esign
        vandermonde = _mp.matrix(vandermonde)

        # vector of y coordinates
        yvec = _mp.matrix([func(x) for x in extrema])

        # solve vandermonde * [c0, c1, ..., cn, e] = yvec
        ans = _mp.lu_solve(vandermonde, yvec)

        coefs = ans[:-1] # new coefficients
        esterr = ans[-1] # error term
        return coefs, esterr

    def coefs(self):
        # give up if a 100th degree polynomial is still not enough
        for N in range(6, 1000):
            # Calculate chebyshev polynomial
            an, chebyerr = _mp.chebyfit(self.func, self.ival, N, error=True)

            # Remez gives at best a ~2.2x improvement, so we only try to improve our chebyshev
            #  polynomial if we're off by less than 2.5. This avoids the additional runtime since
            # Remez isn't the fastest
            if chebyerr < 2.5 * self.tol:
                N_eff = N + 1 if self.proportional else N # if proportional we effectively have one other zero term
                print(f"{self.name}: Chebyshev polynomial of degree {N_eff}; e_max = {_mp.nstr(chebyerr, strip_zeros=False)}")

                # A couple iterations of Remez already gives near-optimal results
                old_err = _mp.inf
                j = 0
                while True:
                    an, _remezerr = Poly._remez(self.func, an, self.ival)
                    efunc = lambda x: _mp.polyval(an, x) - self.func(x)
                    err = max(abs(efunc(x)) for x in Poly._extrema(efunc, self.ival))
                    if _mp.fabs(old_err - err) / err < 2**-_mp.prec:
                        break
                    old_err = err
                    j = j + 1

                print(f"{self.name}: Remez algorithm ({j+1} iters); e_max = {_mp.nstr(err, strip_zeros=False)}",
                    f"({_mp.nstr(chebyerr/err, strip_zeros=False)}x smaller)")

                # If the error from Remez is good enough break and return
                if err < self.tol:
                    break

        if self.proportional:
            an = [*an, _mp.zero] # Add extra term to multiply by x
        return an
