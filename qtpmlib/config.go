package qtpm

import (
	"fmt"
	"github.com/BurntSushi/toml"
	"github.com/fatih/color"
	"os"
	"path/filepath"
	"sort"
)

const packageFileName = "qtpackage.toml"
const userPackageFileName = "qtpackage.user.toml"

var (
	QTPMVersion       = []int{0, 8, 1}
	QTPMVersionString = fmt.Sprintf("%d.%d.%d", QTPMVersion[0], QTPMVersion[1], QTPMVersion[2])
)

type PackageConfig struct {
	Name             string                    `toml:"name"`
	Description      string                    `toml:"description"`
	Author           string                    `toml:"author"`
	Organization     string                    `toml:"organization"`
	License          string                    `toml:"license"`
	Requires         []string                  `toml:"requires"`
	QtModules        []string                  `toml:"qtmodules"`
	Version          []int                     `toml:"version"`
	ExtraInstallDirs []string                  `toml:"extra_install_dirs"`
	ProjectStartYear int                       `toml:"project_start_year"`
	QTPMVersion      []int                     `toml:"qtpm_version"`
	IsApplication    bool                      `toml:"-"`
	Dir              string                    `toml:"-"`
	PackageName      string                    `toml:"-"`
	Children         map[string]*PackageConfig `toml:"-"`
	DirtyFlag        bool                      `toml:"-"`
}

type PackageUserConfig struct {
	QtDir       string `toml:"qtdir"`
	BuildNumber int    `toml:"build_number"`
}

func MustLoadConfig(dir string, traverse bool) *PackageConfig {
	config, err := LoadConfig(".", traverse)
	if err != nil {
		color.Red("%s\n", err.Error())
		os.Exit(1)
	}
	return config
}

func LoadConfig(dir string, traverse bool) (*PackageConfig, error) {
	origDir := dir
	dir, err := filepath.Abs(dir)
	if err != nil {
		return nil, err
	}
	for {
		filePath := filepath.Join(dir, packageFileName)
		file, err := os.Open(filePath)
		if err == nil {
			config := &PackageConfig{}
			_, err := toml.DecodeReader(file, config)
			if err != nil {
				return nil, err
			}
			_, err = os.Stat(filepath.Join(dir, "src", "main.cpp"))
			config.IsApplication = !os.IsNotExist(err)
			config.Dir = dir
			switch len(config.Version) {
			case 0:
				config.Version = append(config.Version, 1, 0, 0)
			case 1:
				config.Version = append(config.Version, 0, 0)
			case 2:
				config.Version = append(config.Version, 0)
			}
			return config, nil
		}
		parent := filepath.Dir(dir)
		if dir == parent || !traverse {
			break
		}
		dir = parent
	}
	return nil, fmt.Errorf("can't find '%s' at %s", packageFileName, origDir)
}

func LoadUserConfig(dir string) (*PackageUserConfig, error) {
	dir, err := filepath.Abs(dir)
	if err != nil {
		return nil, err
	}
	filePath := filepath.Join(dir, userPackageFileName)
	file, err := os.Open(filePath)
	if err == nil {
		config := &PackageUserConfig{}
		_, err := toml.DecodeReader(file, config)
		if err != nil {
			return nil, err
		}
		return config, nil
	}
	return nil, fmt.Errorf("can't find '%s' at %s", userPackageFileName, dir)
}

func (config *PackageConfig) Save() error {
	file, err := os.Create(filepath.Join(config.Dir, packageFileName))
	if err != nil {
		return err
	}
	config.Requires = removeDuplicate(config.Requires)
	config.QtModules = removeDuplicate(config.QtModules)
	sort.Strings(config.Requires)
	sort.Strings(config.QtModules)
	encoder := toml.NewEncoder(file)
	err = encoder.Encode(config)
	if err == nil {
		color.Magenta("Update: %s\n", packageFileName)
	} else {
		color.Red("Write file error: %s - %s\n", packageFileName, err.Error())
	}
	return err
}

func (config *PackageConfig) SaveIfDirty() error {
	if config.DirtyFlag {
		config.DirtyFlag = false
		return config.Save()
	}
	return nil
}

func removeDuplicate(input []string) []string {
	exists := map[string]bool{}
	var output []string
	for _, entry := range input {
		if !exists[entry] {
			exists[entry] = true
			output = append(output, entry)
		}
	}
	return output
}

func UserName() string {
	names := []string{"LOGNAME", "USER", "USERNAME"}
	for _, name := range names {
		value := os.Getenv(name)
		if value != "" {
			return value
		}
	}
	return "(no name)"
}

func AddQtModule(config *PackageConfig, module string) {
	config.QtModules = CleanList(append(config.QtModules, module))
	config.Save()
}
