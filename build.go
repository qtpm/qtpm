package qtpm

import (
	"bytes"
	"fmt"
	"github.com/Kodeworks/golang-image-ico"
	"github.com/fatih/color"
	"image"
	"io/ioutil"
	"os"
	"path/filepath"
	"strings"
)

func Build(refresh, debugBuild bool) {
	config, err := LoadConfig(".", true)
	if err != nil {
		color.Red(err.Error())
		os.Exit(1)
	}
	os.MkdirAll(filepath.Join(config.Dir, "qtresources", "translations"), 0755)
	err = BuildPackage(config.Dir, config, refresh, debugBuild, true, !config.IsApplication)
	if err != nil {
		color.Red("\nBuild Error\n")
		os.Exit(1)
	}
	printSuccess("\nFinish Build Successfully\n")
}

func Test(refresh bool) {
	config, err := LoadConfig(".", true)
	if err != nil {
		color.Red(err.Error())
		os.Exit(1)
	}
	os.MkdirAll(filepath.Join(config.Dir, "qtresources", "translations"), 0755)
	err = BuildPackage(config.Dir, config, refresh, true, true, false)
	if err != nil {
		color.Red("\nBuild Error\n")
		os.Exit(1)
	}
	buildPath := filepath.Join(config.Dir, BuildFolder(true))
	makeCmd := Command("make", buildPath, "test")
	makeCmd.AddEnv("CTEST_OUTPUT_ON_FAILURE=1")
	err = makeCmd.Run()
	if err != nil {
		color.Red("\nTest Fail\n")
		os.Exit(1)
	}
	printSuccess("\nTest Pass\n")
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
	cmd := Command(command, s.workDir, args...)
	cmd.Silent = true
	err := cmd.Run()
	if err != nil {
		s.err = fmt.Errorf("cmd: `%s %s` err: %s", command, strings.Join(args, " "), err.Error())
	}
	return s
}

func (s *sequentialRun) Finish() error {
	return s.err
}

