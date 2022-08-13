#!/usr/bin/env python3

import consts

from decimal import Decimal
import jinja2
from pathlib import Path
import sys
import math

try:
    filename = Path(sys.argv[1])
except IndexError:
    print(f"Usage: {sys.argv[0]} <jinja file>")
    sys.exit(1)

TYPE = "fix64"
REPR = "int64"
FRAC_BITS = 32
CHEBYSHEV_FRAC_BITS = 62

def repr_const(value, base="x", digits=1):
    base = base.lower()
    if base not in "dx":
        raise ValueError("Unsupported base")
    return f"{REPR.upper()}_C({value:#0{digits}{base}})"

def const(value, base="x", digits=1):
    repr = int(round(value * (1 << FRAC_BITS)))
    return repr_const(repr, base, digits)

def cheby_const(value, base="x", digits=1):
    repr = int(round(value * (1 << CHEBYSHEV_FRAC_BITS)))
    return repr_const(repr, base, digits)

args = {
    "autogen_comment": "// autogenerated file - edits to this file will be lost",
    "type": TYPE,
    "const": const,
    "int_bits": (64 - 1 - FRAC_BITS), # -1 for sign bit
    "frac_bits": FRAC_BITS,
    "repr": {
        "type": REPR,
        "const": repr_const,
    },
    "float_types": {
        "float": {"short": "flt", "suffix": "f"},
        "double": {"short": "dbl", "suffix": ""},
        "long double": {"short": "ldbl", "suffix": "l"},
    },
    "nums": {
        "zero": Decimal(0),
        "one": Decimal(1),
        "half": Decimal(0.5),
        "two": Decimal(2),
    },
    "consts": {
        "e": consts.e,
        "log_2(e)": consts.log2e,
        "log_10(e)": consts.log10e,
        "ln(2)": consts.ln2,
        "ln(10)": consts.ln10,
        "pi": consts.pi,
        "pi/2": consts.pi_2,
        "pi/4": consts.pi_4,
        "1/pi": consts.c_1_pi,
        "2/pi": consts.c_2_pi,
        "2/sqrt(pi)": consts.c_2_sqrtpi,
        "sqrt(2)": consts.sqrt2,
        "sqrt(1/2)": consts.sqrt1_2,
    },
    "chebyshev": {
        "frac_bits": CHEBYSHEV_FRAC_BITS,
        "const": cheby_const,
        "sin": {
            "coefs": consts.chebyshev_coefs(math.sin, [0, consts.pi/4], 10),
        },
        "cos": {
            "coefs": consts.chebyshev_coefs(math.cos, [0, consts.pi/4], 10),
        },
        "tan": {
            "coefs": consts.chebyshev_coefs(math.tan, [0, consts.pi/4], 22),
        }
    }
}

env = jinja2.Environment(
    loader=jinja2.FileSystemLoader('src'),
    lstrip_blocks=True,
    trim_blocks=True,
    undefined=jinja2.StrictUndefined)
template = env.get_template(filename.name)

result = template.render(args)

print(result)
