// PixiJS backdrop: snareScene, full-viewport, no CDN.
const C = { rust: 0x8a3a22, brass: 0xc4a35a, cream: 0xe8dcc8 };
const BPM = 150;
const BEAT = 60 / BPM;
const FLAM = 0.04;
const ST = [0.18, 0.39, 0.61, 0.82];
const reduceMotion = matchMedia('(prefers-reduced-motion: reduce)').matches;
const rand = (lo, hi) => lo + Math.random() * (hi - lo);

const pointer = { x: 0.5, y: 0.5 };
addEventListener('pointermove', (e) => {
  pointer.x = e.clientX / innerWidth;
  pointer.y = e.clientY / innerHeight;
}, { passive: true });

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
      scene.update(8, w, h, 0);
      app.render();
    };
    addEventListener('resize', () => requestAnimationFrame(still));
    still();
    return;
  }

  let t = 0;
  app.ticker.add((ticker) => {
    const dt = Math.min(ticker.deltaMS / 1000, 0.05);
    t += dt;
    const { width: w, height: h } = app.screen;
    const ease = Math.min(1, dt * 2.5);
    scene.root.x += ((0.5 - pointer.x) * 40 - scene.root.x) * ease;
    scene.root.y += ((0.5 - pointer.y) * 24 - scene.root.y) * ease;
    scene.update(t, w, h, dt);
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
function glowLine(g, pts, color, scale = 1, passes = GLOW_PASSES) {
  for (const [width, alpha] of passes) {
    g.moveTo(pts[0], pts[1]);
    for (let i = 2; i < pts.length; i += 2) g.lineTo(pts[i], pts[i + 1]);
    g.stroke({ width: width * scale, color, alpha, cap: 'round', join: 'round' });
  }
}

function snareScene(PIXI, glow) {
  const root = new PIXI.Container();
  const ink = new PIXI.Graphics();
  const wave = new PIXI.Graphics();
  ink.blendMode = 'add';
  wave.blendMode = 'add';
  root.addChild(ink, wave);

  const sparks = Array.from({ length: 90 }, () => {
    const s = glowSprite(PIXI, glow, Math.random() < 0.3 ? C.rust : C.cream);
    s.alpha = 0;
    root.addChild(s);
    return s;
  });
  let nextSpark = 0;
  const hits = [];
  let lastQ = -1;
  let lastFlamBar = -1;
  let env = 0;

  function spawn(t, w, h, slot, amp, nSparks, dx) {
    const x = w * ST[slot] + dx;
    const y = h * 0.45;
    hits.push({ born: t, x, y, amp });
    env = Math.max(env, amp);
    for (let i = 0; i < nSparks; i++) {
      const s = sparks[nextSpark++ % sparks.length];
      const side = Math.random() < 0.5 ? -1 : 1;
      s.x = x; s.y = y;
      s.vx = side * rand(180, 520);
      s.vy = rand(-90, 90);
      s.alpha = 0.4 + 0.6 * amp;
      s.scale.set(rand(0.12, 0.32) * (0.5 + amp));
    }
  }

  return {
    root,
    update(t, w, h, dt) {
      const q = Math.floor(t / BEAT);
      const barLen = 4 * BEAT;
      const flamBar = Math.floor((t + FLAM) / barLen);
      if (flamBar !== lastFlamBar && t >= flamBar * barLen - FLAM) {
        lastFlamBar = flamBar;
        spawn(t, w, h, 0, 0.35, 6, -10);
      }
      if (q !== lastQ) {
        lastQ = q;
        const slot = ((q % 4) + 4) % 4;
        const accent = slot === 0 || slot === 2;
        spawn(t, w, h, slot, accent ? 1 : 0.28, accent ? 18 : 5, slot === 0 ? 8 : 0);
      }
      while (hits.length && t - hits[0].born > 0.5) hits.shift();
      env *= Math.exp(-dt * 16);

      ink.clear();
      for (const hit of hits) {
        const a = t - hit.born;
        const life = 1 - a / 0.45;
        if (life <= 0) continue;
        const len = (70 + 150 * hit.amp) * (0.35 + 0.65 * life);
        const nWires = 3 + Math.round(hit.amp * 3);
        for (let i = 0; i < nWires; i++) {
          const dy = (i - (nWires - 1) / 2) * 4.5;
          glowLine(ink, [hit.x - len, hit.y + dy, hit.x + len, hit.y + dy], C.brass, (0.35 + 0.65 * hit.amp) * life);
        }
        if (a < 0.08) {
          const sl = 1 - a / 0.08;
          const hgt = 18 + 48 * hit.amp;
          glowLine(ink, [hit.x, hit.y - hgt, hit.x, hit.y + hgt * 0.28], C.rust, sl * (0.5 + 0.5 * hit.amp));
        }
      }

      for (const s of sparks) {
        if (s.alpha <= 0.01) continue;
        s.x += s.vx * dt; s.y += s.vy * dt;
        s.vx *= Math.exp(-dt * 3.2); s.vy *= Math.exp(-dt * 3.2);
        s.alpha *= Math.exp(-dt * 3.4);
      }

      wave.clear();
      const pts = [], wy = h * 0.82, amp = h * 0.07 * (0.2 + 0.8 * env);
      for (let x = 0; x <= w; x += 3) {
        const u = x / w;
        const phase = 140 * u - t * 28;
        const crack = Math.sin(x * 0.55 + t * 55) * 0.28 + Math.sin(x * 1.4 - t * 90) * 0.12;
        pts.push(x, wy + amp * Math.exp(-u * 5.5) * (Math.sin(phase) + crack));
      }
      glowLine(wave, pts, C.brass);
    },
  };
}
