class Ecran {
  constructor(canvas) {
    this.sandbox = new GlslCanvas(canvas);
    this.shader = 'run.frag';
    this.load();
  }

  load() {
    fetch('/shader/' + this.shader)
      .then(r => r.text())
      .then(r => this.sandbox.load(r));
  }

  status() {
    return fetch('/status').then(r => r.json());
  }

  send(action, data) {
    return fetch('/', {
      mode: 'cors', method: 'POST',
      body: JSON.stringify({action, data}),
      headers: {'content-type': 'application/json'}
    }).then(r => r.json());
  }

  sync() {
    return this.send('sync');
  }

  stop(halt = false) {
    return this.send('stop', {halt});
  }

  update(name, value) {
    if ( Array.isArray(value) ) {
      this.sandbox.setUniform(name, ...value);
    } else {
      this.sandbox.setUniform(name, value);
    }
    return this.send('update', {[name]: value});
  }
}
