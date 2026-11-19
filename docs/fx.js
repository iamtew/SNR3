// PixiJS backdrop: snareScene, full-viewport, no CDN.
const C = { rust: 0x8a3a22, brass: 0xc4a35a, cream: 0xe8dcc8 };
const BPM = 108;
const STEP = 60 / BPM / 4;
const FLAM = 0.035;
const BAR = 16 * STEP;
// a of 1, flam+accent on 2, a of 2, a of 3, accent on 4.
const GROOVE = [
  { step: 3, vel: 0.22 },
  { step: 4, vel: 1, flam: FLAM },
  { step: 7, vel: 0.3 },
  { step: 11, vel: 0.26 },
  { step: 12, vel: 1 },
];
const EVENTS = [];
for (const g of GROOVE) {
  if (g.flam) EVENTS.push({ at: g.step * STEP - g.flam, vel: 0.42, grace: true });
  EVENTS.push({ at: g.step * STEP, vel: g.vel, grace: false });
}
// Open just before the first ghost so the pocket starts with the page.
const START = 3 * STEP - 0.08;
const reduceMotion = matchMedia('(prefers-reduced-motion: reduce)').matches;
const rand = (lo, hi) => lo + Math.random() * (hi - lo);

// ?fx / ?fx=true keeps only the canvas. ?fx=false / 0 / off stays a normal page.
const fxRaw = new URLSearchParams(location.search).get('fx');
const fxOnly = fxRaw !== null && !/^(0|false|off)$/i.test(fxRaw);
if (fxOnly) document.body.classList.add('fx-only');

const pointer = { x: 0.5, y: 0.5 };
if (!fxOnly) {
  addEventListener('pointermove', (e) => {
    pointer.x = e.clientX / innerWidth;
    pointer.y = e.clientY / innerHeight;
  }, { passive: true });
}

start().catch((err) => {
  console.info('fx: animated backdrop disabled:', err?.message || err);
  document.getElementById('fx')?.remove();
});

async function start() {
  const PIXI = await import('./vendor/pixi.min.mjs');
  const app = new PIXI.Application();
  await app.init({
    canvas: document.getElementById('fx'),
    resizeTo: window,
    backgroundAlpha: 0,
    antialias: true,
    autoDensity: true,
    resolution: Math.min(devicePixelRatio || 1, 2),
    preference: 'webgl',
  });

  const glow = glowTexture(PIXI);
  const scene = snareScene(PIXI, glow);
  app.stage.addChild(scene.root);
  bootTicker(app, scene);
}

function bootTicker(app, scene) {
  if (reduceMotion) {
    app.ticker.stop();
    const still = () => {
      const { width: w, height: h } = app.screen;
      scene.update(0, w, h, 0);
      app.render();
    };
    addEventListener('resize', () => requestAnimationFrame(still));
    still();
    return;
  }

  let t = START;
  app.ticker.add((ticker) => {
    const dt = Math.min(ticker.deltaMS / 1000, 0.05);
    t += dt;
    const { width: w, height: h } = app.screen;
    // Preview mode is full-bleed, so there is no page to parallax against.
    if (!fxOnly) {
      const ease = Math.min(1, dt * 2.5);
      scene.root.x += ((0.5 - pointer.x) * 40 - scene.root.x) * ease;
      scene.root.y += ((0.5 - pointer.y) * 24 - scene.root.y) * ease;
    }
    scene.update(t, w, h, dt || 0.001);
  });

  document.addEventListener('visibilitychange', () => {
    if (document.hidden) app.ticker.stop(); else app.ticker.start();
  });
}

function glowTexture(PIXI, size = 64) {
  const c = document.createElement('canvas');
  c.width = c.height = size;
  const g = c.getContext('2d');
  const r = size / 2;
  const grd = g.createRadialGradient(r, r, 0, r, r, r);
  grd.addColorStop(0, 'rgba(255,255,255,1)');
  grd.addColorStop(0.25, 'rgba(255,255,255,0.5)');
  grd.addColorStop(1, 'rgba(255,255,255,0)');
  g.fillStyle = grd;
  g.fillRect(0, 0, size, size);
  return PIXI.Texture.from(c);
}

function glowSprite(PIXI, tex, tint) {
  const s = new PIXI.Sprite(tex);
  s.anchor.set(0.5);
  s.tint = tint;
  s.blendMode = 'add';
  return s;
}

const GLOW_PASSES = [[22, 0.05], [10, 0.14], [2.5, 1]];
function glowLine(g, pts, color, scale = 1, fade = 1) {
  for (const [width, alpha] of GLOW_PASSES) {
    g.moveTo(pts[0], pts[1]);
    for (let i = 2; i < pts.length; i += 2) g.lineTo(pts[i], pts[i + 1]);
    g.stroke({ width: width * scale, color, alpha: alpha * fade, cap: 'round', join: 'round' });
  }
}

