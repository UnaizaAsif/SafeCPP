const express = require("express");
const cors = require("cors");
const { execFile } = require("child_process");
const path = require("path");
const fs = require("fs");
const os = require("os");

const app = express();
app.use(cors());
app.use(express.json({ limit: "1mb" }));

const REPO_ROOT = path.resolve(__dirname, "..", "..");
const SAMPLES_DIR = path.join(REPO_ROOT, "samples");
const COMPILER_EXE = path.join(REPO_ROOT, "compiler_test5.exe");

function listSamples() {
  if (!fs.existsSync(SAMPLES_DIR)) return [];
  return fs
    .readdirSync(SAMPLES_DIR)
    .filter((f) => f.toLowerCase().endsWith(".cpp"))
    .sort((a, b) => a.localeCompare(b));
}

function parseCompilerOutput(stdout) {
  // Split by the explicit phase markers your compiler prints.
  const markers = [
    "[LEXICAL ANALYZER PHASE]",
    "[SYNTAX ANALYZER PHASE]",
    "[SEMANTIC ANALYZER PHASE]",
    "[ANALYSIS SUMMARY]",
  ];

  const sections = {
    raw: stdout,
    lexical: "",
    syntax: "",
    semantic: "",
    summary: "",
  };

  const idx = {};
  for (const m of markers) idx[m] = stdout.indexOf(m);

  // Helpers to slice between markers if present
  function sliceBetween(startMarker, endMarker) {
    const s = idx[startMarker];
    if (s === -1) return "";
    const start = s + startMarker.length;
    const e = endMarker ? idx[endMarker] : -1;
    if (e === -1) return stdout.slice(start).trim();
    return stdout.slice(start, e).trim();
  }

  sections.lexical = sliceBetween(markers[0], markers[1]);
  sections.syntax = sliceBetween(markers[1], markers[2]);
  sections.semantic = sliceBetween(markers[2], markers[3]);
  sections.summary = sliceBetween(markers[3], null);

  return sections;
}

app.get("/api/health", (_req, res) => {
  res.json({
    ok: true,
    repoRoot: REPO_ROOT,
    compilerExists: fs.existsSync(COMPILER_EXE),
    samplesCount: listSamples().length,
  });
});

app.get("/api/samples", (_req, res) => {
  res.json({ samples: listSamples() });
});

app.get("/api/sample/:name", (req, res) => {
  const safeName = path.basename(req.params.name || "");
  const samplePath = path.join(SAMPLES_DIR, safeName);
  if (!safeName || !fs.existsSync(samplePath)) {
    return res.status(404).json({ error: `Sample not found: ${safeName}` });
  }
  const code = fs.readFileSync(samplePath, "utf8");
  return res.json({ sample: safeName, code });
});

app.post("/api/analyze", (req, res) => {
  const { sample, code } = req.body || {};

  if ((!sample || typeof sample !== "string") && (!code || typeof code !== "string")) {
    return res.status(400).json({ error: "Provide either 'sample' or 'code'." });
  }

  if (!fs.existsSync(COMPILER_EXE)) {
    return res.status(500).json({
      error:
        "compiler_test2.exe not found in repo root. Build it first (see UI/README.md).",
    });
  }

  let inputPath = "";
  let label = "";
  let cleanupTemp = null;

  if (code && typeof code === "string") {
    const tempDir = fs.mkdtempSync(path.join(os.tmpdir(), "safecpp-ui-"));
    inputPath = path.join(tempDir, "input.cpp");
    fs.writeFileSync(inputPath, code, "utf8");
    label = "custom_input.cpp";
    cleanupTemp = () => fs.rmSync(tempDir, { recursive: true, force: true });
  } else {
    const safeName = path.basename(sample);
    inputPath = path.join(SAMPLES_DIR, safeName);
    if (!fs.existsSync(inputPath)) {
      return res.status(404).json({ error: `Sample not found: ${safeName}` });
    }
    label = safeName;
  }

  execFile(COMPILER_EXE, [inputPath], { cwd: REPO_ROOT, windowsHide: true, maxBuffer: 10 * 1024 * 1024 }, (err, stdout, stderr) => {
    const out = (stdout || "") + (stderr || "");
    if (cleanupTemp) cleanupTemp();
    if (err) {
      // still return output so UI can show what happened
      return res.json({
        ok: false,
        sample: label,
        exitCode: err.code ?? null,
        ...parseCompilerOutput(out),
      });
    }
    return res.json({
      ok: true,
      sample: label,
      exitCode: 0,
      ...parseCompilerOutput(out),
    });
  });
});

const PORT = process.env.PORT || 5174;
app.listen(PORT, () => {
  console.log(`SafeCPP UI backend running on http://localhost:${PORT}`);
  console.log(`Repo root: ${REPO_ROOT}`);
});

