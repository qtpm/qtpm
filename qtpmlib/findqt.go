package qtpm

import (
	"io/ioutil"
	"os"
	"os/exec"
	"path/filepath"
	"runtime"
	"strings"
)

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
		candidate, ok := checkInstallDir(path)
		if ok {
			return candidate
		}
	}
	return ""
}

/*
   Network installer install to ~/Qt/5.7
   Offline installer install to ~/Qt5.7.0/5.7
*/
func checkInstallDir(path string) (string, bool) {
	dirs, err := ioutil.ReadDir(path)
	if err != nil {
		return "", false
	}
	var biggestVersion string
	for _, dir := range dirs {
		if !strings.HasPrefix(dir.Name(), "Qt") {
			continue
		}
		versions, err := ioutil.ReadDir(filepath.Join(path, dir.Name()))
		if err != nil {
			continue
		}
		var biggestLocalDir string
		for _, version := range versions {
			if strings.HasPrefix(version.Name(), "5.") {
				stat, _ := os.Stat(filepath.Join(path, dir.Name(), version.Name()))
				if !stat.IsDir() {
					continue
				}
				if version.Name() > biggestLocalDir {
					biggestLocalDir = version.Name()
				}
			}
		}
		if biggestLocalDir == "" {
			continue
		}
		candidate, ok := searchQtFolder(filepath.Join(path, dir.Name(), biggestLocalDir))
		if ok && candidate > biggestVersion {
			biggestVersion = candidate
		}
	}
	return biggestVersion, biggestVersion != ""
}

func searchQtFolder(folderPath string) (string, bool) {
	targets, _ := ioutil.ReadDir(folderPath)
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
		return filepath.Join(folderPath, candidate), true
	}
	return "", false
}
