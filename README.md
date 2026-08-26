# ft_ping

42 project: reimplementation of the `ping` command, referenced against `inetutils-2.0`'s `ping`.

## Build

```
make
```

Produces the `ft_ping` executable. Must be run as root or with `CAP_NET_RAW` (raw ICMP socket).

```
sudo ./ft_ping [OPTIONS] <hostname|ip>
```

> **Status: mandatory part complete, 5 bonus flags implemented.** `make re` passes under `-Wall -Werror -Wextra`, no warnings, and a repeat plain `make` does nothing (no unnecessary recompile/relink).

## Mandatory part — status

| # | Requirement | Status |
|---|---|---|
| 1 | Executable named `ft_ping` | ✅ |
| 2 | Written in C, `Makefile` with usual rules, no unneeded relink | ✅ — a second `make` is now a true no-op |
| 3 | Never crash unexpectedly | ✅ — receive path validates buffer length before parsing IP/ICMP headers |
| 4 | Manage `-v` and `-?` options | ✅ |
| 5 | `-v` must not stop the program on packet-related errors | ✅ |
| 6 | Handle a simple IPv4 address/hostname argument | ✅ |
| 7 | Handle FQDN without doing DNS resolution on the packet reply | ✅ |
| 8 | Output indentation identical to `inetutils-2.0` | ✅ — unconditional `PING host (ip): N data bytes` header, per-reply line, and final `--- host ping statistics ---` summary (transmitted/received/loss%, rtt min/avg/max/mdev) |

## Bonus part — implemented (5/10)

- [x] `-n` — numeric output only (already a no-op given requirement 7 above; parsed and accepted)
- [x] `-T` / `--ttl <n>` — set IP time-to-live via `IP_TTL` sockopt
- [x] `-W <seconds>` — per-reply wait timeout (`SO_RCVTIMEO`)
- [x] `-w <seconds>` — deadline for the whole run
- [x] `-s <bytes>` — set ICMP payload size (default 56, matching `inetutils`/`iputils` default)

Not implemented (descoped by choice, not blocked): `-f` (flood), `-l` (preload), `-p` (pattern), `-r` (bypass routing), `--ip-timestamp`.

## Design notes

- RTT is computed from a timestamp embedded in the ICMP payload on send and read back on receive, rather than trusting the caller's local clock — this keeps the design correct if multiple packets are ever in flight at once (not currently the case, since `-f`/`-l` weren't implemented).
- The send/receive/sleep cycle keeps a consistent ~1s interval between sends regardless of whether a reply arrives quickly or the receive times out.
- IPv6 was investigated and intentionally **not** implemented — it isn't part of the subject's actual bonus list, and the codebase's prior partial `IPV6` scaffolding was dead/broken (socket creation was hardcoded to `AF_INET`), so it was removed rather than left as a landmine.

## References

- [RFC 792 — Internet Control Message Protocol](https://datatracker.ietf.org/doc/html/rfc792)
- [RFC 791, §3.1 — Internet Protocol, header format](https://datatracker.ietf.org/doc/html/rfc791#section-3.1)
- [raw(7) — Linux IPv4 raw sockets](https://man7.org/linux/man-pages/man7/raw.7.html)
- [icmp(7) — Linux IPv4 ICMP kernel module](https://man7.org/linux/man-pages/man7/icmp.7.html)
- [getaddrinfo(3)](https://man7.org/linux/man-pages/man3/getaddrinfo.3.html)
- [ping(8)](https://man7.org/linux/man-pages/man8/ping.8.html)
