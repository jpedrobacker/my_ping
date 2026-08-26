# ft_ping

42 project: reimplementation of the `ping` command, referenced against `inetutils-2.0`'s `ping`.

## Build

```
make
```

Produces the `ft_ping` executable. Must be run as root or with `CAP_NET_RAW` (raw ICMP socket).

```
sudo ./ft_ping <hostname|ip>
```

> **Status: the project builds cleanly** (`make re` passes under `-Wall -Werror -Wextra`, no warnings). All 8 blocking bugs below are fixed. See [Missing pieces](#missing-pieces-not-bugs-just-not-written-yet) for what's still needed to be spec-complete.

## Mandatory part — status

| # | Requirement | Status | Notes |
|---|---|---|---|
| 1 | Executable named `ft_ping` | ✅ | `Makefile` `NAME = ft_ping` |
| 2 | Written in C, `Makefile` with usual rules, no unneeded relink | ✅ | `all` / `clean` / `fclean` / `re` present |
| 3 | Never crash unexpectedly (segfault, bus error, double free...) | ⚠️ likely, not fully verified | builds clean, argument-parsing and permission-denied paths tested manually with no crash; the raw-socket send/receive path hasn't been exercised as root yet |
| 4 | Manage `-v` and `-?` options | ✅ | both are parsed and don't conflict; `-?` prints usage/help text and exits before opening any socket (works with or without a hostname argument) |
| 5 | `-v` must not stop the program on packet-related errors | ✅ | send errors are `perror`'d and don't abort; `listen_packet_reply` now reports non-`ICMP_ECHOREPLY` types (destination unreachable, TTL exceeded, etc.) under `-v` instead of silently dropping them, with no change to the success path |
| 6 | Handle a simple IPv4 address/hostname argument | ✅ | `getaddrinfo` resolution bug fixed, `sockfd`/`remote_addr`/`addr_len` correctly stored and used for `sendto` |
| 7 | Handle FQDN without doing DNS resolution on the packet reply | ✅ | forward resolution via `getaddrinfo`, reply printed with `inet_ntoa` (numeric only) |
| 8 | Output indentation identical to `inetutils-2.0` (except RTT line / reverse DNS) | ❌ | two gaps: (a) the `PING host (ip): N data bytes` header only prints under `-v` — the subject only gates *error* reporting behind `-v`, not the base output, so a plain `ft_ping host` currently prints no header at all; (b) no final `--- <host> ping statistics ---` summary block (packets transmitted/received, loss %, rtt min/avg/max) exists |

## Blocking bugs — fixed

All 8 originally found here are resolved (see git history for the diffs):

1. ~~`include/icmp.h` didn't compile~~ — now forward-declares `t_ping`/`struct timeval` itself instead of depending on inclusion order, which also broke the `main.h` ⇄ `parse.h` ⇄ `icmp.h` circular include when `parse.c` was the compile entry point.
2. ~~`include/sig_handle.h` redefined `_POSIX_C_SOURCE`~~ — redundant `#define`s dropped.
3. ~~`src/addr_config.c` `getaddrinfo(..., res)`~~ — now passes `&res`.
4. ~~`main_loop` called `send_icmp_packet(ping, sockfd, res, seq++)` with an undeclared `res`~~ — `send_icmp_packet` now takes no `addrinfo*` (which `config_addr` frees anyway) and sends via `ping->socket.remote_addr`/`addr_len`, which `config_addr` now populates.
5. ~~`printf("PING %s (%s): %ld data bytes\n")` with no arguments~~ — now passed `ping->hostname`, `ping->raw_ip`, and the data length (via the new shared `ICMP_PACKET_SIZE` macro in `icmp.h`).
6. ~~`init_ping()` used `sizeof(ping)`~~ — fixed to `sizeof(*ping)`.
7. ~~SIGINT handling was double-hop~~ — `main_loop` now calls `handle_sigint(SIGINT)` directly (installs `signal_handle` via `sigaction` immediately) instead of registering `handle_sigint` itself as the signal handler.
8. ~~`config_addr`'s IPv6 branch cast to `sockaddr_in`/`AF_INET`~~ — fixed to `sockaddr_in6`/`AF_INET6`.

Two latent bugs found and fixed while wiring the above up: `g_run` was declared `int` in `main.c` but referenced as `extern bool g_run` in `sig_handle.c` (type mismatch across translation units); `handle_sigint`'s `signal` parameter was unused, which fails under `-Wextra`.

## Missing pieces (not bugs, just not written yet)

- The `PING <host> (<ip>): <n> data bytes` startup line should print unconditionally, not only under `-v` (currently in `main_loop`, gated by `if (ping->verbose == true)`).
- End-of-run statistics summary (`--- host ping statistics ---`, packets transmitted/received, packet loss %, round-trip min/avg/max/mdev) — needs counters/RTT accumulation added to `t_ping`, printed on SIGINT/exit.

## Bonus part — not started

Per the subject, only assessed if the mandatory part is 100% functional. None implemented yet:

- [ ] `-f` (flood)
- [ ] `-l` (preload)
- [ ] `-n` (numeric output only)
- [ ] `-w` (deadline)
- [ ] `-W` (timeout)
- [ ] `-p` (pattern)
- [ ] `-r` (bypass routing)
- [ ] `-s` (packet size)
- [ ] `-T` / `--ttl` (set TTL)
- [ ] `--ip-timestamp`

(`-V`, `--usage`, `--echo` don't count as bonus flags; two flags for the same feature, e.g. `-t`/`--type`, count as one.)
