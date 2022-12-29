#!/usr/bin/env python3

from jinja import ARGS
import consts

import numpy as _np
import numpy.polynomial.polynomial as _poly
import matplotlib.pyplot as _plt
from mpmath import mp as _mp, mpf as _mpf

func = lambda a: _mp.sin(a*consts.pi_4)
coefs = ARGS["chebyshev_coefs"]["sin"]()[::-1]
xpoints = _np.linspace(-1, 1, 1000, dtype=_mpf)
ypoints = _poly.polyval(xpoints, coefs) - _np.vectorize(func)(xpoints)

_plt.subplot(221)
_plt.plot(xpoints, ypoints)
_plt.title("error for $sin(\pi x/4)$")
_plt.xlabel("$x$")
_plt.ylabel("$e$")

func = lambda a: _mp.cos(a*consts.pi_4)
coefs = ARGS["chebyshev_coefs"]["cos"]()[::-1]
xpoints = _np.linspace(-1, 1, 1000, dtype=_mpf)
ypoints = _poly.polyval(xpoints, coefs) - _np.vectorize(func)(xpoints)

_plt.subplot(222)
_plt.plot(xpoints, ypoints)
_plt.title("error for $cos(\pi x/4)$")
_plt.xlabel("$x$")
_plt.ylabel("$e$")

func = lambda a: _mp.tan(a*consts.pi_4)
coefs = ARGS["chebyshev_coefs"]["tan"]()[::-1]
xpoints = _np.linspace(-1, 1, 1000, dtype=_mpf)
ypoints = _poly.polyval(xpoints, coefs) - _np.vectorize(func)(xpoints)

_plt.subplot(223)
_plt.plot(xpoints, ypoints)
_plt.title("error for $tan(\pi x/4)$")
_plt.xlabel("$x$")
_plt.ylabel("$e$")

func = lambda x: _mp.powm1(2, x)
coefs = ARGS["chebyshev_coefs"]["exp2m1"]()[::-1]
xpoints = _np.linspace(0, 1, 1000, dtype=_mpf)
ypoints = _poly.polyval(xpoints, coefs) - _np.vectorize(func)(xpoints)

_plt.subplot(224)
_plt.plot(xpoints, ypoints)
_plt.title("error for $2^x-1$")
_plt.xlabel("$x$")
_plt.ylabel("$e$")

_plt.tight_layout()
_plt.show()
