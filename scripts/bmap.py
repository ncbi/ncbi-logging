#!/usr/bin/env python3.13

import sys

# bits, from Ken's "sratools print-argbits"
# prefetch        3.4.2   0x0000000000001000 (1 << 12)    min-size

fbits = open("bits")
toolbit = {}
for line in fbits:
    fields = line.split()
    if len(fields) == 7:
        # ['vdb-dump', '3.4.2', '0x0000010000000000', '(1', '<<', '40)', 'schema']
        tool = fields[0]
        ver = fields[1]
        bit = int(fields[5][:-1])
        arg = fields[6]
        if tool not in toolbit:
            toolbit[tool] = {}
        toolbit[tool][bit] = arg

tools = list(toolbit.keys())
print(f"{tools=}", file=sys.stderr)
for tool in tools:
    print(f"{tool}: {toolbit[tool]}", file=sys.stderr)
# print(f"{toolbit=}")

# bmaps, from BigQuery "top bmaps":
# SELECT regexp_extract(user_agent, r'bmap=[0-9aA-z]+') as bmap, count(*) as cnt
# FROM `ncbi-logmon.strides_analytics.test_user_agents_2026`
# group by bmap
# order by cnt desc

# nob,24229879
# 2128090,9863427
# 800,8991867

for tool in tools:
    print(f"{tool}", file=sys.stderr)
    bmaps = open("bmaps")
    for line in bmaps:
        fields = line.split(",")
        bmap = fields[0]
        if bmap == "nob":
            continue
        bmap = int(bmap, 16)
        opts = []
        print(f"  {bmap=:x} {bmap=:b}", file=sys.stderr)

        for bit in range(64):
            val = 1 << bit
            if bmap & val:
                print(f"  Bit {bit} set", file=sys.stderr)
                arg = toolbit[tool].get(bit, "Unk")
                if arg != "Unk":
                    print(f"  {tool} {arg=}", file=sys.stderr)
                    opts.append(arg)
        print(opts, file=sys.stderr)
        opts = " ".join(opts)
        print(f"{tool},{bmap:X},{opts}")
