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

// Note that these "D3D" constants don't match the actual D3D values
// they're from the mocked wrapper implementation that only defines
// what's actually referenced.
// They'll be replaced a more dynamic system in the future?

const TS_WORLD = 0u;
const TS_VIEW = 1u;
const TS_PROJECTION = 2u;
const TS_TEXTURE0 = 3u;
const TS_TEXTURE1 = 4u;
const TS_COUNT = 5u;

const TOP_DISABLE = 1u;
const TOP_SELECTARG1 = 2u;
const TOP_SELECTARG2 = 3u;

const TOP_MODULATE = 4u;

const TOP_ADD = 7u;
const TOP_SUBTRACT = 10u;
const TOP_ADDSMOOTH = 11u;

const TOP_BLENDTEXTUREALPHA = 12u;
const TOP_BLENDCURRENTALPHA = 16u;

const TOP_BLENDENVMAP = 22u;
const TOP_BLENDENVMAPLUMINANCE = 23u;

const TOP_DOTPRODUCT3 = 24u;

const TA_TEXTURE = 0u;
const TA_DIFFUSE = 1u;
const TA_CURRENT = 2u;

const TCI_PASSTHRU = 0x00000000u;
const TCI_CAMERASPACENORMAL = 0x00010000u;
const TCI_CAMERASPACEPOSITION = 0x00020000u;
const TCI_CAMERASPACEREFLECTIONVECTOR = 0x00030000u;

const MCS_MATERIAL = 0u;
const MCS_COLOR1 = 1u;
const MCS_COLOR2 = 2u;

struct TextureState {
    @align(16)
    // D3DTOP_*: texture operation
    color_op: u32,
    // D3DTA_*: texture arguments
    color_arg1: u32,
    color_arg2: u32,
    // D3DTOP_*: texture operation
    alpha_op: u32,
    // D3DTA_*: texture arguments
    alpha_arg1: u32,
    alpha_arg2: u32,
    // texture coordinate index bit-or'ed with D3DTSS_TCI_*:
    texcoordindex: u32,
}

// Color in BGRA format (0xAARRGGBB as u32) for uniforms
// Can't declare as a vec as WGSL doesn't have u8.
//struct COLOR {
//    bgra_value: u32,
//}
//fn read_color(color: COLOR) -> vec4f {
//    let value = color.bgra_value;
//}
// Above doesn't work as structs in uniform
// must have 16-byte alignment. Silly!
alias COLOR = u32;
fn read_color(value: u32) -> vec4f {
    return unpack4x8unorm(value).bgra;
}

const LIGHT_POINT = 1u;
const LIGHT_SPOT = 2u;
const LIGHT_DIRECTIONAL = 3u;

// D3DLIGHT8
struct LightState {
    @align(16)
    light_type: u32,
    diffuse: COLOR,
    specular: COLOR,
    ambient: COLOR,
    position: vec3f,
    direction: vec3f,
    range: f32,
    falloff: f32,
    attenuation0: f32,
    attenuation1: f32,
    attenuation2: f32,
    theta: f32,
    phi: f32,
}

// D3DMATERIAL8
struct MaterialState {
    @align(16)
    diffuse: COLOR,
    ambient: COLOR,
    specular: COLOR,
    emissive: COLOR,
    power: f32,
}

// D3DRS_*: other render states
struct RenderState {
    @align(16)
    alpha_blend_enable: u32, // ALPHABLENDENABLE
    lighting_enable: u32, // LIGHTING
    ambient_color: COLOR, // AMBIENT
    specular_enable: u32, // SPECULARENABLE
    // D3DRS_*MATERIALSOURCE: MSC_* values
    ambient_source: u32,
    diffuse_source: u32,
    specular_source: u32,
    emissive_source: u32,
}

struct State {
    // D3DTS_*: transformation state
    ts: array<mat4x4f, TS_COUNT>,
    // D3DTSS_*: texture stage state
    tex: array<TextureState, 2>,
    rs: RenderState,
    lights: array<LightState, 4>,
    material: MaterialState,
    light_enable_bits: u32, // LightEnable(index, value)
}

@group(0) @binding(0) var<uniform> state: State;

@group(1) @binding(0) var tex1: texture_2d<f32>;
@group(1) @binding(1) var tex1_sampler: sampler;

@group(2) @binding(0) var tex2: texture_2d<f32>;
@group(2) @binding(1) var tex2_sampler: sampler;

// Light contribution.
struct Lighting {
    ambient: vec3f,
    diffuse: vec3f,
    specular: vec3f,
}

struct LightInput {
    world_position: vec3f,
    world_normal: vec3f,
}

