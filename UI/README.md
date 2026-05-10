# SafeCPP UI (React + Express)

This folder contains a small UI to visualize your compiler phases:

- Lexical analyzer output (tokens)
- Syntax analyzer output (errors + summary)
- Semantic analyzer output (your existing systems output)

## Prerequisites

- Node.js + npm installed
- `compiler_test2.exe` built in the repository root

Build your compiler (from repo root):

```powershell
g++ -std=c++17 -Wall -Wextra -I. -o compiler_test2.exe main.cpp lexer/Token.cpp lexer/Lexer.cpp parser/Parser.cpp utils/SymbolTable.cpp semantic/SemanticAnalyzer.cpp analyzer/IncludeDependencyAnalyzer.cpp
```

## Install dependencies

```powershell
cd UI\server
npm install

cd ..\client
npm install
```

## Run

Terminal 1:

```powershell
cd UI\server
npm run dev
```

Terminal 2:

```powershell
cd UI\client
npm run dev
```

Open the URL printed by Vite (usually `http://localhost:5173`).

