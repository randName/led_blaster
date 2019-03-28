const canvas = document.getElementById('ecran')
const ecran = new Ecran(canvas)

let fader = null

let settings = {
  fade: 0.1,
  lines: [0.1, 0.1, 0.1, 0.1, 0.1],
  active: [0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1],
  bright: 1,
  color: [0, 0, 0],
  thor: () => ecran.update('u_state', [1,1,0,0,0,0,0,0]),
  wave: () => ecran.update('u_state', [0,0,0,0,0,0,0,0]),
  beat: () => { settings.bright = 1; fader = setInterval(fadeOut, 24) }
}

ecran.load('hyperbands.frag')

const gui = new dat.GUI({resizable: false})

gui.addColor(settings, 'color').onChange(updateColor)
gui.add(settings, 'fade', 0, 0.8).onChange(updateFade)
gui.add(settings, 'beat')
gui.add(settings, 'thor')
gui.add(settings, 'wave')

const lineFolder = gui.addFolder('lines')
settings.lines.forEach((s, i) => {
  lineFolder.add(settings.lines, i, 0, 1).onChange(updateLines)
})

const activityFolder = gui.addFolder('activity')
settings.active.forEach((s, i) => {
  activityFolder.add(settings.active, i, 0, 1).onChange(updateActive)
})

function fadeOut() {
  settings.bright *= 0.9
  ecran.update('u_brightness', settings.bright)
  if ( settings.bright < 0.3 ) {
    clearInterval(fader)
  }
}

function updateColor(c) {
  let div = 255, lim = div * 2.5;
  let s = c.reduce((a, v) => a + v);
  if ( s > lim ) { div *= s/lim; }
  ecran.update('c', c.map((v) => v/div));
}

function updateFade(v) {
  ecran.update('u_fade', v)
}

function updateLines(v) {
  ecran.update('u_lines', [...settings.lines, 0,0,0])
}

function updateActive(v) {
  ecran.update('u_active', settings.active)
}