fn light(index: u32, in: LightInput) -> Lighting {
    let light = state.lights[index];

    var light_dir: vec3f;
    var atten: f32;
    var spot: f32;

    // wip
    switch (light.light_type) {
    default { return Lighting(); }
    case LIGHT_POINT: {
        let light_vector = light.position - in.world_position;
        light_dir = normalize(light_vector);
        let distance = length(light_vector);
        atten = 1.0 / (
            light.attenuation0 +
            light.attenuation1 * distance +
            light.attenuation2 * distance * distance);
        spot = 1.0;
    }
    case LIGHT_SPOT: {
        let light_vector = light.position - in.world_position;
        light_dir = normalize(light_vector);
        let distance = length(light_vector);
        atten = 1.0 / (
            light.attenuation0 +
            light.attenuation1 * distance +
            light.attenuation2 * distance * distance);
        // todo
        spot = 1.0;
    }
    case LIGHT_DIRECTIONAL: {
        light_dir = -light.direction;
        atten = 1.0;
        spot = 1.0;
    }
    }
    let n_dot_l = max(0.0, dot(in.world_normal, light_dir));

    var result: Lighting;
    result.ambient = read_color(light.ambient).rgb * atten * spot;
    result.diffuse = n_dot_l * read_color(light.diffuse).rgb * atten * spot;

    return result;
}

struct MainOutput {
    @builtin(position) position: vec4f,
    @location(0) normal: vec3f,
    @location(1) diffuse: vec4f,
    @location(2) specular: vec4f,
    @location(3) uv0: vec2f,
    @location(4) uv1: vec2f,
}

@vertex
fn vs_main(
    @location(LOC_POSITION) position: vec3f,
    @location(LOC_NORMAL) normal: vec3f,
    @location(LOC_UV1) uv1: vec2f,
    @location(LOC_DIFFUSE) diffuse: vec4u, // aka color1. vec4u as it's declared as uint8x4
//    @location(LOC_SPECULA) specular: vec4u, // aka color2
) -> MainOutput {
    let world = state.ts[TS_WORLD];
    let view = state.ts[TS_VIEW];
    let proj = state.ts[TS_PROJECTION];

    var output: MainOutput;
    output.position = vec4f(position, 1) * world * view * proj;
    output.normal = (vec4f(normal, 0) * world).xyz; // sure, why not
    output.diffuse = vec4f(diffuse).bgra / 255.0;
    output.specular = vec4f(0, 0, 0, 0); // todo
    output.uv0 = (vec4f(uv1, 0, 1) * state.ts[TS_TEXTURE1]).xy;
    output.uv1 = vec2f(0, 0); // todo
    return output;
}

struct TexArgIn {
    stage: u32,
    uv: array<vec2f, 2>,
    position: vec4f,
    normal: vec3f,
    current: vec4f,
    // ambient: vec4f,
    diffuse: vec4f,
    specular: vec4f,
    // emissive: vec4f,
    texture: vec4f,
}

fn tex_color_arg(in: TexArgIn, arg: u32) -> vec3f {
    switch (arg) {
    case TA_TEXTURE: { return in.texture.rgb; }
    case TA_DIFFUSE: { return in.diffuse.rgb; }
    case TA_CURRENT: { return in.current.rgb; }
    default: { return vec3f(0, 0, 0); }
    }
}

fn tex_color_stage(in: TexArgIn) -> vec3f {
    let tss = state.tex[in.stage];
    let arg1 = tex_color_arg(in, tss.color_arg1);
    let arg2 = tex_color_arg(in, tss.color_arg2);
    switch (tss.color_op) {
    default: { discard; }
    case TOP_DISABLE: { discard; }
    case TOP_SELECTARG1: { return arg1; }
    case TOP_SELECTARG2: { return arg2; }

    case TOP_ADD: { return arg1 + arg2; }
    case TOP_SUBTRACT: { return arg1 - arg2; }
    case TOP_MODULATE: { return arg1 * arg2; }
    case TOP_ADDSMOOTH: { return arg1 + arg2 - arg1 * arg2; }

    case TOP_BLENDTEXTUREALPHA: {
        return mix(arg1, arg2, in.texture.a);
    }
    case TOP_BLENDCURRENTALPHA: {
        return mix(arg1, arg2, in.current.a);
    }

    case TOP_DOTPRODUCT3: { return vec3f(dot(arg1, arg2)); }
    }
}

fn tex_alpha_arg(in: TexArgIn, arg: u32) -> f32 {
    switch (arg) {
    case TA_TEXTURE: { return in.texture.a; }
    case TA_DIFFUSE: { return in.diffuse.a; }
    case TA_CURRENT: { return in.current.a; }
    default: { return 0.0; }
    }
}

