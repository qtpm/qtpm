package qtpm

import (
	"github.com/fatih/color"
	"os"
)

func Touch(config *PackageConfig, debug, verbose bool) {
	_, err := BuildPackage(config, config, true, debug, false, !config.IsApplication)
	if err != nil {
		color.Red("\nBuild Error: %v\n", err)
		os.Exit(1)
	}
	if verbose {
		printSuccess("\nFinish updating CMakeLists.txt successfully\n")
	}
}
