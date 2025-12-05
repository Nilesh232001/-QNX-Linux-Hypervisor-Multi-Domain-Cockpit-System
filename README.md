# 🚗 QNX/Linux Hypervisor Multi-Domain Cockpit System  
*A Virtualized Automotive Digital Cockpit Architecture (QNX Cluster + Linux IVI)*

## 📌 Overview  
This project demonstrates a **virtualized automotive digital cockpit system** running **QNX** (safety-critical cluster) and **Linux** (infotainment IVI) together on a **single System-on-Chip (SoC)** using the **QNX Hypervisor**.  

The goal is to show how modern vehicles consolidate multiple ECUs into one hardware platform while maintaining **safety, isolation, and real-time guarantees** for critical functionality like the instrument cluster.

This is a **prototype**, but it follows the same architectural principles used in production automotive systems by OEMs and Tier-1 suppliers.

---

## 🎯 Key Features
- **QNX Domain (Cluster)**  
  - Displays speed, RPM, warnings  
  - Real-time scheduling guarantees  
  - Independent execution even when Linux domain is overloaded  

- **Linux Domain (IVI)**  
  - Mock navigation + media applications  
  - Sends non-critical data to QNX (directions, metadata)  

- **QNX Hypervisor Isolation**
  - Dedicated CPU cores & memory regions for each domain  
  - Device partitioning (GPU slices, CAN, display controller)  
  - Prevents Linux failures from affecting real-time cluster behavior  

- **Cross-Domain Communication Layer**
  - Shared memory ring buffer  
  - Doorbell notification (interrupt/event)  
  - Custom lightweight message protocol  

- **Safety & Recovery**
  - QNX cluster runs safely even if Linux IVI crashes  
  - Watchdog monitoring & fallback behavior  

---

## 🧩 Architecture Diagram

```
                +---------------------------------------------+
                |               QNX Hypervisor                |
                |   CPU/Memory/GPU/Device Partitioning        |
                +-----------------------+----------------------+
                                        |
           -----------------------------+-----------------------------
           |                                                           |
+------------------------+                           +------------------------+
|   QNX Domain (Cluster) |                           |   Linux Domain (IVI)   |
|------------------------|                           |------------------------|
| - Real-time tasks      |                           | - Navigation mock      |
| - Speed/RPM display    | <--- Shared Memory -----> | - Media info sender    |
| - Warning system       | <--- Doorbell IRQ ------> | - UI applications      |
| - VirtIO driver (opt.) |                           | - VirtIO driver (opt.) |
+------------------------+                           +------------------------+
           |                                                           |
           -----------------------------+-----------------------------
                                        |
                     +----------------------------------+
                     |   Physical Display / Compositor   |
                     +----------------------------------+
```

---

## 🏗️ Project Structure

```
hypervisor-cockpit/
├── hypervisor-config/         # QNX Hypervisor domain configuration
├── qnx-domain/                # Cluster app (C/C++) + domain initialization
│   ├── src/
│   └── include/
├── linux-domain/              # IVI mock apps + shared mem utilities
│   ├── src/
│   └── include/
├── comm/                      # Cross-domain communication (SHM + doorbell)
│   ├── shm_protocol.h
│   ├── shm_ringbuffer.c
│   └── virtio_driver.c (optional)
├── compositor/                # Optional display compositor
├── tests/                     # Latency & stress tests
├── tools/                     # Setup scripts & CAN simulation
├── docs/
│   ├── architecture.md
│   ├── protocol.md
│   └── safety_overview.md
└── README.md
```

---

## ⚙️ How It Works (Simplified)

### 1️⃣ Hypervisor Setup  
QNX Hypervisor divides CPU, memory, and devices across domains.
This ensures QNX always has predictable performance.
### 2️⃣ Inter-Domain Communication  
-A shared memory region is mapped into both domains.
  -A ring buffer passes messages like:
    -Speed updates
    -Navigation arrows
    -Media metadata
    -Health/heartbeat signals
    -A doorbell interrupt notifies the receiver.

### 3️⃣ QNX Domain  
Real-time cluster simulation.
    Runs real-time tasks:
    Reads simulated CAN data
    Updates speed/RPM
    Displays warnings
    Processes navigation prompts from Linux
    Even under IVI stress load, the cluster maintains real-time deadlines.

### 4️⃣ Linux Domain  
Mock IVI apps sending metadata/navigation.
    Runs mock apps:
    Navigation (turn-left, turn-right updates)
    Media info (song name, artist)
    Sends non-critical messages to cluster.
### 5️⃣ Safety Behavior  
Cluster continues running even if IVI crashes.
    If Linux crashes or becomes unresponsive:
    QNX continues running normally
    Comms layer detects timeout
    Cluster switches to fallback mode
---

## 🚀 Getting Started

### Prerequisites
-QNX SDP + QNX Hypervisor
-Board supporting QNX Hypervisor (e.g., NXP i.MX8, Renesas R-Car)
-Linux cross-toolchain (Yocto or GCC)
-CMake / QNX Momentics 

### Build Steps
```
# QNX domain
qcc -o cluster_app src/*.c

# Linux domain
cmake .. && make
```

---

## 🧪 Testing

Run latency and stress tests.

---

## 📊 Results (Sample)
| Test | Result |
|------|--------|
| Inter-domain latency | **2.8 ms avg** |
| Linux crash recovery | **Cluster unaffected** |

---

## 📄 Documentation
Located in `docs/`.

---

## 🧑‍💻 Author  
Nilesh Patil

---

## 📝 License  
MIT License