fn tex_alpha_stage(in: TexArgIn) -> f32 {
    let tss = state.tex[in.stage];
    let arg1 = tex_alpha_arg(in, tss.alpha_arg1);
    let arg2 = tex_alpha_arg(in, tss.alpha_arg2);
    switch (tss.alpha_op) {
    default: { discard; }
    case TOP_DISABLE: { discard; }
    case TOP_SELECTARG1: { return arg1; }
    case TOP_SELECTARG2: { return arg2; }

    case TOP_ADD: { return arg1 + arg2; }
    case TOP_SUBTRACT: { return arg1 - arg2; }
    case TOP_MODULATE: { return arg1 * arg2; }
    case TOP_ADDSMOOTH: { return arg1 + arg2 - arg1 * arg2; }

    case TOP_BLENDTEXTUREALPHA: {
        return mix(arg1, arg2, in.texture.a);
    }
    case TOP_BLENDCURRENTALPHA: {
        return mix(arg1, arg2, in.current.a);
    }

    case TOP_DOTPRODUCT3: { return 0.0; }
    }
}

fn tex_stage(in: TexArgIn) -> vec4f {
    let color = tex_color_stage(in);
    let alpha = tex_alpha_stage(in);
    return vec4f(color, alpha);
}

fn stage_uv(stage: u32, in: TexArgIn) -> vec2f {
    let tci = state.tex[stage].texcoordindex;
    let index = tci & 0xffff;
    let tf = tci & 0xffff0000;
    var uv: vec2f;
    switch (tf) {
    case TCI_PASSTHRU: { return in.uv[index]; }
    // Camera space defined: https://learn.microsoft.com/en-us/windows/win32/direct3d9/camera-space-transformations
    // TLDR is world * view, not including projection
    case TCI_CAMERASPACEPOSITION: { return in.position.xy; }
    case TCI_CAMERASPACENORMAL: { return in.normal.xy; }
    default: { return in.position.xy; }
    }
}

@fragment
fn fs_main(in: MainOutput) -> @location(0) vec4f {

    // https://learn.microsoft.com/en-us/windows/win32/direct3d9/ambient-lighting
    // TODO: fallback to material color if no vertex color
    var ambient: vec4f;
    switch (state.rs.ambient_source) {
    case MCS_MATERIAL, default: { ambient = read_color(state.material.ambient); }
    case MCS_COLOR1: { ambient = in.diffuse; }
    case MCS_COLOR2: { ambient = in.specular; }
    }

    var diffuse: vec4f;
    switch (state.rs.diffuse_source) {
    case MCS_MATERIAL, default: { diffuse = read_color(state.material.diffuse); }
    case MCS_COLOR1: { diffuse = in.diffuse; }
    case MCS_COLOR2: { diffuse = in.specular; }
    }

    var specular: vec4f;
    switch (state.rs.specular_source) {
    case MCS_MATERIAL, default: { specular = read_color(state.material.specular); }
    case MCS_COLOR1: { specular = in.diffuse; }
    case MCS_COLOR2: { specular = in.specular; }
    }

    var emissive: vec4f;
    switch (state.rs.emissive_source) {
    case MCS_MATERIAL, default: { emissive = read_color(state.material.emissive); }
    case MCS_COLOR1: { emissive = in.diffuse.bgra; }
    case MCS_COLOR2: { emissive = in.specular.bgra; }
    }

    if (state.rs.lighting_enable != 0) {
        let light_in = LightInput(in.position.xyz, in.normal);
        var total_lighting: Lighting;
        total_lighting.ambient = read_color(state.rs.ambient_color).rgb;

        for (var i = 0u; i < 4; i = i + 1) {
            if ((state.light_enable_bits & (1u << i)) != 0) {
                let contribution = light(i, light_in);
                total_lighting.ambient += contribution.ambient;
                total_lighting.diffuse += contribution.diffuse;
                total_lighting.specular += contribution.specular;
            }
        }
        ambient *= vec4f(total_lighting.ambient, 1.0);
        diffuse *= vec4f(total_lighting.diffuse, 1.0);
        specular *= vec4f(total_lighting.specular, 1.0);
    }

    var arg_in: TexArgIn;
    arg_in.uv = array(in.uv0, in.uv1);
    arg_in.position = in.position;
    arg_in.diffuse = diffuse;
    arg_in.specular = specular;
    arg_in.current = ambient + diffuse + specular + emissive;

    if (state.tex[0].color_op != TOP_DISABLE) {
        arg_in.stage = 0;
        arg_in.texture = textureSample(tex1, tex1_sampler, stage_uv(0, arg_in)).bgra;
        arg_in.current = tex_stage(arg_in);
        if (state.tex[1].color_op != TOP_DISABLE) {
            arg_in.stage = 1;
            arg_in.texture = textureSample(tex2, tex2_sampler, stage_uv(1, arg_in)).bgra;
            arg_in.current = tex_stage(arg_in);
        }
    }

    var result = arg_in.current;
//    if state.rs.alpha_blend_enable == 0 {
        result.a = 1.0;
//    }
    return result;
}
