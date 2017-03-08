package qtpm

import (
	"bytes"
	"errors"
	"fmt"
	"os"
	"path/filepath"
	"sort"
	"strconv"
	"strings"
	"text/template"
	"time"
)

var licenses = map[string]string{
	"apache-v2.0":   "Apache License v2.0",
	"artistic-v2.0": "The Artistic License v2.0",
	"bsd-2":         "Simplified BSD License (BSD 2-clause)",
	"bsd-3":         "Modified BSD License (BSD 3-clause)",
	"epl-v1.0":      "Eclipse Public License (EPL) v1.0",
	"gnu-agpl-v3.0": "GNU Affero General Public License (AGPL) v3.0",
	"gnu-gpl-v2.0":  "GNU General Public License (GPL) v2.0",
	"gnu-gpl-v3.0":  "GNU General Public License (GPL) v3.0",
	"gnu-lgpl-v2.1": "GNU Lesser General Public License (LGPL) v2.1",
	"gnu-lgpl-v3.0": "GNU Lesser General Public License (LGPL) v3.0",
	"mit":           "The MIT License (MIT)",
	"mpl-v2.0":      "Mozilla Public License (MPL) v2.0",
	"unlicense":     "The Unlicense (Public Domain)",
}

var aliases = map[string]string{
	"apache2":       "apache-v2.0",
	"perl":          "artistic-v2.0",
	"eclipse":       "epl-v1.0",
	"mozilla":       "mpl-v2.0",
	"gpl":           "gnu-gpl-v3.0",
	"gpl2":          "gnu-gpl-v2.0",
	"gpl3":          "gnu-gpl-v3.0",
	"lgpl":          "gnu-lgpl-v3.0",
	"lgpl2":         "gnu-lgpl-v2.1",
	"lgpl3":         "gnu-lgpl-v3.0",
	"bsd":           "bsd-3",
	"x":             "mit",
	"public domain": "unlicense",
}

type licenseVariable struct {
	Author string
	Year   string
}

func NormalizeLicense(licenseName string) (string, string, error) {
	licenseName = strings.ToLower(licenseName)
	if alias, ok := aliases[licenseName]; ok {
		licenseName = alias
	}
	if description, ok := licenses[licenseName]; ok {
		return licenseName, description, nil
	}
	keys := make([]string, 0, len(licenses))
	maxLength := 0
	for name := range licenses {
		keys = append(keys, name)
		if len(name) > maxLength {
			maxLength = len(name)
		}
	}
	sort.Strings(keys)
	var buffer bytes.Buffer
	for _, key := range keys {
		buffer.WriteString("  * ")
		buffer.WriteString(key)
		for i := len(key); i < maxLength; i++ {
			buffer.WriteByte(' ')
		}
		buffer.WriteString(" : ")
		buffer.WriteString(licenses[key])
		buffer.WriteByte('\n')
	}
	keys = make([]string, 0, len(aliases))
	maxLength = 0
	for name := range aliases {
		keys = append(keys, name)
		if len(name) > maxLength {
			maxLength = len(name)
		}
	}
	sort.Strings(keys)
	for _, key := range keys {
		buffer.WriteString("  * ")
		buffer.WriteString(key)
		for i := len(key); i < maxLength; i++ {
			buffer.WriteByte(' ')
		}
		buffer.WriteString(" -> ")
		buffer.WriteString(aliases[key])
		buffer.WriteByte('\n')
	}

	fmt.Fprintf(os.Stderr, "License '%s' is invalid. qtpm can accept following licenses:\n\n%s", licenseName, buffer.String())
	return "", "", errors.New("invalid license")
}

func WriteLicense(dir, license string) error {
	file, err := os.Create(filepath.Join(dir, "LICENSE.txt"))
	if err != nil {
		return err
	}
	now := time.Now()
	variable := licenseVariable{
		Author: UserName(),
		Year:   strconv.Itoa(now.Year()),
	}
	src := MustAsset("licenses/" + license + ".rst")
	tmp := template.Must(template.New("license").Parse(string(src)))
	return tmp.Execute(file, variable)
}
