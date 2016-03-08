package main

import (
	"fmt"
	"github.com/BurntSushi/toml"
	"os"
	"path/filepath"
	"sort"
)

const packageFileName = "qtpackage.toml"
const userPackageFileName = "qtpackage.user.toml"

type PackageConfig struct {
	Name          string   `toml:"name"`
	Author        string   `toml:"author"`
	License       string   `toml:"license"`
	Requires      []string `toml:"requires"`
	QtModules     []string `toml:"qtmodules"`
	Version       []int    `toml:"version"`
	IsApplication bool     `toml:"-"`
	Dir           string   `toml:"-"`
}

type PackageUserConfig struct {
	QtDir string `toml:"qtdir"`
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
	return encoder.Encode(config)
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
