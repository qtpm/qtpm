package qtpm

import (
	"bytes"
	"fmt"
	"github.com/fatih/color"
	"io/ioutil"
	"os"
	"sort"
)

func Format() {
	config, err := LoadConfig(".", true)
	if err != nil {
		color.Red("%s\n", err.Error())
		os.Exit(1)
	}
	variable := &SourceVariable{
		config: config,
	}
	variable.SearchFiles()
	var sourceFiles []string
	sourceFiles = append(sourceFiles, variable.Sources.Files...)
	for _, files := range variable.Sources.PlatformSpecificFiles {
		sourceFiles = append(sourceFiles, files...)
	}
	if config.IsApplication {
		sourceFiles = append(sourceFiles, "src/main.cpp")
	}
	sourceFiles = append(sourceFiles, variable.ExtraTestSources.Files...)
	for _, files := range variable.ExtraTestSources.PlatformSpecificFiles {
		sourceFiles = append(sourceFiles, files...)
	}
	sourceFiles = append(sourceFiles, variable.Tests.Files...)
	for _, files := range variable.Tests.PlatformSpecificFiles {
		sourceFiles = append(sourceFiles, files...)
	}
	sourceFiles = append(sourceFiles, variable.Examples.Files...)
	for _, files := range variable.Examples.PlatformSpecificFiles {
		sourceFiles = append(sourceFiles, files...)
	}

	sort.Strings(sourceFiles)

	changed := 0

	if len(sourceFiles) > 0 {
		for _, sourceFile := range sourceFiles {
			before, _ := ioutil.ReadFile(sourceFile)
			cmd := Command("clang-format", config.Dir, "-i", sourceFile)
			err := cmd.Run()
			if err != nil {
				color.Red("%s\n", err.Error())
				os.Exit(1)
			}
			after, _ := ioutil.ReadFile(sourceFile)
			if !bytes.Equal(before, after) {
				changed++
				color.Magenta("%s\n", sourceFile)
			}
		}
	} else {
		color.Red("\nNo source files found\n")
		os.Exit(1)
	}
	if changed != 0 {
		fmt.Printf("\n")
	}
	printSuccess("Finish formatting source files successfully (%d/%d)\n", changed, len(sourceFiles))
}
