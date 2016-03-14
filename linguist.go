package main

import (
	"fmt"
	"io/ioutil"
	"log"
	"os"
	"os/exec"
	"path/filepath"
	"runtime"
	"strings"
)

func LinguistAdd(language string) {
	config, err := LoadConfig(".", true)
	if err != nil {
		log.Fatalln(err)
	}
	tsPath := filepath.Join(config.Dir, "translations", fmt.Sprintf("%s_%s.ts", strings.ToLower(config.Name), language))
	_, err = os.Stat(tsPath)
	if !os.IsNotExist(err) {
		log.Fatalf("%s already exists in translations folder\n", fmt.Sprintf("%s_%s.ts", strings.ToLower(config.Name), language))
	}
	var command string
	qtdir := FindQt(config.Dir)
	if qtdir == "" {
		command = "lupdate"
	} else if runtime.GOOS == "windows" {
		command = filepath.Join(qtdir, "bin", "lupdate.exe")
	} else {
		command = filepath.Join(qtdir, "bin", "lupdate")
	}
	os.MkdirAll(filepath.Join(config.Dir, "translations"), 0755)
	srcPath := filepath.Join(config.Dir, "src")
	cmd := exec.Command(command, "-recursive", "-locations", "relative", "-target-language", language, srcPath, "-ts", tsPath)
	out, err := cmd.CombinedOutput()
	log.Println(string(out))
}

func LinguistUpdate() {
	config, err := LoadConfig(".", true)
	if err != nil {
		log.Fatalln(err)
	}
	entries, _ := ioutil.ReadDir(filepath.Join(config.Dir, "translations"))
	for _, entry := range entries {
		if entry.IsDir() || filepath.Ext(entry.Name()) != ".ts" {
			continue
			name := entry.Name()[:len(entry.Name())-3]
			fragments := strings.Split(name, "_")
			lang := fragments[len(fragments)-1]
			LinguistAdd(lang)
		}
	}
}

func LinguistEdit(language string) {
	config, err := LoadConfig(".", true)
	if err != nil {
		log.Fatalln(err)
	}
	var tsPath string
	if language == "" {
		dirs, _ := ioutil.ReadDir(filepath.Join(config.Dir, "translations"))
		if len(dirs) == 0 {
			log.Fatalln("No .ts files in translations folder")
		}
		tsPath = filepath.Join(config.Dir, "translations", dirs[0].Name())
	} else {
		tsPath = filepath.Join(config.Dir, "translations", strings.ToLower(config.Name)+"_"+language+".ts")
		_, err = os.Stat(tsPath)
		if os.IsNotExist(err) {
			LinguistAdd(language)
		}
	}
	qtdir := FindQt(config.Dir)
	var command string
	if qtdir == "" {
		command = "Linguist"
	} else {
		command = filepath.Join(qtdir, "bin", "Linguist")
	}
	var args []string
	switch runtime.GOOS {
	case "windows":
		command = command + ".exe"
	case "darwin":
		args = append(args, "-a", command+".app")
		command = "open"
	}
	args = append(args, tsPath)
	cmd := exec.Command(command, args...)
	cmd.Run()
}

func ReleaseTranslation(rootPackageDir, srcDir string) error {
	qtdir := FindQt(rootPackageDir)
	os.MkdirAll(filepath.Join(rootPackageDir, "resources", "translations"), 0755)
	files, _ := ioutil.ReadDir(filepath.Join(srcDir, "translations"))
	var command string
	if qtdir == "" {
		command = "lrelease"
	} else if runtime.GOOS == "windows" {
		command = filepath.Join(qtdir, "bin", "lrelease.exe")
	} else {
		command = filepath.Join(qtdir, "bin", "lrelease")
	}
	for _, file := range files {
		if file.IsDir() || filepath.Ext(file.Name()) != ".ts" {
			continue
		}
		srcPath := filepath.Join(srcDir, "translations", file.Name())
		destPath := filepath.Join(rootPackageDir, "resources", "translations", file.Name()[:len(file.Name())-3]+".qm")
		cmd := exec.Command(command, srcPath, "-qm", destPath)
		out, err := cmd.CombinedOutput()
		log.Println(string(out))
		if err != nil {
			println(command, srcPath, destPath)
			println("translate error:", err.Error())
			return err
		}
	}
	return nil
}
