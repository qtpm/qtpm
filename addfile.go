package qtpm

import (
	"bytes"
	"fmt"
	"github.com/fatih/color"
	"io/ioutil"
	"log"
	"os"
	"path/filepath"
	"regexp"
	"sort"
	"strings"
	"text/template"
	"time"
)

type SourceBundle struct {
	Name                  string
	Files                 []string
	Test                  bool
	PlatformSpecificFiles map[string][]string
}

func NewSourceBundle(name string, test bool) *SourceBundle {
	return &SourceBundle{
		Name: name,
		Test: test,
		PlatformSpecificFiles: make(map[string][]string),
	}
}

func (s SourceBundle) HasItem() bool {
	return len(s.Files) > 0 || len(s.PlatformSpecificFiles) > 0
}

func (s SourceBundle) DefineList() string {
	if !s.HasItem() {
		return ""
	}
	var buffer bytes.Buffer
	sort.Strings(s.Files)
	fmt.Fprintf(&buffer, "set(%s %s)", s.Name, strings.Join(s.Files, " "))
	var keys []string
	for key := range s.PlatformSpecificFiles {
		keys = append(keys, key)
	}
	sort.Strings(keys)
	for _, key := range keys {
		files := s.PlatformSpecificFiles[key]
		sort.Strings(files)
		fmt.Fprintf(&buffer, "\nif(%s)\n", key)
		fmt.Fprintf(&buffer, "  list(APPEND %s %s)\n", s.Name, strings.Join(files, " "))
		fmt.Fprintf(&buffer, "endif(%s)", key)
	}
	return buffer.String()
}

func (s SourceBundle) StartLoop() string {
	return fmt.Sprintf("foreach(file IN LISTS %s)", s.Name)
}

func (s SourceBundle) EndLoop() string {
	return "endforeach()\n"
}

var platformNames = map[string]string{
	"windows": "WIN32",
	"darwin":  "APPLE",
	"unix":    "UNIX",
	"linux":   "UNIX AND NOT APPLE AND NOT CYGWIN",
	"mingw":   "MINGW",
	"msys":    "MSYS",
	"cygwin":  "CYGWIN",
	"msvc":    "MSVC",
}

func (s *SourceBundle) addfile(path string) {
	_, name := filepath.Split(path)
	basename := name[:len(name)-len(filepath.Ext(name))]
	if s.Test {
		basename = basename[:len(name)-5]
	}
	fragments := strings.Split(basename, "_")
	last := fragments[len(fragments)-1]
	if platform, ok := platformNames[last]; ok {
		s.PlatformSpecificFiles[platform] = append(s.PlatformSpecificFiles[platform], path)
	} else {
		s.Files = append(s.Files, path)
	}
}

type SourceVariable struct {
	Target            string
	Parent            string
	VendorPath        string
	Requires          []string
	QtModules         []string
	Sources           *SourceBundle
	InstallHeaderDirs map[string]*SourceBundle
	Tests             *SourceBundle
	ExtraTestSources  *SourceBundle
	Examples          *SourceBundle
	HasResource       bool
	IsLibrary         bool
	Debug             bool
	BuildNumber       int
	config            *PackageConfig
}

func (sv SourceVariable) TargetSmall() string {
	return strings.ToLower(sv.Target)
}

func (sv SourceVariable) TargetLarge() string {
	return strings.ToUpper(sv.Target)
}

func (sv SourceVariable) TargetLibName() string {
	name := strings.ToLower(sv.Target)
	return strings.TrimPrefix(strings.TrimSuffix(name, "lib"), "lib")
}

func (sv SourceVariable) RequireLibs() []string {
	result := make([]string, len(sv.Requires))
	for i, require := range sv.Requires {
		result[i] = strings.TrimPrefix(strings.TrimSuffix(require, "lib"), "lib")
	}
	return result
}

func (sv SourceVariable) AuthorName() string {
	if sv.config.Author == "" {
		return sv.config.Organization
	}
	return sv.config.Author
}

func (sv SourceVariable) LicenseName() string {
	return sv.config.License
}

func (sv SourceVariable) Organization() string {
	return sv.config.Organization
}

func (sv SourceVariable) CopyRight() string {
	year := time.Now().Year()
	startYear := sv.config.ProjectStartYear
	if startYear == 0 {
		panic("start year is empty")
	}
	var names []string
	if sv.config.Organization != "" {
		names = append(names, sv.config.Organization)
	}
	if sv.config.Author != "" {
		names = append(names, sv.config.Author)
	}
	if year == startYear {
		return fmt.Sprintf("%d %s", year, strings.Join(names, " "))
	}
	return fmt.Sprintf("%d-%d %s", startYear, year, strings.Join(names, " "))
}

func (sv SourceVariable) VersionMajor() int {
	return sv.config.Version[0]
}

func (sv SourceVariable) VersionMinor() int {
	return sv.config.Version[1]
}

func (sv SourceVariable) VersionPatch() int {
	return sv.config.Version[2]
}

