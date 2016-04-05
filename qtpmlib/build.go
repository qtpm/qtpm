package qtpm

import (
	"bytes"
	"image"
	"io/ioutil"
	"os"
	"path/filepath"
	"runtime"

	"github.com/Kodeworks/golang-image-ico"
	"github.com/fatih/color"
)

func Build(refresh, debugBuild bool) {
	config := MustLoadConfig(".", true)
	os.MkdirAll(filepath.Join(config.Dir, "qtresources", "translations"), 0755)
	_, err := BuildPackage(config, config, refresh, debugBuild, true, !config.IsApplication)
	if err != nil {
		color.Red("\nBuild Error: %s\n", err.Error())
		os.Exit(1)
	}
	printSuccess("\nFinish Build Successfully\n")
}

func Test(refresh bool) {
	config := MustLoadConfig(".", true)
	os.MkdirAll(filepath.Join(config.Dir, "qtresources", "translations"), 0755)
	_, err := BuildPackage(config, config, refresh, true, true, false)
	if err != nil {
		color.Red("\nBuild Error: %s\n", err.Error())
		os.Exit(1)
	}
	buildPath := filepath.Join(config.Dir, BuildFolder(true))
	makeCmd := CMake(buildPath, false, false, "test", nil)
	makeCmd.AddEnv("CTEST_OUTPUT_ON_FAILURE=1")
	err = makeCmd.Run()
	if err != nil {
		color.Red("\nTest Fail\n")
		os.Exit(1)
	}
	printSuccess("\nTest Pass\n")
}

func BuildPackage(rootConfig, config *PackageConfig, refresh, debugBuild, build, install bool) (*ProjectDetail, error) {
	var changed bool
	var rootPackageDir string
	if rootConfig != nil {
		rootPackageDir = rootConfig.Dir
	} else {
		rootPackageDir, _ = filepath.Abs(".")
	}

	err := ReleaseTranslation(rootPackageDir, config.Dir)
	if err != nil {
		return nil, err
	}
	rootPackageBuildPath := filepath.Join(rootPackageDir, BuildFolder(debugBuild))
	dependencies, err := getRecursively(rootConfig, config.Requires, false, false, false, false)
	if err != nil {
		return nil, err
	}
	dependencies = append(dependencies, config)
	var detail *ProjectDetail
	if config.IsApplication {
		changed, detail, err = AddCMakeForApp(config, rootPackageDir, dependencies, refresh, debugBuild)
		buildPath := filepath.Join(config.Dir, BuildFolder(debugBuild))
		os.MkdirAll(buildPath, 0755)
		CreateIcon(config.Dir, debugBuild)
	} else {
		changed, detail, err = AddCMakeForLib(config, rootPackageDir, dependencies, refresh, debugBuild)
		os.MkdirAll(filepath.Join(rootPackageBuildPath, "include"), 0755)
		os.MkdirAll(filepath.Join(rootPackageBuildPath, "lib"), 0755)
	}
	if err != nil {
		return nil, err
	}
	if !build {
		return detail, nil
	}
	if debugBuild {
		printSection("\nBuild Package: %s (debug)\n", config.Name)
	} else {
		printSection("\nBuild Package: %s (release)\n", config.Name)
	}
	return detail, RunCMakeAndBuild(rootPackageDir, config, changed, debugBuild, true, install)
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
	resourceDir := filepath.Join(rootPackageDir, "Resources")
	os.MkdirAll(resourceDir, 0755)

	workDir := filepath.Join(rootPackageDir, BuildFolder(debugBuild))

	resultPath1 := filepath.Join(resourceDir, "WindowsAppIcon.ico")
	resultPath2 := filepath.Join(resourceDir, "MacOSXAppIcon.icns")
	pngPath := filepath.Join(rootPackageDir, "Resources", "icon.png")

	file, err := os.Open(pngPath)
	var oldestUnixTime int64

	if runtime.GOOS == "windows" {
		oldestUnixTime = OldestUnixTime(resultPath1)
	} else if runtime.GOOS == "darwin" {
		oldestUnixTime = OldestUnixTime(resultPath2)
	} else {
		return
	}

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
		pngPath = filepath.Join(resourceDir, "icon.png")
		ioutil.WriteFile(pngPath, MustAsset("resources/qt-logo.png"), 0644)
		iconImage, _, err = image.Decode(bytes.NewReader(MustAsset("resources/qt-logo.png")))
		if err != nil {
			panic(err) // qtpm should be able to read fallback icon anytime
		}
	}

	if runtime.GOOS == "windows" {
		icon, err := os.Create(resultPath1)
		defer icon.Close()
		if err != nil {
			panic(err)
		}
		ico.Encode(icon, iconImage)
		color.Magenta("Wrote: %s from %s\n", filepath.Join("Resources", "WindowsAppIcon.ico"), sourceFile)
	} else {
		os.MkdirAll(filepath.Join(workDir, "MacOSXAppIcon.iconset"), 0755)
		err = SequentialRun(resourceDir).
			Run("sips", "-z", "16", "16", pngPath, "--out", filepath.Join(workDir, "MacOSXAppIcon.iconset", "icon_16x16.png")).
			Run("sips", "-z", "32", "32", pngPath, "--out", filepath.Join(workDir, "MacOSXAppIcon.iconset", "icon_16x16@2x.png")).
			Run("sips", "-z", "32", "32", pngPath, "--out", filepath.Join(workDir, "MacOSXAppIcon.iconset", "icon_32x32.png")).
			Run("sips", "-z", "64", "64", pngPath, "--out", filepath.Join(workDir, "MacOSXAppIcon.iconset", "icon_32x32@2x.png")).
			Run("sips", "-z", "128", "128", pngPath, "--out", filepath.Join(workDir, "MacOSXAppIcon.iconset", "icon_128x128.png")).
			Run("sips", "-z", "256", "256", pngPath, "--out", filepath.Join(workDir, "MacOSXAppIcon.iconset", "icon_128x128@2x.png")).
			Run("sips", "-z", "256", "256", pngPath, "--out", filepath.Join(workDir, "MacOSXAppIcon.iconset", "icon_256x256.png")).
			Run("sips", "-z", "512", "512", pngPath, "--out", filepath.Join(workDir, "MacOSXAppIcon.iconset", "icon_256x256@2x.png")).
			Run("sips", "-z", "512", "512", pngPath, "--out", filepath.Join(workDir, "MacOSXAppIcon.iconset", "icon_512x512.png")).
			Run("sips", "-z", "1024", "1024", pngPath, "--out", filepath.Join(workDir, "MacOSXAppIcon.iconset", "icon_512x512@2x.png")).
			Run("iconutil", "-c", "icns", "--output", resultPath2, filepath.Join(workDir, "MacOSXAppIcon.iconset")).Finish()
		if err != nil {
			panic(err)
		}
		color.Magenta("Wrote: %s from %s\n", filepath.Join("Resources", "MacOSXAppIcon.icns"), sourceFile)
	}
}

