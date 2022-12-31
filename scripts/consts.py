from mpmath import mp as _mp
import sympy as _sp
from sympy.abc import x as _x

_mp.prec = 128

zero = _sp.S(0)
one = _sp.S(1)
two = _sp.S(2)
half = one / two

pi = _sp.pi
e = _sp.E
log2e = _sp.log(e, 2)
log10e = _sp.log(e, 10)
ln2 = _sp.log(2)
ln10 = _sp.log(10)
pi_2 = pi / 2
pi_4 = pi / 4
c_1_pi = 1 / pi
c_2_pi = 2 / pi
c_2_sqrtpi = 2 / _sp.sqrt(pi)
sqrt2 = _sp.sqrt(2)
sqrt1_2 = 1 / sqrt2

class Poly:
    def __init__(self, func, ival, tol):
        self.name = str(func)
        self.func = func
        self.ival = ival
        self.tol = tol

    @staticmethod
    def _extrema(efunc, ival):
        # returns the x-coords of the extrema of the polynomial's error

        def approx_roots(ldefunc, ival):
            # Finds the approximate roots by interpolating the function at 1000 points
            x = _mp.linspace(*ival, 1001)
            y = (ldefunc(xi) for xi in x)

            yneg = [yi < 0 for yi in y] # find where difference is negative
            # find where difference swaps from pos->neg and vice versa
            cross = (dy0 != dy1 for dy0, dy1 in zip(yneg, yneg[1:]))
            idx = list(i for i, x in enumerate(cross) if x) # find indeces of the crossings
            return (_sp.S(x[i]) for i in idx) # return the x-coords

        defunc = _sp.diff(efunc, _x)
        ldefunc = _sp.lambdify(_x, defunc, modules="mpmath")
        approx = list(approx_roots(ldefunc, ival)) # find approx extrema
        return (_sp.nsolve(defunc, _x, a, solver="mnewton") for a in approx) # return exact extrema

    @staticmethod
    def _remez(func, poly, ival):
        efunc = poly.as_expr() - func
        extrema = [ival[0], *Poly._extrema(efunc, ival), ival[1]]

        N = poly.degree()
        # for some functions (e.g. sin) we get an extra extreme due to the curvature of sin itself
        extrema = extrema[:N+2]
        # sign of the first error term
        lefunc = _sp.lambdify(_x, efunc, modules="mpmath")
        esign = -1 if lefunc(extrema[0]) < lefunc(extrema[1]) else 1

        # populate the vandermonde matrix
        # see: https://2π.com/22/approximation/
        vandermonde = _sp.Matrix(N + 2, N + 1, lambda i, j: extrema[i] ** j)
        vandermonde = vandermonde.row_join(_sp.Matrix([esign * (-1)**n for n in range(N + 2)]))

        # vector of y coordinates
        lfunc = _sp.lambdify(_x, func, modules="mpmath")
        yvec = _sp.Matrix([lfunc(x) for x in extrema])

        # solve vandermonde * [c0, c1, ..., cn, e] = yvec
        ans = _mp.lu_solve(vandermonde, yvec)

        poly = _sp.Poly(ans[:-1:-1], _x) # new polynomial
        esterr = ans[-1] # error term
        return poly, esterr

    def poly(self):
        # give up if a 100th degree polynomial is still not enough
        for N in range(6, 1000):
            # Calculate chebyshev polynomial
            lfunc = _sp.lambdify(_x, self.func, modules="mpmath")
            an, chebyerr = _mp.chebyfit(lfunc, self.ival, N, error=True)
            poly = _sp.Poly(an, _x)

            # Remez gives at best a ~2.2x improvement, so we only try to improve our chebyshev
            #  polynomial if we're off by less than 2.5. This avoids the additional runtime since
            # Remez isn't the fastest
            if chebyerr < 2.5 * self.tol:
                print(f"{self.name}: Chebyshev polynomial of degree {N}; e_max = {_mp.nstr(chebyerr, strip_zeros=False)}")

                # 5 iterations of Remez already gives near-optimal results
                # TODO should we have a better stop condition?
                for j in range(5):
                    poly, _remezerr = Poly._remez(self.func, poly, self.ival)
                    efunc = poly.as_expr() - self.func
                    lefunc = _sp.lambdify(_x, efunc, modules="mpmath")
                    err = max(_mp.fabs(lefunc(x)) for x in Poly._extrema(efunc, self.ival))

                print(f"{self.name}: Remez algorithm ({j+1} iters); e_max = {_mp.nstr(err, strip_zeros=False)}",
                    f"({_mp.nstr(chebyerr/err, strip_zeros=False)}x smaller)")

                # If the error from Remez is good enough break and return
                if err < self.tol:
                    return poly

    def coefs(self):
        return self.poly().all_coeffs()
