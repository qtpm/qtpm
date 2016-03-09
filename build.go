package main

import (
	"io/ioutil"
	"log"
	"os"
	"os/exec"
	"path/filepath"
	"runtime"
	"strings"
)

func Build(refresh bool) {
	config, err := LoadConfig(".", true)
	if err != nil {
		log.Fatalln(err)
	}
	err = BuildPackage(config.Dir, config, refresh, !config.IsApplication)
	if err != nil {
		log.Fatalln(err)
	}
}

func Test(refresh bool) {
	config, err := LoadConfig(".", true)
	if err != nil {
		log.Fatalln(err)
	}
	BuildPackage(config.Dir, config, refresh, false)
	makeCmd := exec.Command("make", "test")
	buildPath := filepath.Join(config.Dir, "build")
	makeCmd.Dir = buildPath
	makeCmd.Env = append(makeCmd.Env, "CTEST_OUTPUT_ON_FAILURE=1")
	out, err := makeCmd.CombinedOutput()
	log.Println(string(out))
	if err != nil {
		log.Fatal(err)
	}
}

func BuildPackage(rootPackageDir string, config *PackageConfig, refresh, install bool) error {
	var vendorPath string
	var changed bool
	var err error
	if config.IsApplication {
		changed, err = AddCMakeForApp(config, refresh)
	} else {
		changed, err = AddCMakeForLib(config, refresh)
		vendorPath = filepath.Join(rootPackageDir, "vendor")
	}
	if err != nil {
		return err
	}
	return RunCMakeAndBuild(rootPackageDir, config.Dir, vendorPath, changed, install)
}

func RunCMakeAndBuild(rootPackageDir, packageDir, vendorPath string, update, install bool) error {
	buildPath := filepath.Join(packageDir, "build")
	if update {
		os.MkdirAll(buildPath, 0744)
		var cmd *exec.Cmd
		if vendorPath == "" {
			cmd = exec.Command("cmake", "..")
		} else {
			cmd = exec.Command("cmake", "..", "-DCMAKE_INSTALL_PREFIX="+vendorPath)
		}
		cmd.Dir = buildPath
		qtDir := FindQt(rootPackageDir)
		cmd.Env = append(cmd.Env,
			"QTPM_INCLUDE_PATH="+filepath.Join(rootPackageDir, "vendor", "include"),
			"QTPM_LIBRARY_PATH="+filepath.Join(rootPackageDir, "vendor", "lib"),
		)
		if qtDir != "" {
			cmd.Env = append(cmd.Env, "QTDIR="+qtDir)
		}
		out, err := cmd.CombinedOutput()
		log.Println(string(out))
		if err != nil {
			return err
		}
	}
	makeCmd := exec.Command("make")
	makeCmd.Dir = buildPath
	out, err := makeCmd.CombinedOutput()
	log.Println(string(out))
	if err != nil {
		return err
	}
	if install {
		makeCmd := exec.Command("make", "install")
		makeCmd.Dir = buildPath
		out, err = makeCmd.CombinedOutput()
		log.Println(string(out))
		return err
	}
	return nil
}

func FindQt(dir string) string {
	env := os.Getenv("QTDIR")
	if env != "" {
		return env
	}
	userSetting, _ := LoadUserConfig(dir)
	if userSetting != nil && userSetting.QtDir != "" {
		return userSetting.QtDir
	}
	var paths []string
	if runtime.GOOS == "windows" {
		paths = []string{
			os.Getenv("USERPROFILE"),
			"C:\\", "D:\\",
			os.Getenv("ProgramFiles"),
			os.Getenv("ProgramFiles(x86)"),
		}
	} else {
		paths = []string{
			os.Getenv("HOME"),
			"/",
		}
	}
	for _, path := range paths {
		versions, err := ioutil.ReadDir(filepath.Join(path, "Qt"))
		if err != nil {
			continue
		}
		var biggestDir string
		for _, version := range versions {
			if strings.HasPrefix(version.Name(), "5.") {
				if version.Name() > biggestDir {
					biggestDir = version.Name()
				}
			}
		}
		if biggestDir == "" {
			continue
		}
		targets, err := ioutil.ReadDir(filepath.Join(path, "Qt", biggestDir))
		for _, target := range targets {
			name := target.Name()
			if strings.Contains(name, "ios") || strings.Contains(name, "android") || strings.Contains(name, "winphone") {
				continue
			}
			return filepath.Join(path, "Qt", biggestDir, name)
		}
	}
	return ""
}
