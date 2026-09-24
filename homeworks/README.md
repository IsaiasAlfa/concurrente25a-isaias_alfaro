# Heat Transfer Simulation — Serial to Distributed

A thermal diffusion simulator taken through four implementations, from a single-threaded baseline to a hybrid OpenMP + MPI version. Built for the Parallel and Concurrent Programming course at the University of Costa Rica (first semester, 2025).

## The problem

Simulate how heat spreads across a rectangular metal plate until it reaches thermal equilibrium. Each cell's temperature is recomputed from its four neighbors on every time step:

$$T^{k+1}_{i,j} = T^k_{i,j} + \frac{\Delta t \cdot \alpha}{h^2}\left(T^k_{i-1,j} + T^k_{i,j+1} + T^k_{i+1,j} + T^k_{i,j-1} - 4T^k_{i,j}\right)$$

The simulation stops when the largest temperature change across the whole plate drops below a threshold ε. Plates are large, and the stopping point is data-dependent, so runtimes vary from seconds to tens of minutes depending on the input.

## Results

Measured on the same plate and machine across all versions.

### Serial optimization

| Version | Runtime | Speedup | Change |
|---|---:|---:|---|
| Serial (initial) | 78.6 s | 1.00× | Baseline |
| Serial (optimized) | **58.5 s** | **1.34×** | Flat linear arrays instead of 2D matrix indexing |

Callgrind profiling showed the cost was concentrated in repeated file handling and matrix traversal. Replacing the 2D indexing with two flat arrays improved cache locality and cut about a quarter of the runtime before any concurrency was introduced.

### Concurrency

| Version | Runtime | vs. optimized serial | Change |
|---|---:|---:|---|
| Pthreads (naive) | 1387.7 s | 0.04× | Threads created and destroyed every iteration |
| \+ Semaphore barrier | 1250.0 s | 0.05× | Barrier replaces join; threads stay alive |
| \+ Dynamic mapping | 1193.0 s | 0.05× | Work assigned at runtime instead of statically |
| Pthreads (final) | **56.1 s** | **1.04×** | Persistent thread team, reusable barrier |
| OpenMP | 898.7 s | 0.07× | `schedule(runtime)`, same decomposition |

**The interesting result is the failure, not the win.** The first concurrent version ran 24× *slower* than the serial code it was meant to speed up. The plate is recomputed thousands of times, and spawning and joining a thread team on every one of those iterations cost far more than the parallel work saved. Fixing it meant keeping a single thread team alive for the whole run and synchronizing it with a reusable barrier built on semaphores.

Even after that, the final version only edges past the optimized serial. On this workload, the per-iteration synchronization and the memory-bound access pattern leave little room for parallel gain — which is itself the useful finding.

## Layout

| Directory | What it contains |
|---|---|
| `serial/` | Single-threaded baseline and its optimized form |
| `pthread/` | First concurrent version using POSIX threads |
| `optimized/` | Persistent thread team with a semaphore-based reusable barrier |
| `omp_mpi/` | Hybrid version: OpenMP within a process, MPI across processes |

Each directory holds `src/`, a `design/` folder with pseudocode and UML, and a `Makefile`. `optimized/` and `omp_mpi/` also include a `report/` with the profiling analysis behind the numbers above.

## Building and running

```bash
cd omp_mpi
make release
```

The job file lists one simulation per line:

```
plate001.bin  1200  127  1000  2
```

The columns are the binary plate file, the time step Δt, the thermal diffusivity α, the cell size h, and the equilibrium threshold ε.

Run with a job file and a thread count:

```bash
# Serial, pthread, optimized
./bin/optimized tests/job001.txt 8

# omp_mpi — 4 processes, 8 threads each
mpiexec -np 4 ./bin/omp_mpi tests/job001.txt 8
```

Each `Makefile` already defines its own arguments and MPI prefix, so `make run` works as a shortcut.

Output is written to `out/` as a report per job plus the resulting plate states.

## Implementation notes

**Synchronization.** The reusable barrier is built from counting semaphores rather than `pthread_barrier_t`, so the arrival count and the release are visible and debuggable. Every thread increments a shared counter under mutual exclusion; the last one to arrive resets it and releases the rest.

**Decomposition.** The plate is split by rows. Rows outnumber threads in every realistic input, so a block mapping keeps each thread's slice contiguous and cache-friendly. The dynamic mapping variant was tried and kept for comparison, but it did not pay for its overhead here.

**MPI layer.** Jobs, not plate regions, are distributed across processes. Each process runs the full OpenMP solver on its assigned jobs and reports back through point-to-point `Send`/`Recv`, with a `Barrier` before the final report is assembled. Splitting a single plate across processes would require halo exchange on every iteration, which this workload cannot amortize.

---

*Isaías Alfaro Ugalde — isaias.alfaro@ucr.ac.cr*
