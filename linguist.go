package qtpm

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

func LinguistAdd(language string, update bool) {
	config, err := LoadConfig(".", true)
	if err != nil {
		log.Fatalln(err)
	}
	tsPath := filepath.Join(config.Dir, "translations", fmt.Sprintf("%s_%s.ts", strings.ToLower(config.Name), language))
	if !update {
		_, err = os.Stat(tsPath)
		if !os.IsNotExist(err) {
			log.Fatalf("%s already exists in translations folder\n", fmt.Sprintf("%s_%s.ts", strings.ToLower(config.Name), language))
		}
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
		}
		LinguistAdd(GetLangFromTSFile(entry.Name()), true)
	}
}

func GetLangFromTSFile(filename string) string {
	name := filename[:len(filename)-3]
	fragments := strings.Split(name, "_")
	return fragments[len(fragments)-1]
}

func LinguistEdit(language string) {
	config, err := LoadConfig(".", true)
	if err != nil {
		log.Fatalln(err)
	}
	if language == "" {
		files, _ := ioutil.ReadDir(filepath.Join(config.Dir, "translations"))
		found := false
		for _, file := range files {
			if file.IsDir() || filepath.Ext(file.Name()) != ".ts" {
				continue
			}
			language = GetLangFromTSFile(file.Name())
			break
		}
		if !found {
			log.Fatalln("No .ts files in translations folder")
		}
	}
	tsPath := filepath.Join(config.Dir, "translations", strings.ToLower(config.Name)+"_"+language+".ts")
	LinguistAdd(language, true)

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
	os.MkdirAll(filepath.Join(rootPackageDir, "qtresources", "translations"), 0755)
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
		destPath := filepath.Join(rootPackageDir, "qtresources", "translations", file.Name()[:len(file.Name())-3]+".qm")
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
