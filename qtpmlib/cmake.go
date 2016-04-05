package qtpm

import (
	"fmt"
	"github.com/fatih/color"
	"github.com/shibukawa/configdir"
	"log"
	"os"
	"path/filepath"
	"runtime"
	"strings"
)

var useAsmJS bool
var useWebAssemblyCompiler bool

func SetUseWebAssemblyCompiler(asmJS, webAsm bool) {
	hasError := false
	dir := configdir.New("", "qtpm").QueryCacheFolder()
	if webAsm {
		asmJS = true
	}
	if asmJS {
		if !dir.Exists("emsdk") {
			color.Red("Compiler is not set up yet. Run 'qtpm setup webasm' first.\n")
			hasError = true
		}
	}
	if webAsm {
		dir := configdir.New("", "qtpm").QueryCacheFolder()
		if !dir.Exists("binaryen") {
			color.Red("Compiler is not set up yet. Run 'qtpm setup webasm' first.\n")
			hasError = true
		}
	}
	if hasError {
		os.Exit(1)
	}
	useAsmJS = asmJS
	useWebAssemblyCompiler = webAsm
}

func CMake(workDir string, generate, debug bool, target string, buildArgs []string) *Cmd {
	if useAsmJS {
		if runtime.GOOS == "windows" {
			color.Red("Now WebAssembly build on Windows is not supporting.\n")
			os.Exit(1)
		}
		dir := configdir.New("", "qtpm").QueryCacheFolder()
		shellScriptPath := filepath.Join(os.TempDir(), "cmake.sh")
		shellScript, err := os.Create(shellScriptPath)
		if err != nil {
			log.Fatalln(err)
		}
		shellScript.Chmod(0755)
		shellScript.WriteString("#!/bin/sh\n")
		shellScript.WriteString("set -e\n")
		fmt.Fprintf(shellScript, "source %s\n", filepath.Join(dir.Path, "emsdk", "emsdk_env.sh"))

		if generate {
			fmt.Fprintf(shellScript, "emcmake cmake .. %s\n", strings.Join(buildArgs, " "))
		} else {
			fmt.Fprintf(shellScript, "emmake make %s\n", target)
		}
		shellScript.Close()
		println(shellScriptPath)
		return Command("sh", workDir, shellScriptPath)
	} else {
		var args []string
		if generate {
			args = append(args, "..")
			args = append(args, buildArgs...)
		} else {
			args = []string{"--build", "."}
			if debug {
				args = append(args, "--config", "Debug")
			} else {
				args = append(args, "--config", "Release")
			}
			if target != "" {
				args = append(args, "--target", target)
			}
		}
		return Command("cmake", workDir, args...)
	}
}
