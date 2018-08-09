const canvas = document.getElementById('ecran');
const ecran = new Ecran(canvas);

const tspace = document.getElementById('u_t');
const fspace = document.getElementById('fps');

let updater = setInterval(updateStats, 1250);

let settings = {
  color: [0, 0, 0],
  halt: () => { clearInterval(updater); ecran.stop(1); }
}

const gui = new dat.GUI({resizable: false});
gui.close();

gui.addColor(settings, 'color').onChange(updateColor);
gui.add(settings, 'halt');

function updateStats() {
  ecran.status().then((s) => {
    tspace.innerHTML = s.t.join(', ');
    fspace.innerHTML = s.fps.join(', ');
  });
}

function updateColor(c) {
  let div = 255, lim = div * 2.5;
  let s = c.reduce((a, v) => a + v);
  if ( s > lim ) { div *= s/lim; }
  ecran.update('c', c.map((v) => v/div));
}
