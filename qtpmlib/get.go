package qtpm

import (
	"archive/zip"
	"bytes"
	"errors"
	"fmt"
	"git"
	"github.com/fatih/color"
	"io"
	"io/ioutil"
	"log"
	"net/http"
	"os"
	"path/filepath"
	"strings"
)

func Get(packageName string, update, useGit bool) {
	parentConfig, _ := LoadConfig(".", true)

	var packages []*PackageConfig
	var err error
	if packageName == "" && parentConfig != nil {
		packages, err = getRecursively(parentConfig, parentConfig.Requires, true, false, update, useGit)
	} else if packageName != "" {
		packages, err = getRecursively(parentConfig, []string{packageName}, true, parentConfig != nil, update, useGit)
	} else {
		fmt.Println("qtpackage.toml or package name argument are needed")
		return
	}
	if err != nil {
		log.Fatalln(err)
	}
	if parentConfig != nil {
		parentConfig.SaveIfDirty()
	}
	var dir string
	if parentConfig != nil {
		dir = parentConfig.Dir
	} else {
		dir, _ = filepath.Abs(".")
	}
	os.MkdirAll(filepath.Join(dir, "qtresources", "translations"), 0755)
	if len(packages) == 0 {
		color.Red("%s package has no dependent packages.\n", parentConfig.Name)
		os.Exit(1)
	}
	for _, packageConfig := range packages[:len(packages)-1] {
		_, err = BuildPackage(parentConfig, packageConfig, Release, update, true, false)
		if err != nil {
			continue
		}
	}
}

func getRecursively(rootPackage *PackageConfig, packages []string, download, save, update, useGit bool) ([]*PackageConfig, error) {
	loadedPackages := make(map[string]*PackageConfig)
	if rootPackage != nil {
		rootPackage.Children = loadedPackages
	}
	var edges [][2]string
	for _, packageName := range packages {
		edges = append(edges, [2]string{packageName, "*"})
	}
	nextPackages := packages
	for len(nextPackages) > 0 {
		waitingPackage := []string{}
		for _, packageName := range nextPackages {
			child, err := getSinglePackage(rootPackage, packageName, download, save, update, useGit)
			if err != nil {
				return nil, err
			}
			loadedPackages[packageName] = child
			child.PackageName = packageName
			for _, neededPackage := range child.Requires {
				edges = append(edges, [2]string{neededPackage, packageName})
				if _, ok := loadedPackages[neededPackage]; !ok && !has(waitingPackage, neededPackage) {
					waitingPackage = append(waitingPackage, neededPackage)
				}
			}
		}
		nextPackages = waitingPackage
	}
	packageNamesInBuildOrder, loops := TopologicalSort(edges)
	if len(loops) > 0 {
		var buffer bytes.Buffer
		buffer.WriteString("Circular reference issue. The following dependencies are :\n")
		for _, loop := range loops {
			fmt.Fprintf(&buffer, "  * %s -> %s\n", loop[0], loop[1])
		}
		return nil, errors.New(buffer.String())
	}
	var result []*PackageConfig
	for _, packageName := range packageNamesInBuildOrder {
		if packageName == "*" {
			if rootPackage != nil {
				result = append(result, rootPackage)
			}
		} else {
			result = append(result, loadedPackages[packageName])
		}
	}
	return result, nil
}

func has(list []string, entry string) bool {
	for _, listItem := range list {
		if listItem == entry {
			return true
		}
	}
	return false
}

func getSinglePackage(rootConfig *PackageConfig, packageName string, download, save, update, useGit bool) (*PackageConfig, error) {
	// download from git
	paths := strings.Split(packageName, "/")
	if len(paths) != 3 {
		return nil, fmt.Errorf("qtpm supports only github.com/author/repository style path: %s", packageName)
	}
	var parentDir string
	if rootConfig != nil {
		parentDir = rootConfig.Dir
	} else {
		parentDir, _ = filepath.Abs(".")
	}
	dirName := strings.Join(paths, "___")
	installDir := filepath.Join(parentDir, "vendor", dirName)
	workDir := filepath.Dir(installDir)
	os.MkdirAll(workDir, 0755)

	if _, err := os.Stat(installDir); os.IsNotExist(err) {
		if !download {
			installDir = filepath.Clean(filepath.Join(parentDir, "..", dirName))
			workDir = filepath.Dir(installDir)
			if _, err := os.Stat(installDir); os.IsNotExist(err) {
				return nil, fmt.Errorf("%s is not downloaded yet. Run qtpm get first.", packageName)
			}
		}
		if useGit || paths[0] != "github.com" {
			err = git.CloneWithoutFilters([]string{
				"--depth", "1", fmt.Sprintf("git@%s:%s/%s.git", paths[0], paths[1], paths[2]), dirName,
			}, workDir)
		} else {
			err = DownloadZip(installDir, paths[1], paths[2])
		}
		if err != nil {
			return nil, err
		}
	} else if update {
		_, err := os.Stat(filepath.Join(installDir, ".git"))
		if os.IsNotExist(err) {
			os.RemoveAll(installDir)
			err = DownloadZip(installDir, paths[1], paths[2])
		} else {
			cmd := Command("git", installDir, "pull", "--ff-only")
			err = cmd.Run()
		}
		if err != nil {
			return nil, err
		}
	}
	if save && !has(rootConfig.Requires, packageName) {
		rootConfig.Requires = append(rootConfig.Requires, packageName)
		rootConfig.DirtyFlag = true
	}
	return LoadConfig(installDir, false)
}

func DownloadZip(installDir, orgName, repoName string) error {
	os.MkdirAll(installDir, 0755)
	resp, err := http.Get(fmt.Sprintf("https://codeload.github.com/%s/%s/zip/master", orgName, repoName))
	if err != nil {
		return err
	}
	defer resp.Body.Close()
	body, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		return err
	}
	rawReader := bytes.NewReader(body)
	reader, err := zip.NewReader(rawReader, int64(len(body)))
	if err != nil {
		return err
	}
	var prefixLength int
	for i, file := range reader.File {
		if i == 0 {
			prefixLength = len(file.Name)
			continue
		}
		path := filepath.Join(installDir, file.Name[prefixLength:])
		if file.FileInfo().IsDir() {
			os.MkdirAll(path, 0755)
			continue
		}
		outFile, err := os.Create(path)
		if err != nil {
			return err
		}
		inFile, err := file.Open()
		if err != nil {
			outFile.Close()
			return err
		}
		io.Copy(outFile, inFile)
		outFile.Close()
		inFile.Close()
	}
	return nil
}
