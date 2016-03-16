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
	"runtime"
	"strings"
)

func Build(refresh, debugBuild bool) {
	config, err := LoadConfig(".", true)
	if err != nil {
		color.Red(err.Error())
		os.Exit(1)
	}
	os.MkdirAll(filepath.Join(config.Dir, "resources", "translations"), 0755)
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
	os.MkdirAll(filepath.Join(config.Dir, "resources", "translations"), 0755)
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

func CreateIcon(rootPackageDir string, debugBuild bool) {

	var iconImage image.Image
	releaseBuildDir := filepath.Join(rootPackageDir, BuildFolder(false))
	resultPath1 := filepath.Join(releaseBuildDir, "MacOSXAppIcon.icns")
	resultPath2 := filepath.Join(releaseBuildDir, "WindowsAppIcon.ico")
	pngPath := filepath.Join(rootPackageDir, "resources", "icon.png")
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
		printSection("\nCreating Icon: default image\n")
		pngPath = filepath.Join(releaseBuildDir, "icon.png")
		ioutil.WriteFile(pngPath, MustAsset("resources/qt-logo.png"), 0644)
		iconImage, _, err = image.Decode(bytes.NewReader(MustAsset("resources/qt-logo.png")))
		if err != nil {
			panic(err) // qtpm should be able to read fallback icon anytime
		}
	} else {
		printSection("Creating Icon: 'resources/icon.png'\n\n")
	}

	icon, err := os.Create(resultPath2)
	defer icon.Close()
	if err != nil {
		panic(err)
	}
	ico.Encode(icon, iconImage)

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
	debugBuildDir := filepath.Join(rootPackageDir, BuildFolder(true))
	resultPath3 := filepath.Join(debugBuildDir, "MacOSXAppIcon.icns")
	resultPath4 := filepath.Join(debugBuildDir, "WindowsAppIcon.ico")
	CopyFile(resultPath1, resultPath3)
	CopyFile(resultPath2, resultPath4)
}

func RunCMakeAndBuild(rootPackageDir, packageDir, vendorPath string, update, debugBuild, install bool) error {
	buildPath := filepath.Join(packageDir, BuildFolder(debugBuild))
	if update {
		printSubSection("\nGenerate CMakeLists.txt\n")
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
		cmd := Command("cmake", buildPath, args...)
		qtDir := FindQt(rootPackageDir)
		if qtDir != "" {
			cmd.AddEnv("QTDIR=" + qtDir)
		}
		err := cmd.Run()
		if err != nil {
			return err
		}
	}
	printSubSection("\nStart Building\n")
	makeCmd := Command("make", buildPath)

	err := makeCmd.Run()
	if err != nil {
		return err
	}
	if install {
		printSubSection("\nStart Installing\n")
		makeCmd := Command("make", buildPath, "install")
		return makeCmd.Run()
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
