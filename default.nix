with import <nixpkgs> {}; {
  freestyleEnv = stdenv.mkDerivation rec {
    name = "Neuro";
    buildInputs = [ ninja llvm_38 gcc6 ];
  };
}