func (sv SourceVariable) Version() string {
	return fmt.Sprintf("%d.%d.%d", sv.VersionMajor(), sv.VersionMinor(), sv.VersionPatch())
}

func (sv SourceVariable) ShortVersion() string {
	return fmt.Sprintf("%d.%d", sv.VersionMajor(), sv.VersionMinor())
}

type InstallHeaderDir struct {
	TargetDir string
	Files     *SourceBundle
}

func (sv SourceVariable) InstallHeaders() []*InstallHeaderDir {
	var keys []string
	for key := range sv.InstallHeaderDirs {
		keys = append(keys, key)
	}
	sort.Strings(keys)
	var result []*InstallHeaderDir
	for key, value := range sv.InstallHeaderDirs {
		result = append(result, &InstallHeaderDir{
			TargetDir: key,
			Files:     value,
		})
	}
	return result
}

var supportedSourceExtensions = map[string]bool{
	".cpp": true,
	".c":   true,
	".cxx": true,
}

var supportedHeaderExtensions = map[string]bool{
	".h":   true,
	".hpp": true,
	".h++": true,
}

func (sv *SourceVariable) SearchFiles() {
	dir := sv.config.Dir
	sv.Sources = NewSourceBundle("sources", false)
	sv.InstallHeaderDirs = make(map[string]*SourceBundle)
	sv.Tests = NewSourceBundle("tests", true)
	sv.ExtraTestSources = NewSourceBundle("extra_test_sources", false)
	sv.Examples = NewSourceBundle("examples", false)

	for _, extraDir := range sv.config.ExtraInstallDirs {
		dirName := strings.Replace(extraDir, "/", "__", -1)
		varName := strings.Join([]string{"public_headers", dirName}, "__")
		sv.InstallHeaderDirs[extraDir] = NewSourceBundle(varName, false)
	}
	if _, ok := sv.InstallHeaderDirs[""]; !ok {
		sv.InstallHeaderDirs[""] = NewSourceBundle("public_headers", false)
	}

	srcDir := filepath.Join(dir, "src")
	filepath.Walk(srcDir, func(fullPath string, info os.FileInfo, err error) error {
		if err != nil {
			return err
		}
		if info.IsDir() || strings.HasPrefix(info.Name(), "_") {
			return nil
		}
		path := fullPath[len(srcDir)+1:]
		outputPath := "src/" + path
		dir := filepath.Dir(path)
		if dir == "." {
			dir = ""
		}
		if supportedSourceExtensions[filepath.Ext(path)] && path != "main.cpp" {
			sv.Sources.addfile(outputPath)
		} else {
			_, ok := sv.InstallHeaderDirs[dir]
			if supportedHeaderExtensions[filepath.Ext(path)] && ok {
				sv.InstallHeaderDirs[dir].addfile(outputPath)
			}
		}
		return nil
	})

	tests, err := ioutil.ReadDir(filepath.Join(dir, "test"))
	if err == nil {
		for _, test := range tests {
			name := test.Name()
			if strings.HasPrefix(name, "_") || !supportedSourceExtensions[filepath.Ext(name)] {
				continue
			}
			if strings.HasSuffix(name, "_test.cpp") {
				sv.Tests.addfile("test/" + name)
			} else {
				sv.ExtraTestSources.addfile("test/" + name)
			}
		}
	}

	exampleDir := filepath.Join(dir, "examples")
	filepath.Walk(exampleDir, func(fullPath string, info os.FileInfo, err error) error {
		if err != nil {
			return err
		}
		if info.IsDir() || strings.HasPrefix(info.Name(), "_") {
			return nil
		}
		if supportedSourceExtensions[filepath.Ext(fullPath)] {
			sv.Examples.addfile("examples/" + fullPath[len(exampleDir)+1:])
		}
		return nil
	})
}

func AddTest(basePath, name string) {
	name, _ = ParseName(name)
	variable := &SourceVariable{
		Target: name,
	}
	WriteTemplate(basePath, "test", strings.ToLower(name)+"_test.cpp", "testclass.cpp", variable, false)
}

func AddClass(basePath, name string, isLibrary bool) {
	className, parent := ParseName(name)
	if parent == "" {
		parent = "QObject"
	}
	variable := &SourceVariable{
		Target:    className,
		Parent:    parent,
		IsLibrary: isLibrary,
	}
	WriteTemplate(basePath, "src", strings.ToLower(className)+".h", "classsource.h", variable, false)
	WriteTemplate(basePath, "src", strings.ToLower(className)+".cpp", "classsource.cpp", variable, false)
}

func CreateResource(rootPackageDir string) bool {
	resourceDir := filepath.Join(rootPackageDir, "resources")
	os.MkdirAll(resourceDir, 0755)
	var result []string
	filepath.Walk(resourceDir, func(fullPath string, info os.FileInfo, err error) error {
		if info.IsDir() || strings.HasPrefix(info.Name(), "_") {
			return nil
		}
		path := fullPath[len(resourceDir)+1:]
		if !strings.HasSuffix(path, ".qrc") && path != "icon.png" && info.Name() != "Thumbs.db" && info.Name() != ".DS_Store" {
			result = append(result, path)
		}
		return nil
	})
	if len(result) == 0 {
		return false
	}
	WriteTemplate(rootPackageDir, "resources", "resource.qrc", "resource.qrc", result, false)
	return true
}

