package qtpm

import (
	"github.com/fatih/color"
	"os"
	"os/exec"
	"path/filepath"
	"runtime"
	"strings"
)

func Run(refresh, debug bool) {
	config := MustLoadConfig(".", true)
	if !config.IsApplication {
		color.Red("This project is not application\n")
		os.Exit(1)
	}
	Build(refresh, debug)
	cmdPath := filepath.Join(config.Dir, BuildFolder(debug), strings.ToLower(config.Name))
	if runtime.GOOS == "darwin" {
		cmd := exec.Command("open", "-a", cmdPath+".app")
		cmd.Start()
	} else if runtime.GOOS == "windows" {
		cmd := exec.Command(cmdPath + ".exe")
		cmd.Start()
	} else {
		cmd := exec.Command(cmdPath)
		cmd.Start()
	}
}
