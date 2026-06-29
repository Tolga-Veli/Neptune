# ♆ Neptune

*A modern chess game and future chess engine written in C++23.*

Neptune is a personal project that started as a graphical chess game and is gradually evolving into a complete chess engine built from scratch.

---

## Features

### Current

- ♟️ Playable chess game
- ✅ Legal move generation
- ♖  Castling
- 🔄 Pawn promotion
- 📖 FEN import
- 👆 Piece selection with legal move highlighting
- 🎨 Graphical interface built with SFML and Dear ImGui

### Planned

- ⚡ Alpha-beta search with iterative deepening
- 🪄 Magic bitboards
- ♻️ Zobrist hashing
- 📚 Transposition tables
- 🎯 Move ordering heuristics
- ⏱️ Time management
- 🤖 UCI protocol support
- 📈 Evaluation function

---

## Project Structure

```text
src/
├── Core/          # Board representation and game state
├── Engine/        # Move generation and (future) search
├── Rendering/     # SFML rendering and GUI
├── Assets/        # Piece textures and resources
└── main.cpp       # Entry point
```

---

## Building

### Requirements

- C++23 compatible compiler
- CMake
- SFML 3
- Dear ImGui
- ImGui-SFML

Clone the repository:

```bash
git clone --recursive https://github.com/Tolga-Veli/Neptune.git
cd Neptune
```

Configure and build:

```bash
cmake -B build
cmake --build build
```

Run:

```bash
cd build
./Neptune
```

---

## Roadmap

- [x] Board representation
- [x] Legal move generation
- [x] Castling
- [x] Pawn promotion
- [x] Interactive GUI
- [x] Checkmate / stalemate detection
- [x] FEN import
- [ ] Testing framework
- [ ] Alpha-beta search
- [ ] Quiescence search
- [ ] Move ordering
- [ ] Transposition tables
- [ ] UCI support
- [ ] Opening book
- [ ] Endgame tablebases

---

## Why "Neptune"?

Neptune is named after the Roman god of the sea. Chess programming has a surprising amount of depth, and this project is my way of exploring it one component at a time.

---

## License

This project is licensed under the MIT License.
