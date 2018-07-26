class Ecran {
  constructor() {
  }

  send(action, data) {
    return fetch('/', {
      mode: 'cors', method: 'POST',
      body: JSON.stringify({action, data}),
      headers: {'content-type': 'application/json'}
    }).then(r => r.json());
  }
}

const ecran = new Ecran();
