package qtpm

import (
	"bytes"
	"fmt"
	"github.com/fatih/color"
	"io/ioutil"
	"log"
	"os"
	"path/filepath"
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
	fmt.Fprintf(&buffer, "set(%s %s)", s.Name, strings.Join(s.Files, "\n    "))
	var keys []string
	for key := range s.PlatformSpecificFiles {
		keys = append(keys, key)
	}
	sort.Strings(keys)
	for _, key := range keys {
		files := s.PlatformSpecificFiles[key]
		sort.Strings(files)
		fmt.Fprintf(&buffer, "\nif(%s)\n", key)
		fmt.Fprintf(&buffer, "  list(APPEND %s %s)\n", s.Name, strings.Join(files, "\n      "))
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
	path = filepath.ToSlash(path)
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

type FilterType int

const (
	AllSource FilterType = iota
	NoHeaderFile
)

type Require struct {
	PackageName string
	LibName     string
}

type ProjectDetail struct {
	Target              string
	Requires            []Require
	QtModules           []string
	AllDependencies     []string
	Sources             *SourceBundle
	InstallHeaderDirs   map[string]*SourceBundle
	Tests               *SourceBundle
	ExtraTestSources    *SourceBundle
	Examples            *SourceBundle
	ExtraExampleSources *SourceBundle
	Resources           *SourceBundle
	HasQtResource       bool
	SubDir              bool
	BuildNumber         int
	config              *PackageConfig
}

func (sv ProjectDetail) TargetSmall() string {
	return strings.ToLower(sv.Target)
}

func (sv ProjectDetail) TargetLarge() string {
	return strings.ToUpper(sv.Target)
}

func (sv ProjectDetail) TargetLibName() string {
	return libname(sv.Target)
}

func (sv ProjectDetail) RequireLibs() string {
	result := make([]string, len(sv.Requires))
	for i, require := range sv.Requires {
		result[i] = require.LibName
	}
	return strings.Join(result, " ")
}

func (sv ProjectDetail) AuthorName() string {
	if sv.config.Author == "" {
		return sv.config.Organization
	}
	return sv.config.Author
}

func (sv ProjectDetail) Description() string {
	return sv.config.Description
}

func (sv ProjectDetail) LicenseName() string {
	return sv.config.License
}

func (sv ProjectDetail) Organization() string {
	if sv.config.Organization == "" {
		return sv.config.Author
	}
	return sv.config.Organization
}

func (sv ProjectDetail) CopyRight() string {
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

func (sv ProjectDetail) VersionMajor() int {
	return sv.config.Version[0]
}

func (sv ProjectDetail) VersionMinor() int {
	return sv.config.Version[1]
}

func (sv ProjectDetail) VersionPatch() int {
	return sv.config.Version[2]
}

func (sv ProjectDetail) Version() string {
	return fmt.Sprintf("%d.%d.%d", sv.VersionMajor(), sv.VersionMinor(), sv.VersionPatch())
}

func (sv ProjectDetail) ShortVersion() string {
	return fmt.Sprintf("%d.%d", sv.VersionMajor(), sv.VersionMinor())
}

type InstallHeaderDir struct {
	TargetDir string
	Files     *SourceBundle
}

func (sv ProjectDetail) InstallHeaders() []*InstallHeaderDir {
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

var ignoreResources = map[string]bool{
	"icon.png":           true,
	"WindowsAppIcon.ico": true,
	"MacOSXAppIcon.icns": true,
	"windows.rc":         true,
}

func (sv *ProjectDetail) SearchFiles() {
	dir := sv.config.Dir
	sv.Sources = NewSourceBundle("sources", false)
	sv.InstallHeaderDirs = make(map[string]*SourceBundle)
	sv.Tests = NewSourceBundle("tests", true)
	sv.ExtraTestSources = NewSourceBundle("extra_test_sources", false)
	sv.Examples = NewSourceBundle("examples", false)
	sv.ExtraExampleSources = NewSourceBundle("extra_example_sources", false)
	sv.Resources = NewSourceBundle("resources", false)

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
		if path == "main.cpp" {
			return nil
		}
		outputPath := "src/" + path
		dir := filepath.Dir(path)
		if dir == "." {
			dir = ""
		}
		ext := filepath.Ext(path)
		if supportedSourceExtensions[ext] || ext == ".ui" {
			sv.Sources.addfile(outputPath)
		} else if supportedHeaderExtensions[ext] {
			_, ok := sv.InstallHeaderDirs[dir]
			if ok {
				sv.InstallHeaderDirs[dir].addfile(outputPath)
			} else {
				sv.Sources.addfile(outputPath)
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

	examples, err := ioutil.ReadDir(filepath.Join(dir, "examples"))
	if err == nil {
		for _, example := range examples {
			name := example.Name()
			ext := filepath.Ext(name)
			if strings.HasPrefix(name, "_") || (!supportedSourceExtensions[ext] && ext != ".ui") {
				continue
			}
			if strings.HasSuffix(name, "_example.cpp") || name == "example.cpp" {
				sv.Examples.addfile("examples/" + name)
			} else {
				sv.ExtraExampleSources.addfile("examples/" + name)
			}
		}
	}

	resourceDir := filepath.Join(dir, "Resources")
	filepath.Walk(resourceDir, func(fullPath string, info os.FileInfo, err error) error {
		if err != nil {
			return err
		}
		if info.IsDir() || strings.HasPrefix(info.Name(), "_") || ignoreResources[info.Name()] {
			return nil
		}
		sv.Resources.addfile("Resources/" + fullPath[len(resourceDir)+1:])
		return nil
	})
}

func (pd *ProjectDetail) AllFiles(isApplication bool, filter FilterType) []string {
	var sourceFiles []string
	sourceFiles = append(sourceFiles, pd.Sources.Files...)
	for _, files := range pd.Sources.PlatformSpecificFiles {
		sourceFiles = append(sourceFiles, files...)
	}
	if isApplication {
		sourceFiles = append(sourceFiles, "src/main.cpp")
	}
	sourceFiles = append(sourceFiles, pd.ExtraTestSources.Files...)
	for _, files := range pd.ExtraTestSources.PlatformSpecificFiles {
		sourceFiles = append(sourceFiles, files...)
	}
	sourceFiles = append(sourceFiles, pd.Tests.Files...)
	for _, files := range pd.Tests.PlatformSpecificFiles {
		sourceFiles = append(sourceFiles, files...)
	}
	sourceFiles = append(sourceFiles, pd.Examples.Files...)
	for _, files := range pd.Examples.PlatformSpecificFiles {
		sourceFiles = append(sourceFiles, files...)
	}
	sourceFiles = append(sourceFiles, pd.ExtraExampleSources.Files...)
	for _, files := range pd.ExtraExampleSources.PlatformSpecificFiles {
		sourceFiles = append(sourceFiles, files...)
	}
	if filter == NoHeaderFile {
		var tmpResult []string
		for _, source := range sourceFiles {
			if supportedSourceExtensions[filepath.Ext(source)] {
				tmpResult = append(tmpResult, source)
			}
		}
		sourceFiles = tmpResult
	}
	sort.Strings(sourceFiles)
	return sourceFiles
}

func AddTest(config *PackageConfig, name string) {
	dirName, className, _ := ParseName(name)
	if dirName != "" {
		dirName = dirName + "/"
	}
	variable := map[string]interface{}{
		"ClassName":      className,
		"ClassNameSmall": strings.ToLower(className),
		"Dir":            dirName,
	}
	WriteTemplate(config.Dir, "test", strings.ToLower(className)+"_test.cpp", "testclass.cpp", variable, false)
}

func AddClass(config *PackageConfig, name string) {
	dirName, className, parent := ParseName(name)
	if parent == "" {
		parent = "QObject"
	}
	dir := filepath.Join(append([]string{"src"}, strings.Split(dirName, "/")...)...)
	os.MkdirAll(filepath.Join(config.Dir, dir), 0755)
	if dirName != "" {
		dirName = dirName + "/"
	}
	variable := map[string]interface{}{
		"Parent":         parent,
		"ClassName":      className,
		"ClassNameSmall": strings.ToLower(className),
	}
	WriteTemplate(config.Dir, dir, strings.ToLower(className)+".h", "classsource.h", variable, false)
	WriteTemplate(config.Dir, dir, strings.ToLower(className)+".cpp", "classsource.cpp", variable, false)
}

func CreateResource(rootPackageDir string) bool {
	resourceDir := filepath.Join(rootPackageDir, "qtresources")
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
	WriteTemplate(rootPackageDir, "qtresources", "resource.qrc", "resource.qrc", result, true)
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

func AddDotGitIgnore(config *PackageConfig) {
	WriteTemplate(config.Dir, "", ".gitignore", "dotgitignore", nil, false)
}

func RequiredQtModules(config *PackageConfig, dependencies []*PackageConfig) []string {
	var modules []string
	modules = append(modules, config.QtModules...)
	for _, dependency := range dependencies {
		modules = append(modules, dependency.QtModules...)
	}
	return CleanList(modules)
}

func FilterDependencies(config *PackageConfig, allDependencies []*PackageConfig) []*PackageConfig {
	allPackages := make(map[string]*PackageConfig)
	for _, dependency := range allDependencies {
		allPackages[dependency.PackageName] = dependency
	}
	waitingList := config.Requires
	readPackageNames := make(map[string]bool)
	for _, packageName := range waitingList {
		readPackageNames[packageName] = true
	}
	for len(waitingList) > 0 {
		var nextWaitingList []string
		for _, packageName := range waitingList {
			dependency := allPackages[packageName]
			for _, nextPackageName := range dependency.Requires {
				if !readPackageNames[nextPackageName] {
					nextWaitingList = append(nextWaitingList, nextPackageName)
					readPackageNames[nextPackageName] = true
				}
			}
		}
		waitingList = nextWaitingList
	}

	var filteredDependencies []*PackageConfig
	for _, dependency := range allDependencies {
		if readPackageNames[dependency.PackageName] {
			filteredDependencies = append(filteredDependencies, dependency)
		}
	}
	return filteredDependencies
}

func DependenciesLibs(config *PackageConfig, dependencies []*PackageConfig) []Require {
	var requires []Require
	for _, dependency := range dependencies {
		packageNames := strings.Split(dependency.PackageName, "/")
		requires = append(requires, Require{
			PackageName: strings.Join(packageNames, "___"),
			LibName:     libname(packageNames[2]),
		})
	}
	return requires
}

func AddCMakeForApp(config *PackageConfig, rootPackageDir string, dependencies []*PackageConfig, refresh, debugBuild bool) (bool, *ProjectDetail, error) {
	var destinationPath string
	if config.Dir == rootPackageDir {
		destinationPath = ""
	} else {
		destinationPath, _ = filepath.Rel(config.Dir, rootPackageDir)
		destinationPath += "/"
	}
	filteredDependencies := FilterDependencies(config, dependencies)
	detail := &ProjectDetail{
		config:    config,
		Target:    CleanName(config.Name),
		QtModules: RequiredQtModules(config, filteredDependencies),
		Requires:  DependenciesLibs(config, filteredDependencies),
	}
	detail.SearchFiles()
	detail.HasQtResource = CreateResource(config.Dir)

	WriteTemplate(config.Dir, "Resources", "windows.rc", "windows.rc", detail, !refresh)
	_, err := os.Stat(filepath.Join(config.Dir, BuildFolder(debugBuild)))
	changed, err2 := WriteTemplate(config.Dir, "", "CMakeLists.txt", "CMakeListsApp.txt", detail, !refresh)
	return changed || os.IsNotExist(err), detail, err2
}

func AddCMakeForLib(config *PackageConfig, rootPackageDir string, dependencies []*PackageConfig, refresh, debugBuild bool) (bool, *ProjectDetail, error) {
	var destinationPath string
	if config.Dir == rootPackageDir {
		destinationPath = ""
	} else {
		destinationPath, _ = filepath.Rel(config.Dir, rootPackageDir)
		destinationPath += "/"
	}
	filteredDependencies := FilterDependencies(config, dependencies)
	detail := &ProjectDetail{
		config:    config,
		Target:    CleanName(config.Name),
		QtModules: RequiredQtModules(config, filteredDependencies),
		Requires:  DependenciesLibs(config, filteredDependencies),
	}
	detail.SearchFiles()
	detail.HasQtResource = CreateResource(config.Dir)

	_, err := os.Stat(filepath.Join(config.Dir, BuildFolder(debugBuild)))
	changed, err2 := WriteTemplate(config.Dir, "", "CMakeLists.txt", "CMakeListsLib.txt", detail, !refresh)
	return changed || os.IsNotExist(err), detail, err2
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
