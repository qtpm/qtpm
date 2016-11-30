package qtpm

import (
	"os/exec"
	"path/filepath"
	"strings"
)

var defaultTools = map[string][]string{
	"mingw":       []string{"-G", "MinGW Makefiles"},
	"msvc2013":    []string{"-G", "Visual Studio 12 2013"},
	"msvc2013_64": []string{"-G", "Visual Studio 12 2013 Win64"},
	"msvc2015":    []string{"-G", "Visual Studio 14 2015"},
	"msvc2015_64": []string{"-G", "Visual Studio 14 2015 Win64"},
}

func CMakeOptions(qtDir string) []string {
	base := filepath.Base(qtDir)
	if strings.HasPrefix(base, "mingw") {
		return defaultTools["mingw"]
	}
	opt, ok := defaultTools[base]
	if !ok {
		cmd := exec.Command("ninja", "--version")
		output, err := cmd.CombinedOutput()
		if err == nil && len(output) > 0 {
			return []string{"-G", "Ninja"}
		}
	}
	return opt
}

func hasPrefixes(str string, prefixes ...string) bool {
	for _, prefix := range prefixes {
		if strings.HasPrefix(str, prefix) {
			return true
		}
	}
	return false
}
