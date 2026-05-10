Here’s a clean professional `README.md` you can directly paste into your GitHub repository.

````md
# mtrace — Mini Linux System Call Tracer

A lightweight educational Linux syscall tracer built in C using the `ptrace()` API and an `ncurses`-based Terminal User Interface (TUI).

`mtrace` allows users to trace running Linux processes, monitor system calls in real time, apply syscall category filters, and generate logs — all through an interactive terminal menu.

---

# Features

- Trace processes by PID
- Trace processes by process name
- Interactive ncurses TUI
- Category-based syscall filtering
- Real-time syscall tracing
- Timestamped syscall logging
- Non-blocking keyboard controls
- Clean process detach support
- Educational alternative to `strace`

---

# Technologies Used

- C Programming Language
- Linux `ptrace()` API
- ncurses
- termios
- GNU Make
- Linux system calls

---

# Supported Syscall Categories

- File I/O
- Process Management
- Memory Management
- IPC
- Signals
- Time
- Security
- Scheduler

---

# Project Structure

```text
mtrace/
│
├── main.c
├── TUI.c
├── Ptracer.c
├── filter.c
├── SysCalls.c
├── logs.c
│
├── include/
│   ├── TUI.h
│   ├── filter.h
│   ├── Ptracer.h
│   ├── SysCalls.h
│   └── logs.h
│
├── Makefile
├── README.md
└── mtrace.exe
````

---

# How It Works

1. User launches `mtrace.exe`
2. ncurses menu appears
3. User selects:

   * Trace by PID
   * Trace by process name
   * Apply syscall filters
4. `ptrace()` attaches to target process
5. Syscalls are intercepted
6. Matching syscalls are displayed/logged
7. User presses `q` to detach safely

---

# Compilation

## Requirements

Install required packages:

### Ubuntu / Debian

```bash
sudo apt update
sudo apt install build-essential libncurses5-dev
```

---

## Build Project

```bash
make
```

OR manually:

```bash
gcc main.c TUI.c filter.c Ptracer.c SysCalls.c logs.c -lncurses -o mtrace.exe
```

---

# Running the Tracer

## Run the executable

```bash
./mtrace.exe
```

The interactive terminal UI will open.

---

# Using mtrace

## Trace by PID

1. Select:

   ```text
   Trace by PID
   ```

2. Enter target PID

3. Tracing starts immediately

---

## Trace by Process Name

1. Select:

   ```text
   Trace by Process Name
   ```

2. Enter process name

Example:

```text
bash
```

The tracer automatically resolves the PID using `pgrep`.

---

## Apply Filters

Select:

```text
Filter Syscall Categories
```

Use:

* Arrow Keys → Navigate
* Space → Toggle category
* Enter → Confirm
* q → Cancel

---

# Controls

| Key   | Action         |
| ----- | -------------- |
| ↑ / ↓ | Navigate menus |
| Enter | Select option  |
| Space | Toggle filter  |
| q     | Quit / Detach  |
| ESC   | Cancel popup   |

---

# Example Output

```text
[12:01:14] open
[12:01:14] read
[12:01:15] write
[12:01:16] mmap
```

---

# Log Files

Tracing sessions generate log files automatically.

Example:

```text
trace_1234.log
```

Each log contains:

* Timestamp
* Syscall name
* Syscall activity sequence

---

# Operating System Concepts Demonstrated

* System Calls
* Process Tracing
* Signals
* IPC
* Scheduling
* Memory Management
* User vs Kernel Mode
* Terminal I/O
* Synchronization
* Process Control

---

# Known Limitations

* Does not yet decode syscall arguments
* High-frequency tracing may reduce performance
* Linux-only project

---

# Future Improvements

* Decode syscall arguments
* Capture syscall return values

---



---

# References

* ptrace Linux Manual
  [https://man7.org/linux/man-pages/man2/ptrace.2.html](https://man7.org/linux/man-pages/man2/ptrace.2.html)

* NCURSES HOWTO
  [https://profs.info.uaic.ro/sabin.buraga/teach/courses/net/docs/NCURSES.html#menu_basics](https://profs.info.uaic.ro/sabin.buraga/teach/courses/net/docs/NCURSES.html#menu_basics)

* Oracle Linux ptrace Article
  [https://blogs.oracle.com/linux/tracing-the-ptrace](https://blogs.oracle.com/linux/tracing-the-ptrace)

* OpenSourceForU ptrace Tutorial
  [https://www.opensourceforu.com/2023/09/tracing-processes-with-ptrace-system-call/](https://www.opensourceforu.com/2023/09/tracing-processes-with-ptrace-system-call/)

* StackOverflow ptrace Discussion
  [https://stackoverflow.com/questions/38676995/linux-is-there-a-way-to-use-ptrace-without-stopping-pausing-the-process-sigsto](https://stackoverflow.com/questions/38676995/linux-is-there-a-way-to-use-ptrace-without-stopping-pausing-the-process-sigsto)

---

# Authors

* Mushaf Ali Mir
* Muhammad Mujtaba Chohan
* Sherjeel Ahmed

---

# License

This project was developed for educational purposes as part of an Operating Systems course project.

```
```
