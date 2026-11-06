// Hash only: notes.html?doc=… is not a selector and would throw before particles spawn.
const links = [...document.querySelectorAll("nav a")];
const sections = links
  .map((a) => {
    const href = a.getAttribute("href") || "";
    const hash = href.includes("#") ? href.slice(href.indexOf("#")) : "";
    return hash.length > 1 ? document.querySelector(hash) : null;
  })
  .filter(Boolean);

const watch = new IntersectionObserver((entries) => {
  entries.forEach((entry) => {
    if (!entry.isIntersecting) return;
    const id = "#" + entry.target.id;
    links.forEach((a) => a.classList.toggle("on", a.getAttribute("href") === id));
  });
}, { rootMargin: "-40% 0px -50% 0px" });

sections.forEach((section) => watch.observe(section));
if (document.body.dataset.note !== undefined)
  document.querySelector('nav a[href$="#notes"]')?.classList.add("on");

// SNR3-<ver>-{win|linux|mac}-{arch}-{installer|vst3|standalone}.{zip|msi}
const FILE = /-(win|linux|mac)-[^-]+-(installer|vst3|standalone)(?:\.zip|\.msi)$/i;
const box = document.querySelector(".downloads");
if (box) {
  fetch("releases.json")
    .then((r) => (r.ok ? r.json() : null))
    .then((data) => {
      if (!data?.tag || !Array.isArray(data.files)) return;
      const base = `https://github.com/iamtew/SNR3/releases/download/${data.tag}/`;
      for (const name of data.files) {
        const m = name.match(FILE);
        if (!m) continue;
        const a = box.querySelector(`[data-kind="${m[2].toLowerCase()}"] a[data-os="${m[1].toLowerCase()}"]`);
        if (!a) continue;
        a.href = base + encodeURIComponent(name);
        a.hidden = false;
      }
      box.querySelectorAll(".dl-row").forEach((row) => {
        if (row.querySelector("a[href]")) row.hidden = false;
      });
    })
    .catch(() => {});
}
