*This project has been created as part of the 42 curriculum by kebertra.*

# Codexion

> Master the race for resources before the deadline masters you.

## 🚀 Build and run

```bash
git clone https://github.com/KeroBeros68/Codexion.git
cd Codexion/coders
make
./codexion 4 800 200 100 200 5 400 fifo
```

> The binary `codexion` is built inside `coders/`.

---

## 📋 Prerequisites

- 🐧 **Linux** (tested on Ubuntu 22.04)
- 🛠️ **make**
- 🔧 **C compiler** supporting C99 or later (gcc / clang)
- 🔍 **Valgrind / Helgrind** *(optional, for race-condition and memory checks)*

---

## 🗂️ Repository structure

```
Codexion/
├── README.md          ← this file
├── LICENSE
├── .gitignore
└── coders/            ← all source files; binary is built here
    ├── Makefile
    ├── main.c
    ├── coders.h
    ├── init.c / mem.c / sim.c
    ├── routine.c
    ├── dongles.c
    ├── monitoring.c
    ├── heap.c
    ├── logs.c
    ├── getter.c / setter.c
    ├── parsing.c
    ├── error.c
    └── utils.c
```

---

## 📖 Description

**Codexion** is a concurrent programming project implementing a variation of the classic **Dining Philosophers** problem, adapted to a coding environment.

N coders sit in a circle around a shared Quantum Compiler. Each coder alternates between three activities: **compiling**, **debugging**, and **refactoring**. Compiling requires holding two USB dongles simultaneously (left and right). There are as many dongles as coders, one between each adjacent pair.

A coder **burns out** if they fail to start a new compilation within `time_to_burnout` milliseconds since their last compile (or the start of the simulation). The goal is to keep every coder alive long enough for all of them to complete the required number of compilations.

Each coder runs as an independent **POSIX thread**. A dedicated **monitor thread** polls the simulation state every 100 µs and detects burnout within the 10 ms precision constraint imposed by the subject.

---

## ⚙️ CLI arguments

