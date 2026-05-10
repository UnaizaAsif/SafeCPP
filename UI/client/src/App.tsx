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
  const [loading, setLoading] = useState(false)
  const [err, setErr] = useState<string | null>(null)
  const [result, setResult] = useState<AnalyzeResponse | null>(null)

  useEffect(() => {
    ;(async () => {
      try {
        const r = await fetch('/api/samples')
        const data = (await r.json()) as { samples: string[] }
        setSamples(data.samples || [])
        if ((data.samples || []).length > 0) setSelected(data.samples[0]!)
      } catch (e) {
        setErr('Failed to load samples. Is the backend running?')
      }
    })()
  }, [])

  const canRun = useMemo(() => !!selected && !loading, [selected, loading])

  async function run() {
    if (!selected) return
    setLoading(true)
    setErr(null)
    setResult(null)
    try {
      const r = await fetch('/api/analyze', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ sample: selected }),
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
          <div className="subtitle">Lexical • Syntax • Semantic</div>
        </div>

        <div className="controls">
          <select value={selected} onChange={(e) => setSelected(e.target.value)} disabled={loading}>
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
          <pre className="panelBody">{result?.semantic || '—'}</pre>
        </section>

        <section className="panel">
          <div className="panelHeader">
            <div>SUMMARY</div>
          </div>
          <pre className="panelBody">{result?.summary || '—'}</pre>
        </section>
      </main>
    </div>
  )
}

export default App
