package qtpm

import (
	"bytes"
	"fmt"
	"github.com/fatih/color"
	"os/exec"
	"path/filepath"
	"regexp"
	"sort"
	"strings"
)

func ParseName(name string) (dirName, className, parentName string) {
	if strings.Contains(name, "/") {
		names := strings.Split(name, "/")
		dirName = strings.Join(names[:len(names)-1], "/")
		name = names[len(names)-1]
	}
	if strings.Contains(name, "@") {
		names := strings.Split(name, "@")
		parentName = strings.ToUpper(names[1][:2]) + names[1][2:]
		name = names[0]
	}
	className = name
	if strings.HasPrefix(className, "Test") {
		className = className[4:]
	} else if className == "" {
		path, _ := filepath.Abs(".")
		_, className = filepath.Split(path)
	}
	className = strings.ToUpper(className[:1]) + className[1:]

	return
}

var re1 = regexp.MustCompile("[^a-zA-Z0-9_-]")
var re2 = regexp.MustCompile("[-]")

func CleanName(name string) string {
	return re2.ReplaceAllString(re1.ReplaceAllString(name, ""), "_")
}

func CleanList(modules []string) []string {
	used := make(map[string]bool)
	var result []string
	for _, module := range modules {
		if !used[module] {
			result = append(result, module)
			used[module] = true
		}
	}
	sort.Strings(result)
	return result
}

type SourcePathFilter struct {
	prefixes []string
}

func NewSourcePathFilter(config *PackageConfig, paths []string) *SourcePathFilter {
	result := &SourcePathFilter{}
	for _, path := range paths {
		absPath, _ := filepath.Abs(filepath.Join(".", path))
		result.prefixes = append(result.prefixes, filepath.ToSlash(absPath[len(config.Dir)+1:]))
	}
	sort.Strings(result.prefixes)
	return result
}

func (pf SourcePathFilter) Match(path string) bool {
	if strings.HasSuffix(path, ".ui") {
		return false
	}
	path = filepath.ToSlash(path)
	if len(pf.prefixes) == 0 {
		return true
	}
	for _, prefix := range pf.prefixes {
		if strings.HasPrefix(path, prefix) {
			return true
		}
	}
	return false
}

func libname(name string) string {
	return strings.TrimPrefix(strings.TrimSuffix(strings.ToLower(name), "lib"), "lib")
}

type sequentialRun struct {
	workDir string
	err     error
}

func SequentialRun(workDir string) *sequentialRun {
	return &sequentialRun{
		workDir: workDir,
	}
}

func (s *sequentialRun) Run(command string, args ...string) *sequentialRun {
	if s.err != nil {
		return s

	}
	cmd := Command(command, s.workDir, args...)
	cmd.Silent = !Verbose
	err := cmd.Run()
	if err != nil {
		s.err = fmt.Errorf("cmd: `%s %s` err: %s", command, strings.Join(args, " "), err.Error())
	}
	return s
}

func (s *sequentialRun) Finish() error {
	return s.err
}

var Verbose bool

func SetVerbose() {
	Verbose = true
}

func PrintCommand(cmd *exec.Cmd, envs []string) {
	var buffer bytes.Buffer
	for _, env := range envs {
		buffer.WriteString(color.BlueString(env))
		buffer.WriteByte(' ')
	}
	buffer.WriteString(color.RedString(cmd.Path))
	for _, arg := range cmd.Args[1:] {
		buffer.WriteByte(' ')
		if strings.Contains(arg, " ") {
			buffer.WriteByte('"')
			buffer.WriteString(color.YellowString(arg))
			buffer.WriteByte('"')
		} else {
			buffer.WriteString(color.YellowString(arg))
		}
	}
	buffer.WriteByte('\n')
	fmt.Println(buffer.String())
}
