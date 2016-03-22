package qtpm

import (
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