function ellipsePts(cx, cy, rx, ry, n = 40) {
  const pts = [];
  for (let i = 0; i <= n; i++) {
    const a = (i / n) * Math.PI * 2;
    pts.push(cx + Math.cos(a) * rx, cy + Math.sin(a) * ry);
  }
  return pts;
}

function drumAt(w, h) {
  // Wide pages: reach past the centered column so the rim sits in the margin.
  const column = Math.min(w - 48, 1152);
  const margin = (w - column) / 2;
  const rx = margin > 160 ? Math.min(w * 0.46, w / 2 - 48) : Math.min(w, h) * 0.34;
  const ry = Math.min(rx * 0.42, h * 0.3);
  const cy = Math.max(ry * 0.55 + 72, Math.min(h * 0.4, h - ry * 0.4));
  return { cx: w * 0.5, cy, rx, ry };
}

function scoreAt(d, w) {
  const column = Math.min(w - 48, 1152);
  const margin = (w - column) / 2;
  let left;
  let width;
  let y;
  if (!fxOnly && margin > 220) {
    width = Math.min(300, margin * 0.78);
    left = Math.max(16, (margin - width) * 0.45);
    y = d.cy + d.ry * 0.85;
  } else {
    width = Math.min(Math.max(180, w * 0.5), 440, Math.max(160, w - 16));
    left = d.cx - width / 2;
    y = d.cy + d.ry + Math.max(26, d.ry * 0.28);
  }
  const groupGap = Math.min(14, width * 0.03);
  const cell = (width - groupGap * 3) / 16;
  return {
    y, cell,
    x(step) {
      return left + Math.floor(step / 4) * groupGap + (step + 0.5) * cell;
    },
  };
}

