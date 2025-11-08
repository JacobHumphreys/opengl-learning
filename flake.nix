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

          shellHook = ''
            echo "🔧 CPP/C Template.Zig dev shell (Nixpkgs 25.05)"
          '';
        };

#        # Package / build command
#        packages.default = pkgs.stdenv.mkDerivation {
#          pname = "cpp-zig-build";
#          version = "0.1";
#          sandbox = false;
#
#          # Grab same dependencies as devShell
#          buildInputs = with pkgs; [
#            rocmPackages.clang
#            lldb_20
#            zig_0_15
#            pkg-config
#            gdb
#
#            glfw
#            mesa
#            libGL
#            glew
#
#            wayland
#            wayland-scanner
#            libxkbcommon
#            libffi
#          ];
#
#          # Source directory (assumes your code is in ./src)
#          src = ./.;
#
#          # Build command: just run `zig build`
#          buildPhase = ''
#            echo "🏗️ Building with Zig..."
#            export ZIG_GLOBAL_CACHE_DIR=$PWD/.zig-cache
#            zig build --system
#          '';
#
#          installPhase = ''
#            echo "📦 Installing (copying build output)..."
#            mkdir -p $out/bin
#            cp zig-out/bin/* $out/bin/ || true
#          '';
#        };
      });
}

