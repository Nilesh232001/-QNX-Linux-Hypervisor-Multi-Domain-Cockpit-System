# Architecture Overview

This document describes the simplified architecture used in this demo:
- A host supervisor (simulated hypervisor) launches two user-space processes acting as domains.
- Shared memory is used to exchange messages (ring buffer + condition variable).
- The compositor (optional) merges framebuffers for display.

See README.md for quickstart.