function snareScene(PIXI, glow) {
  const root = new PIXI.Container();
  const ink = new PIXI.Graphics();
  ink.blendMode = 'add';
  root.addChild(ink);

  const sparks = Array.from({ length: 16 }, () => {
    const s = glowSprite(PIXI, glow, C.cream);
    s.alpha = 0;
    root.addChild(s);
    return s;
  });
  let nextSpark = 0;
  const hits = [];
  let prev = START;
  let hoop = 0;

  function spawn(when, w, h, vel, grace) {
    hits.push({ born: when, vel, grace });
    hoop = Math.max(hoop, vel);
    if (grace || vel < 0.7) return;
    const d = drumAt(w, h);
    for (let i = 0; i < 6; i++) {
      const s = sparks[nextSpark++ % sparks.length];
      const a = rand(0, Math.PI * 2);
      const sp = rand(90, 260);
      s.tint = Math.random() < 0.3 ? C.rust : C.cream;
      s.x = d.cx + rand(-10, 10);
      s.y = d.cy + rand(-6, 6);
      s.vx = Math.cos(a) * sp;
      s.vy = Math.sin(a) * sp * 0.55;
      s.alpha = 0.35 + 0.45 * vel;
      s.scale.set(rand(0.1, 0.22));
    }
  }

  function fire(t0, t1, w, h) {
    const b0 = Math.floor(t0 / BAR);
    const b1 = Math.floor((t1 - 1e-9) / BAR);
    for (let b = b0; b <= b1; b++) {
      const base = b * BAR;
      for (const e of EVENTS) {
        const when = base + e.at;
        if (when > t0 && when <= t1) spawn(when, w, h, e.vel, e.grace);
      }
    }
  }

  function drawStick(d, hit, t) {
    const dur = hit.grace ? 0.04 : hit.vel > 0.5 ? 0.07 : 0.034;
    const age = t - hit.born;
    const life = 1 - age / dur;
    if (age < 0 || life <= 0) return;
    let sx, sy, fx, fy;
    if (hit.grace) {
      sx = d.cx - d.rx * 0.12; sy = d.cy - d.ry * 0.08;
      fx = sx - d.rx * 0.35; fy = sy - d.ry * 2.1;
    } else if (hit.vel > 0.5) {
      sx = d.cx + d.rx * 0.04; sy = d.cy;
      fx = sx + d.rx * 0.42; fy = sy - d.ry * 2.4;
    } else {
      sx = d.cx + d.rx * 0.62; sy = d.cy - d.ry * 0.28;
      fx = sx + d.rx * 0.28; fy = sy - d.ry * 1.5;
    }
    glowLine(ink, [fx, fy, sx, sy], C.cream, 0.45 + 0.55 * hit.vel, life);
  }

  function draw(t, w, h) {
    const d = drumAt(w, h);
    ink.clear();

    const wires = 14;
    for (let i = 0; i < wires; i++) {
      const f = 0.08 + 0.82 * (i / (wires - 1));
      const y = d.cy + d.ry * f;
      const ny = (y - d.cy) / d.ry;
      const half = Math.sqrt(Math.max(0, 1 - ny * ny)) * d.rx * 0.9;
      let buzz = 0;
      for (const hit of hits) {
        const age = t - hit.born;
        if (age < 0 || age > 0.48) continue;
        const tau = 0.05 + 0.14 * hit.vel;
        buzz += hit.vel * Math.exp(-age / tau) * Math.sin(age * (62 + i * 19) + i * 1.3);
      }
      const sign = i % 2 ? 1 : -1;
      const amp = buzz * d.ry * 0.045 * sign;
      glowLine(
        ink,
        [d.cx - half, y + amp, d.cx + half, y + amp * 0.35],
        C.brass,
        0.22 + Math.min(0.5, Math.abs(buzz) * 0.35),
      );
    }

    for (const hit of hits) {
      if (hit.grace) continue;
      const n = hit.vel > 0.6 ? 3 : 1;
      const lifeT = hit.vel > 0.5 ? 0.36 : 0.18;
      for (let i = 0; i < n; i++) {
        const age = t - hit.born - i * 0.018;
        if (age <= 0) continue;
        const u = age / lifeT;
        if (u >= 1) continue;
        const grow = 0.18 + 0.92 * Math.sqrt(u);
        glowLine(ink, ellipsePts(d.cx, d.cy, d.rx * grow, d.ry * grow, 80), C.rust, 0.45, (1 - u) * hit.vel);
      }
    }

    glowLine(ink, ellipsePts(d.cx, d.cy, d.rx * 0.92, d.ry * 0.92, 120), C.rust, 0.28 + hoop * 0.2);
    glowLine(ink, ellipsePts(d.cx, d.cy, d.rx, d.ry, 120), C.brass, 0.5 + hoop * 0.65);
    for (let i = 0; i < 8; i++) {
      const a = (i / 8) * Math.PI * 2 - Math.PI / 2;
      const c = Math.cos(a);
      const s = Math.sin(a);
      const x = d.cx + c * d.rx;
      const y = d.cy + s * d.ry;
      const reach = Math.min(26, Math.max(10, d.rx * 0.04));
      glowLine(ink, [x, y, x + c * reach, y + s * reach * (d.ry / d.rx)], C.brass, 0.4);
    }
    for (const hit of hits) drawStick(d, hit, t);

    const sc = scoreAt(d, w);
    const step = ((Math.floor(t / STEP) % 16) + 16) % 16;
    for (let s = 0; s < 16; s++) {
      const note = GROOVE.find((g) => g.step === s);
      const x = sc.x(s);
      if (!note) {
        glowLine(ink, [x, sc.y - 7, x, sc.y], C.rust, 0.28, 0.65);
        continue;
      }
      const accent = note.vel > 0.5;
      glowLine(
        ink,
        [x, sc.y - (accent ? 28 : 16), x, sc.y],
        accent ? C.cream : C.brass,
        accent ? 0.85 : 0.55,
      );
      if (note.flam) {
        const fx = x - sc.cell * 0.42;
        glowLine(ink, [fx, sc.y - 14, fx, sc.y], C.brass, 0.5);
      }
    }
    glowLine(ink, [sc.x(step), sc.y + 5, sc.x(step), sc.y + 16], C.cream, 0.7);
  }

  // Frozen on the accent of 2: stick down, wires mid-buzz, ripples leaving the center.
  function poster(w, h) {
    const hit = 4 * STEP;
    hits.length = 0;
    hits.push({ born: hit - FLAM, vel: 0.42, grace: true });
    hits.push({ born: hit, vel: 1, grace: false });
    hoop = 0.8;
    draw(hit + 0.04, w, h);
  }

  return {
    root,
    update(t, w, h, dt) {
      if (dt === 0) {
        poster(w, h);
        return;
      }
      fire(prev, t, w, h);
      prev = t;
      while (hits.length && t - hits[0].born > 0.5) hits.shift();
      hoop *= Math.exp(-dt * 12);
      for (const s of sparks) {
        if (s.alpha <= 0.01) continue;
        s.x += s.vx * dt; s.y += s.vy * dt;
        s.vx *= Math.exp(-dt * 3.2); s.vy *= Math.exp(-dt * 3.2);
        s.alpha *= Math.exp(-dt * 4.2);
      }
      draw(t, w, h);
    },
  };
}