func BuildPackage(rootPackageDir string, config *PackageConfig, refresh, debugBuild, build, install bool) error {
	var vendorPath string
	var changed bool
	err := ReleaseTranslation(rootPackageDir, config.Dir)
	if err != nil {
		return err
	}
	vendorPath = VendorFolder(rootPackageDir, debugBuild)
	os.MkdirAll(filepath.Join(vendorPath, "include"), 0755)
	os.MkdirAll(filepath.Join(vendorPath, "lib"), 0755)
	if config.IsApplication {
		changed, err = AddCMakeForApp(config, rootPackageDir, refresh, debugBuild)
		buildPath := filepath.Join(config.Dir, BuildFolder(debugBuild))
		os.MkdirAll(buildPath, 0755)
		CreateIcon(config.Dir, debugBuild)
	} else {
		changed, err = AddCMakeForLib(config, rootPackageDir, refresh, debugBuild)
		os.MkdirAll(vendorPath, 0755)
	}
	if err != nil {
		return err
	}
	if !build {
		return nil
	}
	if debugBuild {
		printSection("\nBuild Package: %s (debug)\n", config.Name)
	} else {
		printSection("\nBuild Package: %s (release)\n", config.Name)
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

func OldestUnixTime(paths ...string) int64 {
	var unixTime int64
	for _, path := range paths {
		stat, err := os.Stat(path)
		if os.IsNotExist(err) {
			return 0
		}
		if unixTime < stat.ModTime().Unix() {
			unixTime = stat.ModTime().Unix()
		}
	}
	return unixTime
}

func CreateIcon(rootPackageDir string, debugBuild bool) {
	var iconImage image.Image
	releaseBuildDir := filepath.Join(rootPackageDir, BuildFolder(false))
	resultPath1 := filepath.Join(releaseBuildDir, "MacOSXAppIcon.icns")
	resultPath2 := filepath.Join(releaseBuildDir, "WindowsAppIcon.ico")
	debugBuildDir := filepath.Join(rootPackageDir, BuildFolder(true))
	resultPath3 := filepath.Join(debugBuildDir, "MacOSXAppIcon.icns")
	resultPath4 := filepath.Join(debugBuildDir, "WindowsAppIcon.ico")
	pngPath := filepath.Join(rootPackageDir, "Resources", "icon.png")

	file, err := os.Open(pngPath)
	oldestUnixTime := OldestUnixTime(resultPath1, resultPath2, resultPath3, resultPath4)
	if err == nil {
		inputStat, err := file.Stat()
		if err == nil && oldestUnixTime > inputStat.ModTime().Unix() {
			return
		}
		iconImage, _, err = image.Decode(file)
		if err != nil {
			iconImage = nil
		}
	} else if oldestUnixTime > 0 {
		return
	}
	sourceFile := filepath.Join("Resources", "icon.png")
	if iconImage == nil {
		sourceFile = "default image"
		pngPath = filepath.Join(releaseBuildDir, "icon.png")
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
	ico.Encode(icon, iconImage)
	color.Magenta("Wrote: %s from %s\n", filepath.Join(BuildFolder(false), "WindowsAppIcon.ico"), sourceFile)

	CopyFile(resultPath2, resultPath4)
	color.Magenta("Wrote: %s from %s\n", filepath.Join(BuildFolder(true), "WindowsAppIcon.ico"), sourceFile)

	os.MkdirAll(filepath.Join(releaseBuildDir, "MacOSXAppIcon.iconset"), 0755)
	err = SequentialRun(releaseBuildDir).
		Run("sips", "-z", "16", "16", pngPath, "--out", filepath.Join(releaseBuildDir, "MacOSXAppIcon.iconset", "icon_16x16.png")).
		Run("sips", "-z", "32", "32", pngPath, "--out", filepath.Join(releaseBuildDir, "MacOSXAppIcon.iconset", "icon_16x16@2x.png")).
		Run("sips", "-z", "32", "32", pngPath, "--out", filepath.Join(releaseBuildDir, "MacOSXAppIcon.iconset", "icon_32x32.png")).
		Run("sips", "-z", "64", "64", pngPath, "--out", filepath.Join(releaseBuildDir, "MacOSXAppIcon.iconset", "icon_32x32@2x.png")).
		Run("sips", "-z", "128", "128", pngPath, "--out", filepath.Join(releaseBuildDir, "MacOSXAppIcon.iconset", "icon_128x128.png")).
		Run("sips", "-z", "256", "256", pngPath, "--out", filepath.Join(releaseBuildDir, "MacOSXAppIcon.iconset", "icon_128x128@2x.png")).
		Run("sips", "-z", "256", "256", pngPath, "--out", filepath.Join(releaseBuildDir, "MacOSXAppIcon.iconset", "icon_256x256.png")).
		Run("sips", "-z", "512", "512", pngPath, "--out", filepath.Join(releaseBuildDir, "MacOSXAppIcon.iconset", "icon_256x256@2x.png")).
		Run("sips", "-z", "512", "512", pngPath, "--out", filepath.Join(releaseBuildDir, "MacOSXAppIcon.iconset", "icon_512x512.png")).
		Run("sips", "-z", "1024", "1024", pngPath, "--out", filepath.Join(releaseBuildDir, "MacOSXAppIcon.iconset", "icon_512x512@2x.png")).
		Run("iconutil", "-c", "icns", "--output", resultPath1, filepath.Join(releaseBuildDir, "MacOSXAppIcon.iconset")).Finish()
	if err != nil {
		panic(err)
	}
	color.Magenta("Wrote: %s from %s\n", filepath.Join(BuildFolder(false), "MacOSXAppIcon.icns"), sourceFile)

	CopyFile(resultPath1, resultPath3)
	color.Magenta("Wrote: %s from %s\n", filepath.Join(BuildFolder(true), "MacOSXAppIcon.icns"), sourceFile)
}

func RunCMakeAndBuild(rootPackageDir, packageDir, vendorPath string, update, debugBuild, install bool) error {
	buildPath := filepath.Join(packageDir, BuildFolder(debugBuild))

	makefilePath := filepath.Join(buildPath, "Makefile")
	_, err := os.Stat(makefilePath)

	if update || os.IsNotExist(err) {
		printSubSection("\nRun CMake\n")
		os.MkdirAll(buildPath, 0755)
		args := []string{".."}
		if debugBuild {
			args = append(args, "-DCMAKE_BUILD_TYPE=Debug")
		} else {
			args = append(args, "-DCMAKE_BUILD_TYPE=Release")
		}
		if vendorPath != "" {
			args = append(args, "-DCMAKE_INSTALL_PREFIX="+vendorPath)
		}
		qtDir := FindQt(rootPackageDir)
		args = append(args, CMakeOptions(qtDir)...)
		cmd := Command("cmake", buildPath, args...)
		if qtDir != "" {
			cmd.AddEnv("QTDIR=" + qtDir)
		}
		err := cmd.Run()
		if err != nil {
			return err
		}
	}

	buildArgs := []string{"--build", "."}
	if debugBuild {
		buildArgs = append(buildArgs, "--config", "Debug")
	} else {
		buildArgs = append(buildArgs, "--config", "Release")
	}
	if install {
		printSubSection("\nStart Building and Installing\n")
		buildArgs = append(buildArgs, "--target", "install")
	} else {
		printSubSection("\nStart Building\n")
	}

	makeCmd := Command("cmake", buildPath, buildArgs...)
	return makeCmd.Run()
}
