package main

import (
	"fmt"
	"github.com/qtpm/qtpm/git"
	"os"
	"path/filepath"
	"strings"
)

func Get(packageName string, update bool) {
	config, _ := LoadConfig(".", true)

	if packageName == "" && config != nil {
		// load from config's require modules

	} else if packageName != "" {
		// download from git
		paths := strings.Split(packageName, "/")
		if len(paths) != 3 {
			fmt.Println("qtpm supports only github.com/author/repository style path")
			return
		}
		var parentDir string
		if config != nil {
			parentDir = config.Dir
		} else {
			parentDir, _ = filepath.Abs(".")
		}
		_, err := os.Stat(filepath.Join(parentDir, "vendor", packageName))
		installDir := filepath.Join(parentDir, "vendor", paths[0], paths[1], paths[2])
		if os.IsNotExist(err) {
			workDir := filepath.Join(parentDir, "vendor", paths[0], paths[1])
			os.MkdirAll(workDir, 0755)
			err := git.CloneWithoutFilters([]string{
				"--depth", "1", fmt.Sprintf("git@%s:%s/%s.git", paths[0], paths[1], paths[2]),
			}, workDir)
			if err != nil {
				println("err", err.Error())
			} else {
				BuildChildPackage(parentDir, installDir, true, true)
				if config != nil {
					config.Requires = append(config.Requires, packageName)
					config.Save()
				}
			}
		} else if update {
			workDir := installDir
			os.MkdirAll(workDir, 0755)
			git.Pull([]string{"--ff-only"}, workDir)
			if err != nil {
				println("err", err.Error())
			} else {
				BuildChildPackage(parentDir, installDir, true, true)
			}
		}
	} else {
		fmt.Println("qtpackage.toml or package name argument are needed")
	}
}