func RunCMakeAndBuild(rootPackageDir string, packageConfig *PackageConfig, update, debug, build, install bool) error {
	buildPath := filepath.Join(packageConfig.Dir, BuildFolder(debug))
	var installPrefix string
	if rootPackageDir != packageConfig.Dir {
		installPrefix = filepath.Join(rootPackageDir, BuildFolder(debug))
	} else {
		if debug {
			installPrefix = filepath.Join(rootPackageDir, "dest", "debug")
		} else {
			installPrefix = filepath.Join(rootPackageDir, "dest", "release")
		}
		os.MkdirAll(installPrefix, 0755)
	}

	makefilePath := filepath.Join(buildPath, "Makefile")
	_, err := os.Stat(makefilePath)

	if update || os.IsNotExist(err) {
		printSubSection("\nRun CMake\n")
		os.MkdirAll(buildPath, 0755)
		args := []string{"-DCMAKE_INSTALL_PREFIX=" + installPrefix, "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"}
		if debug {
			args = append(args, "-DCMAKE_BUILD_TYPE=Debug")
		} else {
			args = append(args, "-DCMAKE_BUILD_TYPE=Release")
		}
		qtDir := FindQt(rootPackageDir)
		args = append(args, CMakeOptions(qtDir)...)
		cmd := CMake(buildPath, true, false, "", args)
		if qtDir != "" {
			cmd.AddEnv("QTDIR=" + qtDir)
		}
		err := cmd.Run()
		if err != nil {
			return err
		}
	}
	if !build {
		return nil
	}
	if rootPackageDir == packageConfig.Dir {
		var target string
		if install {
			printSubSection("\nStart Building and Installing\n")
			target = "install"
		} else {
			printSubSection("\nStart Building\n")
		}
		makeCmd := CMake(buildPath, false, debug, target, nil)
		return makeCmd.Run()
	}
	if install {
		printSubSection("\nStart Building and Installing\n")
	} else {
		printSubSection("\nStart Building\n")
	}
	makeCmd := CMake(buildPath, false, debug, libname(packageConfig.Name), nil)
	err = makeCmd.Run()
	if err != nil {
		return err
	}
	if !install {
		return nil
	}
	installCmd := CMake(buildPath, false, debug, "install/fast", nil)
	return installCmd.Run()
}
