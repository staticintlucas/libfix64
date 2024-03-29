#!/usr/bin/env python3

import consts

import sys
from pathlib import Path
import jinja2 as _jinja2
from mpmath import mp as _mp

def const(value, base="x", frac_bits=32, digits=1):
    repr = int(_mp.nint(value * (1 << frac_bits)))
    base = base.lower()
    if base not in "dx":
        raise ValueError("Unsupported base")
    if base == "x":
        digits += 2 # Add 2 for the 0x
    return f"INT64_C({repr:#0{digits}{base}})"

def uconst(value, base="x", frac_bits=32, digits=1):
    return f"U{const(value, base, frac_bits, digits)}"

ARGS = {
    "autogen_comment": "// autogenerated file - edits to this file will be lost",
    "const": const,
    "uconst": uconst,
    "consts": {
        "zero":     { "str": "0",          "val": consts.zero },
        "one":      { "str": "1",          "val": consts.one },
        "half":     { "str": "1/2",        "val": consts.half },
        "two":      { "str": "2",          "val": consts.two },
        "e":        { "str": "e",          "val": consts.e },
        "log2e":    { "str": "log_2(e)",   "val": consts.log2e },
        "log10e":   { "str": "log_10(e)",  "val": consts.log10e },
        "ln2":      { "str": "ln(2)",      "val": consts.ln2 },
        "ln10":     { "str": "ln(10)",     "val": consts.ln10 },
        "pi":       { "str": "pi",         "val": consts.pi },
        "pi_2":     { "str": "pi/2",       "val": consts.pi_2 },
        "pi_4":     { "str": "pi/4",       "val": consts.pi_4 },
        "1_pi":     { "str": "1/pi",       "val": consts.c_1_pi },
        "2_pi":     { "str": "2/pi",       "val": consts.c_2_pi },
        "2_sqrtpi": { "str": "2/sqrt(pi)", "val": consts.c_2_sqrtpi },
        "sqrt2":    { "str": "sqrt(2)",    "val": consts.sqrt2 },
        "sqrt1_2":  { "str": "sqrt(1/2)",  "val": consts.sqrt1_2 },
    },
    "ints": {
        "int":          { "short": "int",  "signed": True },
        "unsigned int": { "short": "uint", "signed": False },
    },
    "floats": {
        "float":       { "short": "flt",  "suffix": "f" },
        "double":      { "short": "dbl",  "suffix": "" },
        "long double": { "short": "ldbl", "suffix": "l" },
    },
    "poly": {
        "sin": consts.Poly("sin(\pi x/4)", lambda a: _mp.sin(a * consts.pi_4), (0, 1), 2**-42),
        "cos": consts.Poly("cos(\pi x/4)", lambda a: _mp.cos(a * consts.pi_4), (0, 1), 2**-42),
        # Proportional error so we can use 1/tan and angle sum identities
        "tan": consts.Poly("tan(\pi x/4)", lambda a: _mp.tan(a * consts.pi / 8), (0, 1), 2**-42, proportional=True),
        "exp2m1": consts.Poly("2^x-1", lambda x: _mp.powm1(2, x), (0, 1), 2**-48),
    },
    "digit_coefs": [
        (len(str(1 << i)), max((1 << 32) - (10 ** len(str(1 << i))), 0))
            for i in range(32)
    ],
    "rounding_coefs": [
        consts.half / (10 ** i) for i in range(len(str(2 ** 64)))
    ],
}

def render(filename, args):
    filename = Path(filename)
    env = _jinja2.Environment(
        keep_trailing_newline=True,
        loader=_jinja2.FileSystemLoader(filename.parent),
        lstrip_blocks=True,
        trim_blocks=True,
        undefined=_jinja2.StrictUndefined)
    template = env.get_template(filename.name)

    return template.render(args).rstrip("\n") + "\n" # ensure exactly one trailing newline


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <input> <output>", file=sys.stderr)
        sys.exit(1)

    with open(sys.argv[2], "w") as f:
        f.write(render(sys.argv[1], ARGS))
