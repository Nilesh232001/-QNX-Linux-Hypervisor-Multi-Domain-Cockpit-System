# Safety Overview (Demo)

This demo demonstrates isolation concepts and basic watchdog behavior:
- Cluster (simulated QNX) runs as a separate process and does not rely on IVI for critical functionality.
- IVI (simulated Linux) provides non-critical metadata.
- The supervisor can kill/restart the IVI to show cluster resilience.

This is a prototype and not suitable for real automotive use without extensive verification.
