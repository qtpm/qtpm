package main

import (
	"bytes"
	"fmt"
	winicon "github.com/Kodeworks/golang-image-ico"
	"image"
	"io/ioutil"
	"log"
	"os"
	"os/exec"
	"path/filepath"
	"runtime"
	"strings"
)

func Build(refresh, debugBuild bool) {
	config, err := LoadConfig(".", true)
	if err != nil {
		log.Fatalln(err)
	}
	err = BuildPackage(config.Dir, config, refresh, debugBuild, !config.IsApplication)
	if err != nil {
		log.Fatalln(err)
	}
}

func Test(refresh bool) {
	config, err := LoadConfig(".", true)
	if err != nil {
		log.Fatalln(err)
	}
	BuildPackage(config.Dir, config, refresh, true, false)
	buildPath := filepath.Join(config.Dir, BuildFolder(true))
	makeCmd := exec.Command("make", "test")
	makeCmd.Dir = buildPath
	makeCmd.Env = append(makeCmd.Env, "CTEST_OUTPUT_ON_FAILURE=1")
	out, err := makeCmd.CombinedOutput()
	log.Println(string(out))
	if err != nil {
		log.Fatal(err)
	}
}

type sequentialRun struct {
	workDir string
	err     error
}

func SequentialRun(workDir string) *sequentialRun {
	return &sequentialRun{
		workDir: workDir,
	}
}

func (s *sequentialRun) Run(command string, args ...string) *sequentialRun {
	if s.err != nil {
		return s

	}
	cmd := exec.Command(command, args...)
	cmd.Dir = s.workDir
	err := cmd.Run()
	if err != nil {
		s.err = fmt.Errorf("cmd: `%s %s` err: %s", command, strings.Join(args, " "), err.Error())
	}
	return s
}

func (s *sequentialRun) Finish() error {
	return s.err
}

func BuildPackage(rootPackageDir string, config *PackageConfig, refresh, debugBuild, install bool) error {
	var vendorPath string
	var changed bool
	var err error
	if config.IsApplication {
		changed, err = AddCMakeForApp(config, refresh, debugBuild)
		buildPath := filepath.Join(config.Dir, BuildFolder(debugBuild))
		os.MkdirAll(buildPath, 0755)
		CreateIcon(config.Dir, debugBuild)
	} else {
		changed, err = AddCMakeForLib(config, refresh, debugBuild)
		vendorPath = VendorFolder(rootPackageDir, debugBuild)
		os.MkdirAll(vendorPath, 0755)
	}
	if err != nil {
		return err
	}
	return RunCMakeAndBuild(rootPackageDir, config.Dir, vendorPath, changed, debugBuild, install)
}

func VendorFolder(rootPackageDir string, debugBuild bool) string {
	if debugBuild {
		return filepath.Join(rootPackageDir, "vendor", "debug")
	}
	return filepath.Join(rootPackageDir, "vendor", "release")
}

func BuildFolder(debugBuild bool) string {
	if debugBuild {
		return "build-debug"
	} else {
		return "build-release"
	}
}

func CreateIcon(rootPackageDir string, debugBuild bool) {
	var iconImage image.Image
	buildDir := filepath.Join(rootPackageDir, BuildFolder(debugBuild))
	resultPath1 := filepath.Join(buildDir, "MacOSXAppIcon.icns")
	resultPath2 := filepath.Join(buildDir, "WindowsAppIcon.ico")
	pngPath := filepath.Join(rootPackageDir, "resource", "icon.png")
	file, err := os.Open(pngPath)
	if err == nil {
		outputStat, err := os.Stat(resultPath2)
		if !os.IsNotExist(err) {
			inputStat, err := file.Stat()
			if err == nil && outputStat.ModTime().Unix() > inputStat.ModTime().Unix() {
				return
			}
		}
		iconImage, _, err = image.Decode(file)
		if err != nil {
			iconImage = nil
		}
	}
	if iconImage == nil {
		pngPath = filepath.Join(buildDir, "icon.png")
		ioutil.WriteFile(pngPath, MustAsset("resources/qt-logo.png"), 0644)
		iconImage, _, err = image.Decode(bytes.NewReader(MustAsset("resources/qt-logo.png")))
		if err != nil {
			panic(err) // qtpm should be able to read fallback icon anytime
		}
	}

	icon, err := os.Create(resultPath2)
	defer icon.Close()
	if err != nil {
		panic(err)
	}
	winicon.Encode(icon, iconImage)

	os.MkdirAll(filepath.Join(buildDir, "MacOSXAppIcon.iconset"), 0755)
	err = SequentialRun(buildDir).
		Run("sips", "-z", "16", "16", pngPath, "--out", filepath.Join(buildDir, "MacOSXAppIcon.iconset", "icon_16x16.png")).
		Run("sips", "-z", "32", "32", pngPath, "--out", filepath.Join(buildDir, "MacOSXAppIcon.iconset", "icon_16x16@2x.png")).
		Run("sips", "-z", "32", "32", pngPath, "--out", filepath.Join(buildDir, "MacOSXAppIcon.iconset", "icon_32x32.png")).
		Run("sips", "-z", "64", "64", pngPath, "--out", filepath.Join(buildDir, "MacOSXAppIcon.iconset", "icon_32x32@2x.png")).
		Run("sips", "-z", "128", "128", pngPath, "--out", filepath.Join(buildDir, "MacOSXAppIcon.iconset", "icon_128x128.png")).
		Run("sips", "-z", "256", "256", pngPath, "--out", filepath.Join(buildDir, "MacOSXAppIcon.iconset", "icon_128x128@2x.png")).
		Run("sips", "-z", "256", "256", pngPath, "--out", filepath.Join(buildDir, "MacOSXAppIcon.iconset", "icon_256x256.png")).
		Run("sips", "-z", "512", "512", pngPath, "--out", filepath.Join(buildDir, "MacOSXAppIcon.iconset", "icon_256x256@2x.png")).
		Run("sips", "-z", "512", "512", pngPath, "--out", filepath.Join(buildDir, "MacOSXAppIcon.iconset", "icon_512x512.png")).
		Run("sips", "-z", "1024", "1024", pngPath, "--out", filepath.Join(buildDir, "MacOSXAppIcon.iconset", "icon_512x512@2x.png")).
		Run("iconutil", "-c", "icns", "--output", resultPath1, filepath.Join(buildDir, "MacOSXAppIcon.iconset")).Finish()
	if err != nil {
		panic(err)
	}
}

func RunCMakeAndBuild(rootPackageDir, packageDir, vendorPath string, update, debugBuild, install bool) error {
	buildPath := filepath.Join(packageDir, BuildFolder(debugBuild))
	if update {
		os.MkdirAll(buildPath, 0755)
		var cmd *exec.Cmd
		if vendorPath == "" {
			cmd = exec.Command("cmake", "..")
		} else {
			cmd = exec.Command("cmake", "..", "-DCMAKE_INSTALL_PREFIX="+vendorPath)
		}
		cmd.Dir = buildPath
		qtDir := FindQt(rootPackageDir)
		cmd.Env = append(cmd.Env,
			"QTPM_INCLUDE_PATH="+filepath.Join(VendorFolder(rootPackageDir, debugBuild), "include"),
			"QTPM_LIBRARY_PATH="+filepath.Join(VendorFolder(rootPackageDir, debugBuild), "lib"),
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

func CreateApplicationBundleFiles() {

}