```
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
           time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

> ⚠️ All time values are in **milliseconds** and must be **strictly positive** integers. `coder_id` values start at **1**.

| Argument | Description |
|---|---|
| `number_of_coders` | Number of coder threads and dongles |
| `time_to_burnout` | Max ms a coder can go without starting a compile |
| `time_to_compile` | ms spent compiling (holding two dongles) |
| `time_to_debug` | ms spent debugging |
| `time_to_refactor` | ms spent refactoring |
| `number_of_compiles_required` | Target compilations per coder for normal termination |
| `dongle_cooldown` | ms a dongle is unavailable after being released |
| `scheduler` | Arbitration policy: `fifo` or `edf` |

### 🗓️ Scheduling policies

- **`fifo`** — First In, First Out: the dongle is granted to the earliest requester.
- **`edf`** — Earliest Deadline First: the dongle is granted to the coder whose deadline (`last_compile_start + time_to_burnout`) is closest.

---

## 📜 Output / log format

Each event is printed as:

```
timestamp_ms  coder_id  event
```

Example output:

```
0     1  has taken a dongle
1     1  has taken a dongle
1     1  is compiling
401   1  is debugging
501   1  is refactoring
701   2  has taken a dongle
701   2  has taken a dongle
701   2  is compiling
1101  2  is debugging
1201  2  is refactoring
```

Events (color-coded in terminal):
- 🔵 `has taken a dongle`
- 🟢 `is compiling`
- 🟡 `is debugging`
- 🟡 `is refactoring`
- 🔴 `burned out`

---

## 🛑 Termination conditions

The simulation stops when **either** of the following occurs:

- ✅ **Normal termination**: every coder has completed `number_of_compiles_required` compilations. The monitor detects this and sets `stop_sim = true`.
- 💀 **Burnout**: a coder has not started a compilation within `time_to_burnout` ms. The monitor logs the `burned out` event and immediately broadcasts on all dongle condition variables to unblock waiting threads.

---

## 🔧 Detailed build instructions

### Compilation

```bash
cd Codexion/coders
make
```

Compiled with `-Wall -Wextra -Werror -pthread`.

### Run

```bash
./codexion 4 800 200 100 200 5 400 fifo
./codexion 4 800 200 100 200 5 400 edf
```

### Makefile targets

| Target | Description |
|---|---|
| `make` / `make all` | Build the binary |
| `make clean` | Remove object files |
| `make fclean` | Remove object files and binary |
| `make re` | `fclean` + `all` |

---

## 🔒 Blocking cases handled

### 🚫 Deadlock prevention
The classic circular-wait deadlock scenario (each coder holds one dongle and waits for the other) is prevented by the **scheduler + heap** mechanism: each dongle maintains a priority queue of waiting coders. A coder is only granted a dongle when they are at the **head of the queue** (lowest key), which enforces a global ordering on resource acquisition and breaks the circular-wait condition.

### ⏳ Starvation prevention (FIFO)
Under FIFO scheduling, requests are served in strict arrival order. Since every coder enqueues with a monotonically increasing timestamp, no coder can be indefinitely bypassed — each request will eventually reach the head of the queue.

### 📅 Starvation prevention (EDF)
Under EDF scheduling, the coder with the closest deadline is always served first. `acquire_dongles` computes the scheduler key **once** and reuses it for both dongle takes in the same compile cycle, preventing priority inversion between the left and right acquisitions.

### ❄️ Cooldown handling
After a dongle is released, it is unavailable for `dongle_cooldown` ms. The waiting loop in `take_dongle` uses `pthread_cond_timedwait` when a cooldown is active: the thread auto-wakes exactly at `time_end_cooldown` without busy-waiting, then re-evaluates the condition.

### ⏱️ Burnout detection precision
The monitor thread polls every 100 µs (`usleep(100)`) and logs burnout within 10 ms of the actual deadline. After detecting a burnout, it sets `stop_sim = true` and immediately calls `wake_all_dongles()`, which broadcasts on every dongle's condition variable so threads blocked in `take_dongle` wake up instantly instead of waiting for the next `release_dongle`.

### 📝 Log serialization
All output is protected by a single `log_mutex`. Every call to `log_message` acquires the mutex before writing to `stdout`, ensuring that concurrent coder threads never produce interleaved lines.

---

## ⚠️ Edge cases

### 🔥 Coder burnout under tight timing
If the parameters do not satisfy the liveness condition below, a coder **will always burn out** — it is not a bug but infeasible parameters:

```
time_to_burnout > time_to_compile + dongle_cooldown + time_to_debug + time_to_refactor
```

Example: `./codexion 3 800 400 100 100 5 300 fifo` → burnout guaranteed (800 < 900).

### 🧍 N = 1 — single coder
With only one coder, `left_dongle == right_dongle`. Acquiring both would deadlock on the same mutex. This is handled explicitly in `acquire_dongles`: if `nb_coders == 1`, the coder spins on `stop_sim` until the monitor detects the burnout, then exits. The single coder **always burns out**.

### 🤝 EDF ties — identical deadlines at startup
When all coders start simultaneously, their initial deadlines are identical. EDF falls back to heap insertion order (which reflects `coder_id`) to break ties, so coder 1 is served first. This is deterministic for the very first round but may produce uneven first-round latencies.

### 🌩️ OS jitter causing spurious burnouts
Under high system load, the OS may delay thread wakeups by several milliseconds. A coder that should safely compile before its deadline may miss it solely due to scheduler jitter. Use `time_to_burnout` values well above `time_to_compile * 2` for reliable runs.

### 🔄 Dongle release order affects throughput
Because each dongle has an independent condition variable and waitlist, the order in which two adjacent coders release their shared dongle can cause one coder to gain a throughput advantage over the other in a given cycle. Over many cycles this effect averages out under FIFO; under EDF it is rebalanced by deadline proximity.

### 🎲 EDF strict-tie non-determinism across runs
If two coders reach exactly the same deadline value (e.g., after synchronized compile cycles), the heap's tie-breaking is based on insertion order, which can vary between runs due to OS thread scheduling. Consecutive runs with identical parameters may therefore produce different coder orderings when deadlines collide.

### Summary table

| Edge case | Scheduler | Observable effect | Mitigation |
|---|---|---|---|
| Infeasible params (burnout > compile+cooldown+debug+refacto) | both | Guaranteed burnout | Respect liveness formula |
| N=1 single coder | both | Always burns out | Handled: spins until monitor fires |
| Identical initial deadlines | EDF | Coder 1 always first in round 1 | Expected; stabilises after round 1 |
| OS jitter | both | Spurious burnout under load | Use large `time_to_burnout` margin |
| Release order throughput skew | both | One coder compiles more per cycle | Averages out over time |
| Strict-tie non-determinism | EDF | Different ordering across runs | Non-critical; heap insertion order |

---

## 🧵 Thread synchronization mechanisms

### `pthread_mutex_t` usage

| Mutex | Protects |
|---|---|
| `sim.log_mutex` | All `printf` output — prevents line interleaving |
| `sim.sim_mutex` | `sim.stop_sim` flag — read/written by monitor and coder threads |
| `sim.coder_finish_mutex` | `sim.coders_finish` counter — incremented when a coder completes all compilations |
| `coder.cond_dead` | `coder.deadline` — written by the coder, read by the monitor |
| `coder.cond_nb_comp` | `coder.nb_compile` — written by the coder, read by the monitor |
| `dongle.lock` | `dongle.waitlist` (heap), `dongle.in_use` and `dongle.time_end_cooldown` |

Every shared field is accessed exclusively through its getter/setter pair, which wraps a `pthread_mutex_lock / pthread_mutex_unlock` around the read or write. This ensures no race condition can occur on any individual field.

### `pthread_cond_t` usage

Each dongle has its own condition variable (`dongle.cond`), paired with `dongle.lock`.

**Waiting** (`take_dongle`):
```c
// Coder enqueues itself in the heap then waits until:
// 1. It is at the head of the waitlist (its key is the smallest)
// 2. The dongle is not currently in use
// 3. The cooldown has expired
while (heap_peek(...).coder_id != coder->id
    || dongle->in_use
    || get_timestamp() < dongle->time_end_cooldown)
{
    if (dongle->in_use || no cooldown active)
        pthread_cond_wait(&dongle->cond, &dongle->lock);
    else
        pthread_cond_timedwait(&dongle->cond, &dongle->lock, &ts);
}
```

**Signaling** (`release_dongle`):  
The releasing coder holds `dongle->lock`, writes `time_end_cooldown`, then calls `pthread_cond_broadcast`. Broadcasting under the lock prevents any waiter from missing the signal between reading the condition and calling `cond_wait`.

**Emergency wakeup** (`wake_all_dongles`):  
When `stop_sim` is set to `true`, the monitor broadcasts on every dongle immediately. All threads blocked in `take_dongle` wake up, re-check `stop_sim`, and exit without waiting for their next natural wakeup.

### Race condition prevention — concrete example

Without the fix, `release_dongle` called `pthread_cond_broadcast` **outside** the lock:

```c
// BROKEN — broadcaster not holding the lock
dongle->time_end_cooldown = ...;
pthread_mutex_unlock(&dongle->lock);
pthread_cond_broadcast(&dongle->cond); // waiter can miss this
```

A waiting thread could wake from `cond_wait`, re-acquire the lock, find the condition still false, call `cond_wait` again, and then the broadcast fires with nobody waiting — permanent sleep. Fixed by broadcasting **before** unlocking:

```c
// CORRECT — broadcast under lock
pthread_mutex_lock(&dongle->lock);
dongle->time_end_cooldown = ...;
pthread_cond_broadcast(&dongle->cond);
pthread_mutex_unlock(&dongle->lock);
```

Helgrind reports **0 data race errors** on all tested configurations.

---

## 📚 Resources

### 🧵 Threading & synchronization
- [Pthreads — tala-informatique](https://www.tala-informatique.fr/wiki/index.php/Pthread)
- [`pthread_cond_wait(3)` — Linux man page](https://man7.org/linux/man-pages/man3/pthread_cond_wait.3p.html)
- [`pthread_cond_timedwait(3)` — Linux man page](https://man7.org/linux/man-pages/man3/pthread_cond_timedwait.3p.html)
- [`pthread_cond_signal(3)` — Linux man page](https://man7.org/linux/man-pages/man3/pthread_cond_signal.3p.html)
- [`pthread_cond_broadcast(3)` — Linux man page](https://man7.org/linux/man-pages/man3/pthread_cond_broadcast.3p.html)

### 🔀 Concurrency theory
- [Dining Philosophers — Wikipedia](https://en.wikipedia.org/wiki/Dining_philosophers_problem)
- [Earliest Deadline First scheduling — Wikipedia](https://en.wikipedia.org/wiki/Earliest_deadline_first_scheduling)

### 🗃️ Data structures
- [Binary Heaps — YouTube (CS Dojo)](https://www.youtube.com/watch?v=t0Cq6tVNRBA)

---

## 🤖 AI usage

**GitHub Copilot** (Claude Sonnet 4.5) was used throughout this project for:
- 🐛 **Debugging**: identifying the race condition in `release_dongle` (broadcast outside lock) and the missing `wake_all_dongles` wakeup on simulation stop
- 🔍 **Code review**: dead code audit (unused fields, prototypes, includes)
- 📝 **Documentation**: writing and updating docstrings and this README
- ♻️ **Refactoring suggestions**: extracting named `#define` constants for log strings and ANSI color codes

**Google Gemini** was additionally used for:
- 💡 **Complementary explanations**: clarifying POSIX condition variable semantics and memory ordering guarantees
- 🧪 **Challenging and validating design choices**: questioning the single-key approach in `acquire_dongles` and confirming that reusing the same scheduler key for both dongle acquisitions correctly prevents priority inversion

All AI-generated suggestions were **reviewed, tested with Valgrind/Helgrind, and cross-checked** against the POSIX specification before integration.
