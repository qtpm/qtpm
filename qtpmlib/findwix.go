package qtpm

import (
	"io/ioutil"
	"os"
	"path/filepath"
	"strings"
)

const wixToolFile = "candle.exe"

func FindWix() string {
	// Retieve from environment variable
	env := os.Getenv("WIXDIR")
	if env != "" {
		return env
	}
	// Retieve from PATH
	for _, path := range filepath.SplitList(os.Getenv("PATH")) {
		if _, err := os.Stat(filepath.Join(path, wixToolFile)); err == nil {
			return path
		}
	}
	// Retieve from "Program Files"
	installDirEnvKeys := []string{"ProgramFiles", "ProgramFiles(x86)", "ProgramW6432"}
	for _, envKey := range installDirEnvKeys {
		installDir := os.Getenv(envKey)
		if installDir == "" {
			continue
		}
		dirs, err := ioutil.ReadDir(installDir)
		if err != nil {
			continue
		}
		// Reverse order to get higher version first
		for i := len(dirs) - 1; i > -1; i-- {
			dir := dirs[i]
			if !dir.IsDir() || !strings.HasPrefix(dir.Name(), "WiX Toolset") {
				continue
			}
			path := filepath.Join(installDir, dir.Name(), "bin")
			if _, err := os.Stat(filepath.Join(path, wixToolFile)); err == nil {
				return path
			}
		}
	}
	return ""
}
