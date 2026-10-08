{
  description = "A basic flake providing a devShell";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
  };

  outputs = { nixpkgs, ... }: {
    devShells = builtins.mapAttrs (system: pkgs: {
      default = pkgs.mkShell {
        packages = with pkgs; [
			gcc
			gnumake
			libGL
			wayland
			glfw
			libxkbcommon
			pkg-config
			glew
			glm
			assimp
			imgui
        ];
      };
    }) nixpkgs.legacyPackages;
  };
}
