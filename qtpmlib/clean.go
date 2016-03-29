package qtpm

import (
	"fmt"
	"os"
	"path/filepath"
	"strings"
)

func Clean() {
	config := MustLoadConfig(".", true)
	dir := config.Dir
	projectRoots := []string{
		"",
	}
	targets := []string{
		"CMakeLists.txt",
		"build-debug",
		"build-release",
		"dest",
	}
	filepath.Walk(filepath.Join(dir, "vendor"), func(fullPath string, info os.FileInfo, err error) error {
		if info.IsDir() {
			relPath := fullPath[len(dir)+1:]
			if strings.Count(filepath.ToSlash(relPath), "/") > 3 {
				return filepath.SkipDir
			}
			projectRoots = append(projectRoots, relPath)
		}
		return nil
	})
	for _, projectRoot := range projectRoots {
		for _, target := range targets {
			path := filepath.Join(dir, projectRoot, target)
			_, err := os.Stat(path)
			if os.IsNotExist(err) {
				continue
			}
			fmt.Printf("%s ... ", filepath.Join(projectRoot, target))
			err = os.RemoveAll(path)
			if err != nil {
				fmt.Printf("error occured: %v\n", err)
			} else {
				fmt.Println("removed")
			}
		}
	}
	fmt.Println("\nRun qtpm get to redeploy vender's header files/libraries.")
}
