// Must match WgslLocation enum
const LOC_POSITION = 0;
const LOC_BLEND_WEIGHTS = 1;
const LOC_BLEND_INDICES = 2;
const LOC_NORMAL = 3;
const LOC_DIFFUSE = 4;
const LOC_SPECULAR = 5;
const LOC_UV1 = 6;
const LOC_UV2 = 7;

const WHITE = vec4f(1, 1, 1, 1);

struct State {
    ts_world: mat4x4f,
    ts_view: mat4x4f,
    ts_proj: mat4x4f,
    ts_tex1: mat4x4f,
    ts_tex2: mat4x4f,
}

@group(0) @binding(0) var<uniform> state: State;
@group(1) @binding(0) var tex1: texture_2d<f32>;
@group(1) @binding(1) var tex1_sampler: sampler;

struct ColorOutput {
    @builtin(position) position: vec4f,
    @location(0) color: vec4f,
}

fn transform(position: vec3f) -> vec4f {
    return vec4f(position, 1) * state.ts_world * state.ts_view * state.ts_proj;
}

fn transform_uv1(position: vec2f) -> vec2f {
    return (vec4f(position, 0, 1) * state.ts_tex1).xy;
}

@vertex
fn vs_xyz(
    @location(LOC_POSITION) position: vec3f,
) -> ColorOutput {
    var output: ColorOutput;
    output.position = transform(position);
    output.color = vec4f(1, 0, 1, 1);
    return output;
}

@vertex
fn vs_xyz_diffuse(
    @location(LOC_POSITION) position: vec3f,
    @location(LOC_DIFFUSE) diffuse: u32,
) -> ColorOutput {
    var output: ColorOutput;
    output.position = transform(position);
    output.color = unpack4x8unorm(diffuse);
    return output;
}

struct Tex1Output {
    @builtin(position) position: vec4f,
    @location(0) color: vec4f,
    @location(1) uv1: vec2f,
}

@vertex
fn vs_xyz_uv(
    @location(LOC_POSITION) position: vec3f,
    @location(LOC_UV1) uv1: vec2f,
) -> Tex1Output {
    var output: Tex1Output;
    output.position = transform(position);
    output.color = WHITE; // so we can share fs_tex1
    output.uv1 = transform_uv1(uv1);
    return output;
}

@vertex
fn vs_xyz_diffuse_uv(
    @location(LOC_POSITION) position: vec3f,
    @location(LOC_UV1) uv1: vec2f,
    @location(LOC_DIFFUSE) diffuse: u32,
) -> Tex1Output {
    var output: Tex1Output;
    output.position = transform(position);
    output.color = unpack4x8unorm(diffuse);
    output.uv1 = transform_uv1(uv1);
    return output;
}

@fragment
fn fs_color(input: ColorOutput) -> @location(0) vec4f {
    return input.color;
}

@fragment
fn fs_tex1(input: Tex1Output) -> @location(0) vec4f {
    // Textures seem to actually be 'ARGB', e.g. BGRA in non-DX format. ehh.
    return textureSample(tex1, tex1_sampler, input.uv1).bgra * input.color;
}
