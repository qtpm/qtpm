package qtpm

import (
	"bytes"
	"fmt"
	"github.com/fatih/color"
	"io/ioutil"
	"os"
)

func Format(targetFiles []string) {
	config := MustLoadConfig(".", true)
	detail := &ProjectDetail{
		config: config,
	}
	detail.SearchFiles()
	sourceFiles := detail.AllFiles(config.IsApplication, AllSource)

	if len(sourceFiles) == 0 {
		color.Red("\nNo source files found\n")
		os.Exit(1)
	}

	pathFilter := NewPathFilter(config, targetFiles)
	changed := 0

	for _, sourceFile := range sourceFiles {
		if !pathFilter.Match(sourceFile) {
			continue
		}
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
	if changed != 0 {
		fmt.Printf("\n")
	}
	printSuccess("Finish formatting source files successfully (%d/%d)\n", changed, len(sourceFiles))
}
