package qtpm

import (
	"github.com/fatih/color"
	"io/ioutil"
	"os"
	"path/filepath"
)

func Touch(rootConfig *PackageConfig, buildType BuildType, verbose bool) {
	projects := []*PackageConfig{
		rootConfig,
	}
	dirs, _ := ioutil.ReadDir(filepath.Join(rootConfig.Dir, "vendor"))
	for _, dir := range dirs {
		config, err := LoadConfig(filepath.Join(rootConfig.Dir, "vendor", dir.Name()), false)
		if err == nil {
			projects = append(projects, config)
		}
	}
	for _, project := range projects {
		printSubSection("Refreshing %s\n", project.Name)
		_, err := BuildPackage(rootConfig, project, buildType, true, false, !project.IsApplication)
		if err != nil {
			color.Red("\nTouch Error: %v\n", err)
			os.Exit(1)
		}
	}
	if verbose {
		printSuccess("\nFinish updating CMakeLists.txt successfully\n")
	}
}
