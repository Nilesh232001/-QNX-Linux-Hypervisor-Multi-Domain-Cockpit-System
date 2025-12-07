# 🚗 QNX/Linux Hypervisor Multi-Domain Cockpit System
See `docs/` for architecture and protocol details.

This repository contains a simplified, prototype implementation intended to demonstrate
hypervisor-based virtualization concepts, inter-domain communication using shared memory,
and a minimal simulated cluster (QNX-like) and IVI (Linux-like) domains.

Directory layout:
- hypervisor-config/: example hypervisor configuration files
- qnx-domain/: simulated cluster application (C++)
- linux-domain/: simulated IVI application (C++)
- comm/: shared-memory protocol and ring-buffer implementation (C)
- compositor/: optional simple compositor (Python)
- tests/: latency & stress test scripts
- tools/: helper scripts (startup, simulate crash)
- docs/: architecture, protocol, safety overview
