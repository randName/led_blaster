class Ecran {
  constructor() {
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
    return this.send('update', {[name]: value});
  }
}

const ecran = new Ecran();
