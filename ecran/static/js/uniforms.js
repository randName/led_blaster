class Uniforms {
  constructor(ecran) {
    this.ecran = ecran;
    this.values = ['f_cos', 'f_sin', 'b_end', 'f_h', 'f_w', 'b_h'];

    this.f_h = 0.68; this.f_w = 1.185;
    this.f_cos_1 = -0.2;  this.f_cos_2 = -0.07; this.f_cos_3 =  0.14;
    this.f_sin_1 =  0.06; this.f_sin_2 = -0.08; this.f_sin_3 =  0.06;
    this.b_h = 0.72; this.b_end_l = 1.4; this.b_end_r = 2.6;

    this.limits = {
      f_h: [0, 1],
      f_w: [1, 2],
      f_cos_1: [-1, 1],
      f_cos_2: [-1 ,1],
      f_cos_3: [-1, 1],
      f_sin_1: [-1, 1],
      f_sin_2: [-1, 1],
      f_sin_3: [-1, 1],
      b_end_l: [1, 1.8],
      b_end_r: [2.2, 3],
      b_h: [0, 1]
    };

    setTimeout(() => this.load(), 200);
  }

  load() {
    this.values.forEach((k) => this.update(k, this[k]));
  }

  get b_end() {
    return [this.b_end_l, this.b_end_r];
  }

  get f_cos() {
    return [this.f_cos_1, this.f_cos_2, this.f_cos_3];
  }

  get f_sin() {
    return [this.f_sin_1, this.f_sin_2, this.f_sin_3];
  }

  update(k, v) {
    this.values.some((u) => {
      if ( k.includes(u) ) {
        ecran.update(u, this[u]);
        return true;
      }
    });
  }
}
