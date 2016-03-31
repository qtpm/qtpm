package qtpm

import (
	"runtime"
	"github.com/shibukawa/configdir"
	"github.com/PuerkitoBio/goquery"
	"log"
	"io/ioutil"
	"net/http"
	"strings"
	"fmt"
	"compress/gzip"
	"archive/tar"
	"io"
	"os"
	"path/filepath"
	"github.com/fatih/color"
	"archive/zip"
	"bytes"
	"git"
)

func SetupWebAssemblyTool(update bool) {
	installEmscripten(update)
	installBinaryen(update)
}

func installBinaryen(update bool) {
	dir := configdir.New("", "qtpm").QueryCacheFolder()
	var err error
	workDir := filepath.Join(dir.Path, "binaryen")
	if !dir.Exists("binaryen") {
		err = git.CloneWithoutFilters([]string{"https://github.com/WebAssembly/binaryen.git", "binaryen"}, dir.Path)
	} else if update {
		err = Command("git", workDir, "pull", "--ff-only").Run()
	} else {
		return
	}
	if err != nil {
		log.Fatal(err)
	}
	os.MkdirAll(workDir, 0755)
	err = Command("cmake", workDir, ".").Run()
	if err != nil {
		log.Fatal(err)
	}
	err = Command("cmake", workDir, "--build", ".").Run()
}

func installEmscripten(update bool) {
	dir := configdir.New("", "qtpm").QueryCacheFolder()
	if !dir.Exists("emsdk") {
		update = true
		var path string
		if runtime.GOOS == "windows" {
			doc, err := goquery.NewDocument("https://kripken.github.io/emscripten-site/docs/getting_started/downloads.html")
			if err != nil {
				log.Fatal(err)
			}
			doc.Find("a").Each(func(_ int, s *goquery.Selection) {
				href, _ := s.Attr("href")
				if strings.HasSuffix(href, ".zip") {
					path = href
				}
			})
		} else {
			path = "https://s3.amazonaws.com/mozilla-games/emscripten/releases/emsdk-portable.tar.gz"
		}
		if path == "" {
			log.Fatal("no download path")
		}
		resp, err := http.Get(path)
		if err != nil {
			log.Fatal(err)
		}
		defer resp.Body.Close()
		if runtime.GOOS == "windows" {
			body, err := ioutil.ReadAll(resp.Body)
			if err != nil {
				return
			}
			rawReader := bytes.NewReader(body)
			reader, err := zip.NewReader(rawReader, int64(len(body)))
			if err != nil {
				return
			}
			for _, file := range reader.File {
				path := filepath.Join(dir.Path, "emsdk", file.Name)
				if file.FileInfo().IsDir() {
					os.MkdirAll(path, 0755)
					continue
				}
				outFile, err := os.Create(path)
				if err != nil {
					return
				}
				inFile, err := file.Open()
				if err != nil {
					outFile.Close()
					return
				}
				io.Copy(outFile, inFile)
				outFile.Close()
				inFile.Close()
				color.Magenta("Wrote: %s\n", file.Name)
			}
		} else {
			gzf, err := gzip.NewReader(resp.Body)
			if err != nil {
				log.Fatal(err)
			}
			tarReader := tar.NewReader(gzf)
			prefixLength := 0 // to skip emsdk_portable
			os.MkdirAll(filepath.Join(dir.Path, "emsdk"), 0755)
			for {
				header, err := tarReader.Next()

				if err == io.EOF {
					break
				}

				if err != nil {
					fmt.Println(err)
					os.Exit(1)
				}
				if prefixLength == 0 {
					prefixLength = len(header.Name)
					continue
				}
				path := filepath.Join(dir.Path, "emsdk", header.Name[prefixLength:])
				switch header.Typeflag {
				case tar.TypeDir:
					os.MkdirAll(path, 0755)
				case tar.TypeReg:
					file, err := os.Create(path)
					file.Chmod(os.FileMode(header.Mode))
					defer file.Close()
					if err != nil {
						fmt.Println(err)
						os.Exit(1)
					}
					io.Copy(file, tarReader)
					color.Magenta("Wrote: %s\n", header.Name)
				}
			}
		}
	}

	if update {
		toolPath := filepath.Join(dir.Path, "emsdk", "emsdk")
		workDir := filepath.Join(dir.Path, "emsdk")
		err := Command(toolPath, workDir, "update").Run()
		if err != nil {
			log.Fatalln(err)
		}
		err = Command(toolPath, workDir, "install", "latest").Run()
		if err != nil {
			log.Fatalln(err)
		}
		err = Command(toolPath, workDir, "activate", "latest").Run()
		if err != nil {
			log.Fatalln(err)
		}
	}
}
