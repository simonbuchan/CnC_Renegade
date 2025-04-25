use std::fs;
use std::path::PathBuf;
use std::process::Command;

fn main() {
    let mut args = std::env::args();
    args.next();
    let manifest_dir = PathBuf::from(args.next().expect("manifest_dir arg"));
    let crate_name = args.next().expect("crate_name arg");
    let output_path = args.next().expect("output_path arg");
    let header_path = args.next().expect("header_path arg");
    let depfile_path = args.next().expect("depfile_path arg");

    let target = "i686-pc-windows-msvc";
    let expected_build_output =
        manifest_dir.join(format!("target/{target}/debug/{crate_name}.lib"));

    let build_status = Command::new("cargo")
        .arg("build")
        .arg("--manifest-path")
        .arg(manifest_dir.join("Cargo.toml"))
        .arg("--target")
        .arg(target)
        .arg("--package")
        .arg(&crate_name)
        .arg("--lib")
        .arg("--color")
        .arg("always")
        .status()
        .unwrap();
    if !build_status.success() {
        eprintln!("cargo build failed");
        std::process::exit(1);
    }

    println!("copying {} to {}", expected_build_output.display(), output_path);
    fs::copy(expected_build_output, output_path).expect("copy output");

    println!("generating bindings...");
    let bindings = cbindgen::Builder::new()
        .with_crate_and_name(manifest_dir, &crate_name)
        .with_pragma_once(true)
        .generate()
        .expect("generate bindings");

    println!("writing header {header_path}");
    if let Err(error) = fs::remove_file(&header_path) {
        if error.kind() != std::io::ErrorKind::NotFound {
            panic!("remove old header: {error}")
        }
    }
    bindings.write_to_file(&header_path);

    println!("writing depfile {depfile_path}");
    bindings.generate_depfile(&header_path, &depfile_path);
}
