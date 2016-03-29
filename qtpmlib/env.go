package qtpm

import (
	"fmt"
	"strings"
)

var allVars = []string{
	"QTDIR",
	"PKGDIR",
	"CMAKEOPT",
}

func Env(vars []string) {
	if len(vars) == 0 {
		vars = allVars
	}
	config, _ := LoadConfig(".", true)
	var configDir string
	if config != nil {
		configDir = config.Dir
	}
	for _, variable := range vars {
		switch variable {
		case "QTDIR":
			fmt.Printf("QTDIR=\"%s\"\n", FindQt(configDir))
		case "PKGDIR":
			fmt.Printf("PKGDIR=\"%s\"\n", configDir)
		case "CMAKEOPT":
			fmt.Printf("CMAKEOPT=\"%s\"\n", strings.Join(CMakeOptions(FindQt(configDir)), " "))
		}
	}
}
