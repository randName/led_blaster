precision mediump float;

uniform float u_fade;
uniform float u_time;
uniform float u_speed;
uniform float u_offset;

uniform vec2 u_resolution;
uniform float u_brightness;

uniform float u_state[8];
uniform float u_lines[8];
uniform float u_active[8];
uniform float u_sound_vol;

const vec2 dimensions = vec2(22.6, 90.0);

float random (in float x) {
    return fract(sin(x)*1e4);
}

float random (in vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233)))* 43758.5453123);
}

vec2 random2(in vec2 p) {
    return fract(sin(vec2(dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3))))*43758.5453);
}

vec3 hsv2rgb(in vec3 c) {
    const vec3 rgb = vec3(0., 4., 2.);
    vec3 h = clamp(abs(mod(c.x*6.+rgb,6.)-3.)-1.,0.,1.);
    h = h*h*(3.-2.*h);
    return c.z * mix(vec3(1.), h, c.y);
}

vec2 bounds(in vec2 st) {
    const vec3 f_cos = vec3(-0.2,-0.07,0.14);
    const vec3 f_sin = vec3(0.06,-0.08,0.06);
    vec2 b = vec2(0.);
    float l = step(2.,st.x);
    vec3 wx = 1.2 * st.x * vec3(1.,2.,3.);
    b.s = smoothstep(1.6,2.0,st.x)*(1.-l) + l*(1.-smoothstep(2.2,2.25,st.x)) - 0.28;
    b.t = 0.77 + dot(f_cos*cos(wx)+f_sin*sin(wx),vec3(1.));
    return b;
}

vec3 strand(in vec2 st, in vec3 color, in float hoffset, in vec2 scale, in float ts)
{
    float glow = 6.4;
    float multiplier = 0.0;
    for (int i=0; i<8; i++) {
        multiplier += u_active[i] * clamp(1.-smoothstep(0.,0.25,abs(st.x/5.-float(i)/8.)),0.,1.);
    }
    multiplier = 0.4 + 0.8 * multiplier;

    float curve = 1.-abs(st.y-0.4-sin(st.x*scale.x+u_time*ts-hoffset)*multiplier*scale.y)*100.;
    return clamp(curve,0.,1.) + clamp((glow+curve)/glow,0.,1.) * 0.4 * color;
}

vec3 rainbow(in vec3 c, in float type) {
    float t = u_offset + u_time * u_speed * 10.;
    float avg = (c.r+c.g+c.b)/3.;
    c = avg + (c-avg)*sin(vec3(0.0,0.333,0.666)+t);
    vec3 a = sin(vec3(0.234,0.422,0.505)*t+vec3(0.134,0.328,0.560))*vec3(0.610,0.000,0.574);
    vec3 b = sin(vec3(0.4,0.3,0.3)*t+vec3(1.1244,3.43215,6.435))*vec3(0.4,0.1,0.3);
    c += mix(a, b, type);
    return c;
}

float pattern(in vec2 st, in vec2 v, in float t) {
    vec2 p = floor(st+v);
    return step(t, random(100.+p*.000001)+random(p.x)*0.5);
}

void main() {
    vec2 st = gl_FragCoord.xy / dimensions;
    vec2 b = bounds(st);
    vec2 uv = vec2(st.x, smoothstep(b.s, b.t, st.y));
    float cut = step(b.s, st.y)*(1.-step(b.t, st.y));
    vec3 color = vec3(0.);

    vec2 bist = st * vec2(10.,50.);

    vec2 ipos = floor(bist);  // integer
    vec2 fpos = fract(bist);  // fraction

    vec2 vel = vec2(u_time * 15.);
    vel *= vec2(-1.,0.) * random(1.+ipos.y); // direction

    vec3 r = hsv2rgb(vec3(random(1.+ipos.y)+st.x/5.+u_time/4.,0.6,pattern(bist,vel,0.7)));

    vec3 o = rainbow(vec3(0.), 1.) * clamp(u_brightness, 0.1, 1.0);
    o += strand(uv, vec3(0.660,0.267,0.159), 2.18, vec2(1.0,0.19), 1.8);
    o += u_lines[0] * strand(uv, vec3(0.406,0.555,1.000), 2.18, vec2(2.5,0.16), 1.5);
    o += u_lines[1] * strand(uv, vec3(1.000,0.034,0.616), 1.14, vec2(1.5,0.20), 1.5);
    o += u_lines[2] * strand(uv, vec3(0.266,0.641,1.000), 1.65, vec2(0.8,0.19), 1.2);
    o += u_lines[3] * strand(uv, vec3(0.660,0.267,0.159), 2.18, vec2(1.8,0.19), 1.2);
    o += u_lines[4] * strand(uv, vec3(0.253,1.000,0.652), 2.47, vec2(2.1,0.19), 1.2);

    color = mix(color, r, u_state[1]);
    color = mix(o, color, u_state[0]);

    gl_FragColor = vec4(color * cut * u_fade, 1.0);
}

