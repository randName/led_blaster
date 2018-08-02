const canvas = document.getElementById('ecran');
const ecran = new Ecran(canvas);

const gui = new dat.GUI({resizable: false});
gui.close();

let u = new Uniforms(ecran);
Object.keys(u.limits).forEach((k) => {
  gui.add(u, k, ...u.limits[k]).onFinishChange((v) => u.update(k, v));
});
