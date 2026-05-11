import { useEffect, useMemo, useState } from 'react'
import './App.css'

type AnalyzeResponse = {
  ok: boolean
  sample: string
  exitCode: number | null
  raw: string
  lexical: string
  syntax: string
  semantic: string
  summary: string
}

function App() {
  const [samples, setSamples] = useState<string[]>([])
  const [selected, setSelected] = useState<string>('')
  const [mode, setMode] = useState<'sample' | 'custom'>('sample')
  const [code, setCode] = useState('')
  const [loading, setLoading] = useState(false)
  const [err, setErr] = useState<string | null>(null)
  const [result, setResult] = useState<AnalyzeResponse | null>(null)

  useEffect(() => {
    ;(async () => {
      try {
        const r = await fetch('/api/samples')
        const data = (await r.json()) as { samples: string[] }
        setSamples(data.samples || [])
        if ((data.samples || []).length > 0) {
          const first = data.samples[0]!
          setSelected(first)
          const rc = await fetch(`/api/sample/${encodeURIComponent(first)}`)
          const dc = (await rc.json()) as { code: string }
          setCode(dc.code || '')
        }
      } catch (e) {
        setErr('Failed to load samples. Is the backend running?')
      }
    })()
  }, [])

  async function onSampleChange(next: string) {
    setSelected(next)
    try {
      const rc = await fetch(`/api/sample/${encodeURIComponent(next)}`)
      const dc = (await rc.json()) as { code: string }
      setCode(dc.code || '')
    } catch {
      setCode('')
    }
  }

  const canRun = useMemo(
    () => !loading && ((mode === 'sample' && !!selected) || (mode === 'custom' && code.trim().length > 0)),
    [selected, loading, mode, code],
  )

  async function run() {
    if (mode === 'sample' && !selected) return
    if (mode === 'custom' && !code.trim()) return
    setLoading(true)
    setErr(null)
    setResult(null)
    try {
      const r = await fetch('/api/analyze', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(mode === 'sample' ? { sample: selected } : { code }),
      })
      const data = (await r.json()) as AnalyzeResponse & { error?: string }
      if (!r.ok) {
        throw new Error(data.error || 'Analyze request failed')
      }
      if ('error' in data && data.error) {
        throw new Error(data.error)
      }
      setResult(data)
    } catch (e) {
      setErr(e instanceof Error ? e.message : 'Unknown error')
    } finally {
      setLoading(false)
    }
  }

  return (
    <div className="page">
      <header className="topbar">
        <div className="brand">
          <div className="title">SafeCPP Visualizer</div>
          <div className="subtitle">Code • Lexical • Syntax • Semantic</div>
        </div>

        <div className="controls">
          <select value={mode} onChange={(e) => setMode(e.target.value as 'sample' | 'custom')} disabled={loading}>
            <option value="sample">Sample File</option>
            <option value="custom">Custom Code</option>
          </select>

          <select
            value={selected}
            onChange={(e) => onSampleChange(e.target.value)}
            disabled={loading || mode !== 'sample'}
          >
            {samples.map((s) => (
              <option key={s} value={s}>
                {s}
              </option>
            ))}
          </select>
          <button onClick={run} disabled={!canRun}>
            {loading ? 'Running…' : 'Run Analysis'}
          </button>
        </div>
      </header>

      {err ? <div className="alert">{err}</div> : null}

      <main className="grid">
        <section className="panel">
          <div className="panelHeader">
            <div>CODE</div>
          </div>
          <textarea
            className="codeEditor panelBody"
            value={code}
            onChange={(e) => setCode(e.target.value)}
            readOnly={mode === 'sample'}
            spellCheck={false}
          />
        </section>

        <section className="panel">
          <div className="panelHeader">
            <div>LEXICAL TOKENS</div>
          </div>
          <pre className="panelBody">{result?.lexical || '—'}</pre>
        </section>

        <section className="panel">
          <div className="panelHeader">
            <div>SYNTAX ANALYSIS</div>
          </div>
          <pre className="panelBody">{result?.syntax || '—'}</pre>
        </section>

        <section className="panel">
          <div className="panelHeader">
            <div>SEMANTIC ANALYSIS</div>
          </div>
          <pre className="panelBody">
            {(result?.semantic || '—') + (result?.summary ? `\n\n[ANALYSIS SUMMARY]\n${result.summary}` : '')}
          </pre>
        </section>
      </main>
    </div>
  )
}

export default App