func AddLicense(config *PackageConfig, name string) {
	licenseKey, licenseName, err := NormalizeLicense(name)
	if err != nil {
		log.Fatalln(err)
	}
	config.License = licenseName
	WriteLicense(config.Dir, licenseKey)
	config.Save()
}

func AddCMakeForApp(config *PackageConfig, rootPackageDir string, refresh, debugBuild bool) (bool, error) {
	var vendorPath string
	if config.Dir == rootPackageDir {
		vendorPath = "vendor"
	} else {
		vendorPath, _ = filepath.Rel(config.Dir, filepath.Join(rootPackageDir, "vendor"))
	}

	variable := &SourceVariable{
		config:     config,
		VendorPath: vendorPath,
		Target:     CleanName(config.Name),
		QtModules:  CleanList(config.QtModules),
	}
	for _, require := range config.Requires {
		packageNames := strings.Split(require, "/")
		if len(packageNames) != 3 {
			continue
		}
		variable.Requires = append(variable.Requires, packageNames[2])
	}
	variable.SearchFiles()
	variable.HasResource = CreateResource(config.Dir)

	sort.Strings(variable.QtModules)
	sort.Strings(variable.Requires)
	WriteTemplate(config.Dir, BuildFolder(false), "windows.rc", "windows.rc", variable, !refresh)
	WriteTemplate(config.Dir, BuildFolder(true), "windows.rc", "windows.rc", variable, !refresh)
	_, err := os.Stat(filepath.Join(config.Dir, BuildFolder(debugBuild)))
	changed, err2 := WriteTemplate(config.Dir, "", "CMakeLists.txt", "CMakeListsApp.txt", variable, !refresh)
	return changed || os.IsNotExist(err), err2
}

func AddCMakeForLib(config *PackageConfig, rootPackageDir string, refresh, debugBuild bool) (bool, error) {
	var vendorPath string
	if config.Dir == rootPackageDir {
		vendorPath = "vendor"
	} else {
		vendorPath, _ = filepath.Rel(config.Dir, filepath.Join(rootPackageDir, "vendor"))
	}

	variable := &SourceVariable{
		config:     config,
		VendorPath: vendorPath,
		Target:     CleanName(config.Name),
		QtModules:  CleanList(config.QtModules),
	}
	for _, require := range config.Requires {
		packageNames := strings.Split(require, "/")
		if len(packageNames) != 3 {
			continue
		}
		variable.Requires = append(variable.Requires, packageNames[2])
	}
	variable.SearchFiles()
	sort.Strings(variable.QtModules)
	sort.Strings(variable.Requires)

	_, err := os.Stat(filepath.Join(config.Dir, BuildFolder(debugBuild)))
	changed, err2 := WriteTemplate(config.Dir, "", "CMakeLists.txt", "CMakeListsLib.txt", variable, !refresh)
	return changed || os.IsNotExist(err), err2
}

func WriteTemplate(basePath, dir, fileName, templateName string, variable interface{}, checkFileChange bool) (bool, error) {
	var filePath string
	var err error
	if dir == "" {
		filePath, err = filepath.Abs(filepath.Join(basePath, fileName))
	} else {
		filePath, err = filepath.Abs(filepath.Join(basePath, dir, fileName))
	}
	if err != nil {
		return false, err
	}
	os.MkdirAll(filepath.Dir(filePath), 0755)
	var buffer bytes.Buffer
	src := MustAsset("templates/" + templateName)
	tmp := template.Must(template.New(templateName).Delims("[[", "]]").Parse(string(src)))
	err = tmp.Execute(&buffer, variable)
	if err != nil {
		panic(err)
	}
	if checkFileChange {
		existingContent, err := ioutil.ReadFile(filePath)
		if err == nil && bytes.Compare(existingContent, buffer.Bytes()) == 0 {
			return false, nil
		}
	}
	err = ioutil.WriteFile(filePath, buffer.Bytes(), 0644)
	if err != nil {
		color.Red("Write file error: %s\n", filepath.Join(dir, fileName))
	} else {
		color.Magenta("Wrote: %s\n", filepath.Join(dir, fileName))
	}
	return true, err
}

func ParseName(name string) (string, string) {
	names := strings.Split(name, "@")
	className := names[0]
	if strings.HasPrefix(className, "Test") {
		className = className[4:]
	} else if className == "" {
		path, _ := filepath.Abs(".")
		_, className = filepath.Split(path)
	}
	className = strings.ToUpper(className[:1]) + className[1:]

	var parentName string
	if len(names) == 2 {
		parentName = strings.ToUpper(names[1][:2]) + names[1][2:]
	}

	return CleanName(className), CleanName(parentName)
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
	return result
}
