package qtpm

import (
	"bytes"
	"encoding/json"
	"github.com/fatih/color"
	"io/ioutil"
	"log"
	"os"
	"os/exec"
	"path"
	"path/filepath"
	"strings"
)

type CompileCommand struct {
	Directory string `json:"directory"`
	Command   string `json:"command"`
	File      string `json:"file"`
}

func GetCompileCommand(dir string) map[string]string {
	result := make(map[string]string)
	compileCommandsRaw, err := ioutil.ReadFile(filepath.Join(dir, BuildFolder(true), "compile_commands.json"))
	if err != nil {
		log.Fatalln(err)
	}
	var compileCommandsList []CompileCommand
	json.Unmarshal(compileCommandsRaw, &compileCommandsList)
	for _, compileCommand := range compileCommandsList {
		result[compileCommand.File] = strings.SplitN(compileCommand.Command, " ", 2)[1]
	}
	return result
}

var defaultIncludeTestSrc = []byte(`// test code
int main() {
    return 0;
}
`)

func GetDefaultIncludeOptions(dir string) string {
	var buffer bytes.Buffer
	workDir := filepath.Join(dir, BuildFolder(true))
	option, err := ioutil.ReadFile(filepath.Join(workDir, "default_include_options.txt"))
	if err == nil {
		return string(option)
	}
	testCodePath := filepath.Join(workDir, "____default_include_test.cpp")
	err = ioutil.WriteFile(testCodePath, defaultIncludeTestSrc, 0644)
	if err != nil {
		log.Fatalln(err)
	}
	cmd := exec.Command("gcc", "-E", "-x", "c++", "-v", testCodePath)
	cmd.Dir = workDir
	output, err := cmd.CombinedOutput()
	if err != nil {
		log.Fatalln(err)
	}
	stateFound := false
	hasValue := false
	for _, line := range strings.Split(string(output), "\n") {
		line = strings.TrimSpace(line)
		if stateFound {
			if strings.HasPrefix(line, "End of search list") {
				break
			}
			if hasValue {
				buffer.WriteByte(' ')
			}
			if strings.Contains(line, "(framework directory)") {
				line = strings.Split(line, " ")[0]
				buffer.WriteString("-iframework")
			} else {
				buffer.WriteString("-isystem")
			}
			buffer.WriteByte(' ')
			buffer.WriteString(path.Clean(line))
			hasValue = true
		} else {
			if strings.Contains(line, "#include <...> search starts here:") {
				stateFound = true
			}
		}
	}
	ioutil.WriteFile(filepath.Join(workDir, "default_include_options.txt"), buffer.Bytes(), 0644)
	return buffer.String()
}

func BuildOptions(defaultOption, compileCommand string, path string) []string {
	args := []string{path, "--"}
	args = append(args, strings.Fields(defaultOption)...)
	return append(args, strings.Fields(compileCommand)...)
}

func Vet(targetFiles []string) {
	config, err := LoadConfig(".", true)
	if err != nil {
		color.Red("%s\n", err.Error())
		os.Exit(1)
	}
	detail, err := BuildPackage(config, config, false, true, false, false)
	if err != nil {
		log.Fatalln(err)
	}
	compileCommand := GetCompileCommand(config.Dir)
	sourceFiles := detail.AllFiles(config.IsApplication, NoHeaderFile)

	if len(sourceFiles) == 0 {
		color.Red("\nNo source files found\n")
		os.Exit(1)
	}

	pathFilter := NewPathFilter(config, targetFiles)
	defaultOption := GetDefaultIncludeOptions(config.Dir)
	var result error

	for _, source := range sourceFiles {
		if !pathFilter.Match(source) {
			continue
		}
		color.Cyan("%s\n", source)
		absPath := filepath.ToSlash(filepath.Join(config.Dir, source))
		options := BuildOptions(defaultOption, compileCommand[absPath], absPath)
		cmd := Command("clang-tidy", config.Dir, options...)
		e := cmd.Run()
		if e != nil {
			result = e
		}
	}
	if result != nil {
		log.Fatal(result)
	}
}
