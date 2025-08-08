# Beckman999 Python Port

A cross‑platform Python re‑implementation of the Beckman 999 IC tester UI/logic you prototyped in Processing. This project aims to run on Windows/Linux/macOS with minimal setup, talk to your Arduino/mega hardware over serial, and provide both a CLI and an optional GUI.

---

## 1) Goals

* **Portable**: one codebase that runs on Windows, Linux, and macOS.
* **Deterministic**: repeatable tests with logged results.
* **Modular**: hardware I/O isolated from UI; test definitions are data‑driven.
* **Fast setup**: `pipx install` or `uv run` and go.
* **Packaged**: PyInstaller builds for workshop machines.

## 2) Hardware Assumptions

* Arduino Mega 2560 (or compatible) handling pin driving/sensing.
* Serial link at configurable baud (default 115200).
* Discrete networks (10k, 160Ω, Zeners, etc.) selectable in firmware.
* 16‑pin DIP baseline with pin‑map support for 14, 20, 24, 40.

> Adjust this section once the exact wiring and resistor banks are finalized.

## 3) Software Requirements

* Python 3.10+
* `pyserial` — serial comms
* `rich` — pretty CLI output/logging
* `typer` — CLI commands
* `pydantic` — validate config/test files
* `pyyaml` — optional YAML test defs
* `pyqtgraph` + `PySide6` (optional GUI)
* \`matplotlib
