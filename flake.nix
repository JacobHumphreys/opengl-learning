{
  description =
    "Flake Dependencies for my C/CPP template for building with Zig. Generated with AI";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let pkgs = import nixpkgs { inherit system; };
      in {
        # Dev shell
        devShells.default = pkgs.mkShell {
          name = "clang-zig-shell";

          buildInputs = with pkgs; [
            rocmPackages.clang
            lldb_20
            zig_0_15
            pkg-config
            gdb

            glfw
            mesa # OpenGL headers and library
            libGL # OpenGL system library
            glew

            wayland
            wayland-scanner
            libxkbcommon
            libffi
          ];
          LD_LIBRARY_PATH = pkgs.lib.makeLibraryPath [
            pkgs.glfw
            pkgs.mesa # OpenGL headers and library
            pkgs.libGL # OpenGL system library
            pkgs.glew
            pkgs.wayland
            pkgs.wayland-scanner
            pkgs.libxkbcommon
            pkgs.libffi
          ];

          shellHook = ''
            echo "🔧 CPP/C Template.Zig dev shell (Nixpkgs 25.05)"
                # Include paths for the compiler
                export CPATH="${pkgs.glew}/include:${pkgs.glfw}/include:${pkgs.libGL}/include:${pkgs.mesa}/include"

                # Library paths
                export LIBRARY_PATH="${pkgs.glew}/lib:${pkgs.glfw}/lib:${pkgs.libGL}/lib:${pkgs.mesa}/lib"

                # pkg-config path
                export PKG_CONFIG_PATH="${pkgs.glew}/lib/pkgconfig:${pkgs.glfw}/lib/pkgconfig:${pkgs.mesa}/lib/pkgconfig"

          '';
        };
      });
}

