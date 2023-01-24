// Follow from legacy/deps/ccommon/rust/ccommon-sys/build.rs
extern crate bindgen;

fn main() {
    // let project_dir = env::var("CARGO_MANIFEST_DIR").unwrap();
    // println!("cargo:rustc-link-search={}/measure", project_dir);
    // println!("cargo:rustc-link-lib=measure");
    // println!("cargo:rerun-if-changed=wrapper.h");

    let bindings = bindgen::Builder::default()
        .clang_arg("-I./measure")
        .clang_arg("-lpfm")
        .header("wrapper.h")
        .generate()
        .expect("Unable to genrate bindings.");
    
    bindings
        .write_to_file("src/bindings.rs")
        .expect("Couldn't write bindings!");
}