#from: https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux

{
	description = "Raylib development environment";

  	inputs = {
    		nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  	};

  	outputs = {self, nixpkgs, ...}:
  	let
    		system = "x86_64-linux";

    		pkgs = import nixpkgs {inherit system;};
  	in
  	{

    		devShells."${system}".default = pkgs.mkShell {

        		packages = [

          			#my additions
          			pkgs.raylib
          			pkgs.gcc
          			pkgs.gdb
          			pkgs.python3
				#------------

          			pkgs.libGL

          			# X11 dependencies
          			pkgs.libx11
          			pkgs.libx11.dev
          			pkgs.libxcursor
          			pkgs.libxi
          			pkgs.libxinerama
          			pkgs.libxrandr

          			pkgs.emscripten

        		];

        		LD_LIBRARY_PATH = pkgs.lib.makeLibraryPath [pkgs.alsa-lib];
		};

  	};

}
