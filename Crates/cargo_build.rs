use std::fs;
use std::process::Command;

fn main() {
    let mut args = std::env::args();
    args.next();
    let manifest_dir = args.next().expect("manifest_dir arg");
    let crate_name = args.next().expect("crate_name arg");
    let output_path = args.next().expect("output_path arg");

    let target = "i686-pc-windows-msvc";
    let expected_build_output = format!("{manifest_dir}/target/{target}/debug/{crate_name}.lib");

    Command::new("cargo")
        .arg("build")
        .arg("--manifest-path")
        .arg(format!("{manifest_dir}/Cargo.toml"))
        .arg("--target")
        .arg(target)
        .arg("--package")
        .arg(crate_name)
        .arg("--lib")
        .status()
        .unwrap();

    fs::copy(expected_build_output, output_path).expect("copy output");
}
