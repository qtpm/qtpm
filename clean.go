package qtpm

import (
	"fmt"
	"log"
	"os"
	"path/filepath"
)

func Clean() {
	config, err := LoadConfig(".", true)
	if err != nil {
		log.Fatalln(err)
	}
	dir := config.Dir
	targets := []string{
		"CMakeLists.txt",
		"build-debug",
		"build-release",
	}
	for _, target := range targets {
		path := filepath.Join(dir, target)
		_, err := os.Stat(path)
		if os.IsNotExist(err) {
			continue
		}
		fmt.Printf("%s ... ", target)
		err = os.RemoveAll(path)
		if err != nil {
			fmt.Printf("error occured: %v\n", err)
		} else {
			fmt.Println("removed")
		}
	}
	fmt.Println("\nRun qtpm get to recover vender header files/libraries.")
}
