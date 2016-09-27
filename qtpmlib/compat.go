package qtpm

import (
	"io/ioutil"
	"os"
	"os/exec"
	"path/filepath"
	"runtime"
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

func FindQt(dir string) string {
	// from environment variable
	env := os.Getenv("QTDIR")
	if env != "" {
		return env
	}

	// from setting file
	userSetting, err := LoadUserConfig(dir)
	if err == nil && userSetting.QtDir != "" {
		return userSetting.QtDir
	}

	// from qmake
	cmd := exec.Command("qmake", "-query", "QT_INSTALL_PREFIX")
	output, err := cmd.CombinedOutput()
	if err == nil {
		return strings.TrimSpace(string(output))
	}

	// Search default install folder
	var paths []string
	if runtime.GOOS == "windows" {
		paths = []string{
			os.Getenv("USERPROFILE"),
			"C:\\", "D:\\",
			os.Getenv("ProgramFiles"),
			os.Getenv("ProgramFiles(x86)"),
		}
	} else {
		paths = []string{
			os.Getenv("HOME"),
			"/",
		}
	}
	for _, path := range paths {
		versions, err := ioutil.ReadDir(filepath.Join(path, "Qt"))
		if err != nil {
			continue
		}
		var biggestDir string
		for _, version := range versions {
			if strings.HasPrefix(version.Name(), "5.") {
				stat, _ := os.Stat(filepath.Join(path, "Qt", version.Name()))
				if !stat.IsDir() {
					continue
				}
				if version.Name() > biggestDir {
					biggestDir = version.Name()
				}
			}
		}
		if biggestDir == "" {
			continue
		}
		targets, err := ioutil.ReadDir(filepath.Join(path, "Qt", biggestDir))
		var candidate string
		for _, target := range targets {
			name := target.Name()
			if hasPrefixes(name, "ios", "android", "winphone", "winrt") {
				continue
			}
			if strings.HasPrefix(name, "mingw") {
				// mingw has higher priority than MSVC by default
				// because Qt bundles mingw. It is good for default behavior to make it easy
				candidate = name
				break
			} else if name > candidate {
				// Higher version of MSVC has higher priority
				candidate = name
			}
		}
		if candidate != "" {
			return filepath.Join(path, "Qt", biggestDir, candidate)
		}
	}
	return ""
}

func hasPrefixes(str string, prefixes ...string) bool {
	for _, prefix := range prefixes {
		if strings.HasPrefix(str, prefix) {
			return true
		}
	}
	return false
}
